#include "App1.h"

#include <random>
#include <ctime>

#include "MyClamp.h"

////
// PUBLIC:
////

App1::App1()
{
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);
	
	srand(time(NULL));

	//TEXTURES:
	textureMgr->loadTexture(L"grass", L"res/grass.jpg");
	textureMgr->loadTexture(L"metal", L"res/metal.jpg");

	//Shaders:
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	lineShader  = new LineShader(renderer->getDevice(), hwnd);

	//Light:
	light = new Light;
	light->setAmbientColour(0.25f, 0.25f, 0.25f, 1.0f);
	light->setDiffuseColour(0.75f, 0.75f, 0.75f, 1.0f);
	light->setDirection(-.5f, -1.0f, 0.0f);

	//Camera:
	camera->setPosition(-20.f, 50.f, -130.f);
	camera->setRotation(0.0f, 0.0f, 0.0f);

	//Ground:
	planeMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	
	//planeMatrix = DirectX::XMMatrixIdentity();
	planeMatrix = DirectX::XMMatrixTranslation(-50.f, 0.f, -50.f);

	//Line Mesh:
	lineMesh = new LineMesh(renderer->getDevice(), renderer->getDeviceContext());


	// Pipeline manager:
	PipelineMgrDefaultSettings defaultSettings;
	defaultSettings.geometryGenerator         = GeometryGeneratorTypes::JITTER_FORK;
	defaultSettings.diameterTransformerActive = false;
	defaultSettings.wholeTransformerActive    = false;
	defaultSettings.electrifierActive         = false;
	pipelineMgr = new PipelineMgr(defaultSettings);

	pipelineMgr->InitJitterForkGenerator(
		MyFloat3(0.f, 100.f, 0.f), //start point
		MyFloat3(0.f, 0.f, 0.f),   //end point
		10,                        //its
		.1f,                       //chaos proportion
		.7f,                       //forkProbability
		.6f                        //forkProbabilityScaleDown
	);

	pipelineMgr->InitStreamerGenerator(
		MyFloat3(0.f, 100.f, 0.f), //start point
		MyFloat3(0.f, -1.f, 0.f),  //init direction
		7000.f,                    //voltage
		35.f,                      //pressure
		0.5f,                      //pressure gradient
		10                         //max num layers
	);

	pipelineMgr->InitDiameterTransformer(
		5.f,  //initial diameter
		0.5f, //diameter scaledown
		4     //max num branch levels
	);

	pipelineMgr->InitElectrifier(
		1.f, //max segment length
		.1f  //chaos proportion to length
	);	
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();
			
	if (lineShader)
	{
		delete lineShader;
		lineShader = 0;
	}

}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	float dt = timer->getTime();
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	XMFLOAT4 backgroundColour = NIGHT_BACKGROUND_COLOUR;

	renderer->beginScene(backgroundColour.x, backgroundColour.y, backgroundColour.z, backgroundColour.w);

	// Generate the view matrix based on the camera's position.
	camera->update();

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	static bool viewLine = true;

	planeMesh->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), planeMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"metal"), light);
	lightShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	linesToRender = MyClamp(linesToRender, 0, int(lineMesh->GetLineCount()));

	if (viewLine)
	{
		for (int i = 0; i < linesToRender; i++)
		{
			lineShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, NULL, light, LIGHTNING_WHITE);

			lineMesh->sendData(renderer->getDeviceContext(), i);
			lineShader->render(renderer->getDeviceContext(), lineMesh->getIndexCount());
		}
	}

	// Render GUI
	Gui();

	// Swap the buffers
	renderer->endScene();

	return true;
}

////
// PRIVATE:
////

void App1::Gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	
	ImGui::Text("*************************************");
	
	ImGui::SliderInt(
		"Debug lines to render",
		&linesToRender,
		0,
		lineMesh->GetLineCount()
	);

	ImGui::Text("*************************************");


	if (ImGui::Button("Run whole process"))
	{
		pipelineMgr->RunProcess();
		UpdateLineMesh(pipelineMgr->GetSegments(), lineMesh);
	}
	
	//Pipeline stages:
	{
		PipelineMgrSettings* settings = pipelineMgr->GetSettings();

		//Geometry generator:
		GeometryGeneratorTypes genType = settings->GetGeometryGeneratorType();

		static std::map<GeometryGeneratorTypes, std::string> genTypesMap = {
			{GeometryGeneratorTypes::JITTER_FORK, "Jitter Fork"},
			{GeometryGeneratorTypes::STREAMER, "Streamer"}
		};

		ImGui::Text(("Current Method: " + genTypesMap[genType]).c_str());

		//Transform stages:
		ImGui::Text("Active Transformers: ");
		ImGui::Indent();
			if (settings->IsPathIdentifierActive())
			{
				ImGui::Text("- Path Identifier (auto)");
			}
			if (settings->IsDiameterTransformerActive())
			{
				ImGui::Text("- Diameter Transformer");
			}
			if (settings->IsWholeTransformerActive())
			{
				ImGui::Text("- Whole Transformer");
			}
			if (settings->IsElectrifierActive())
			{
				ImGui::Text("- Electrifier");
			}
		ImGui::Unindent();
		

		//
		if (ImGui::CollapsingHeader("Adjust Pipeline Stages"))
		{
			ImGui::ListBoxHeader("Geometry Generators");
			for (const auto& pair : genTypesMap)
			{
				std::string name            = pair.second;
				GeometryGeneratorTypes type = pair.first;

				if (ImGui::Selectable(name.c_str()))
				{
					pipelineMgr->SetGeometryGeneratorType(type);
				}
			}
			ImGui::ListBoxFooter();

			if (ImGui::Button("Toggle Diameter Transformer"))
			{
				pipelineMgr->SetDiameterTransformerActive(
					!(settings->IsDiameterTransformerActive())
				);
			}
			if (ImGui::Button("Toggle Whole Transformer"))
			{
				pipelineMgr->SetWholeTransformerActive(
					!(settings->IsWholeTransformerActive())
				);
			}
			if (ImGui::Button("Toggle Electrifier"))
			{
				pipelineMgr->SetElectifierActive(
					!(settings->IsElectrifierActive())
				);
			}

		}


	}

	//if (ImGui::CollapsingHeader("JITTER + FORK GENERATOR"))
	//{
	//	//init these static values to defaults defined above:
	//	static int iterations = 10;
	//	static float chaosProportion = .1f;
	//	static float forkProb = .7f;
	//	static float forkProbScaleDown = .6f;

	//	bool changeNow = false;

	//	changeNow = GuiSliderInt(&changeNow, "JFG iterations", &iterations, 1, 10);
	//	changeNow = GuiSliderFloat(&changeNow, "JFG chaos proportion", &chaosProportion, 0.f, MAX_CHAOS_PROPORTION);
	//	changeNow = GuiSliderFloat(&changeNow, "JFG fork probability", &forkProb, 0.f, 1.f);
	//	changeNow = GuiSliderFloat(&changeNow, "JFG fork prob scaledown", &forkProbScaleDown, 0.f, 1.f);

	//	if (changeNow)
	//	{
	//		jfg.InitParameters(
	//			iterations,
	//			chaosProportion,
	//			forkProb,
	//			forkProbScaleDown
	//		);
	//	}

	//	if (ImGui::Button("RUN JFG AND REBUILD LINE MESH"))
	//	{
	//		jfg.Run();
	//		UpdateLineMesh(jfg.GetOutput(), lineMesh);
	//	}

	//	if (ImGui::CollapsingHeader("ELECTRIFIER"))
	//	{
	//		static float maxLength = 1.f;
	//		static float chaosProportion = 0.1f;

	//		bool changeNow = false;
	//		changeNow = GuiSliderFloat(&changeNow, "E max len", &maxLength, MIN_SEGMENT_LENGTH, 5.f);
	//		changeNow = GuiSliderFloat(&changeNow, "E chaos proportion", &chaosProportion, 0.f, MAX_CHAOS_PROPORTION);
	//		
	//		if (changeNow)
	//		{
	//			electrifier->InitParameters(
	//				maxLength,
	//				chaosProportion
	//			);
	//		}

	//		if (ImGui::Button("RUN JFG, RUN E, AND REBUILD LINE MESH"))
	//		{
	//			jfg.Run();
	//			electrifier->SetInput(
	//				&(jfg.GetOutput())
	//			);
	//			electrifier->Run();
	//			UpdateLineMesh(electrifier->GetOutput(), lineMesh);
	//		}

	//		if (ImGui::Button("RUN E, AND REBUILD LINE MESH"))
	//		{
	//			electrifier->SetInput(
	//				&(jfg.GetOutput())
	//			);
	//			electrifier->Run();
	//			UpdateLineMesh(electrifier->GetOutput(), lineMesh);
	//		}
	//	}
	//}

	//static float streamerVoltage         = 7000;
	//static float streamerInitialPressure = 35.f;
	//static float streamerPressurGradient = 0.5f;
	//static int   streamerMaxLayers       = 10;

	//ImGui::SliderFloat("Streamer voltage", &streamerVoltage, 10.f, 10000.f);
	//ImGui::SliderFloat("Streamer init pressure", &streamerInitialPressure, 1.f, 100.f);
	//ImGui::SliderFloat("Streamre pressure gradient", &streamerPressurGradient, 0.f, 1.f);
	//ImGui::SliderInt("Streamer max layers", &streamerMaxLayers, 1, 20);

	//if (ImGui::Button("Run streamer method and rebuild line mesh"))
	//{
	//	sg.InitParameters(
	//		MyFloat3(0.f, 100.f, 0.f), //start point
	//		MyFloat3(0.f, -1.f, 0.f),  //init direction
	//		streamerVoltage,
	//		streamerInitialPressure,
	//		streamerPressurGradient,				
	//		streamerMaxLayers
	//	);
	//	sg.Run();
	//	UpdateLineMesh(sg.GetOutput(), lineMesh);
	//}
	

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool App1::GuiSliderInt(bool* changeFlag, const char* msg, int* i, float min, float max)
{
	return ImGui::SliderInt(msg, i, min, max) || *changeFlag;
}

bool App1::GuiSliderFloat(bool* changeFlag, const char* msg, float* f, float min, float max)
{
	return ImGui::SliderFloat(msg, f, min, max) || *changeFlag;
}

void App1::UpdateLineMesh(std::vector<Segment*>* segs, LineMesh* mesh)
{
	mesh->Clear();
	for (Segment* seg : *segs)
	{
		mesh->AddLine(
			XMFLOAT3(
				seg->GetStartPoint().x,
				seg->GetStartPoint().y,
				seg->GetStartPoint().z
			),
			XMFLOAT3(
				seg->GetEndPoint().x,
				seg->GetEndPoint().y,
				seg->GetEndPoint().z
			),
			0
		);
	}
}