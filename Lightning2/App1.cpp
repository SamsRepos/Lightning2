#include "App1.h"

#include <random>
#include <ctime>

#include "MyClamp.h"
#include "DefaultParameters.h"
#include "DebugCsvWriter.h"

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
	textureMgr->loadTexture(L"white", L"res/white.jpg");

	//Shaders:
	lightShader = new LightShader(renderer->getDevice(), hwnd);

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
			
	// Pipeline manager:
	PipelineMgrDefaultSettings defaultSettings;
	defaultSettings.geometryGenerator         = GeometryGeneratorTypes::STREAMER;
	defaultSettings.diameterTransformerActive = true;
	defaultSettings.wholeTransformerActive    = false;
	defaultSettings.electrifierActive         = false;
	defaultSettings.lineRendererActive        = true;
	defaultSettings.cylinderRendererActive    = true;
	pipelineMgr = new PipelineMgr(defaultSettings);

	pipelineMgr->InitJitterForkGenerator(
		DEFAULT_JFG_START_PT,
		DEFAULT_JFG_END_PT,
		DEFAULT_JFG_ITERATIONS,
		DEFAULT_JFG_CHAOS_MEAN,
		DEFAULT_JFG_CHAOS_STDDEV,
		DEFAULT_JFG_MIDPOINT_STDDEV,
		DEFAULT_JFG_BASELINE_FORK_PROB, 
		DEFAULT_JFG_FORK_PROB_SCALEDOWN
	);

	pipelineMgr->InitStreamerGenerator(
		DEFAULT_SG_START_PT,
		DEFAULT_SG_INITIAL_DIRECTION,
		DEFAULT_SG_INITIAL_VOLTAGE,
		DEFAULT_SG_INITIAL_PRESSURE,
		DEFAULT_SG_PRESSURE_GRADIENT,
		DEFAULT_SG_MAX_NUM_LAYERS
	);

	pipelineMgr->InitDiameterTransformer(
		DEFAULT_DT_INITIAL_DIAMETER,
		DEFAULT_DT_DIAMETER_SCALEDOWN,
		DEFAULT_DT_MAX_NUM_BRANCH_LEVELS
	);

	pipelineMgr->InitElectrifier(
		DEFAULT_E_MAX_SEG_LENGTH,
		DEFAULT_E_CHAOS_MEAN,
		DEFAULT_E_CHAOS_STDDEV
	);

	pipelineMgr->InitLineRenderer(renderer, hwnd);
	pipelineMgr->InitCylinderRenderer(renderer, hwnd, textureMgr->getTexture(L"white"));
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

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

	
	pipelineMgr->RenderOutput(
		renderer,
		worldMatrix,
		viewMatrix,
		projectionMatrix
	);
	
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

#if 0
	ImGui::SliderInt(
		"Debug lines to render",
		&linesToRender,
		0,
		lineMesh->GetLineCount()
	);
#endif

	ImGui::Text("*************************************");


	if (ImGui::Button("Run whole process"))
	{
		pipelineMgr->RunProcess();
		DebugWriteCsv(pipelineMgr->GetSegments());
	}

	// Pipeline stages:
	{
		PipelineMgrSettings* settings = pipelineMgr->GetSettings();

		//Geometry generator:
		GeometryGeneratorTypes genType = settings->GetGeometryGeneratorType();

		static std::map<GeometryGeneratorTypes, std::string> genTypesMap = {
			{GeometryGeneratorTypes::JITTER_FORK, "Jitter + Fork"},
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

		//Renderers:
		ImGui::Text("Active Renderers: ");
		ImGui::Indent();
			if (settings->IsLineRendererActive())
			{
				ImGui::Text("- Line Renderer");
			}
			if (settings->IsCylinderRendererActive())
			{
				ImGui::Text("- Cylinder Renderer");
			}			
		ImGui::Unindent();

		// Toggle pipeline stages
		if (ImGui::CollapsingHeader("Toggle Pipeline Stages"))
		{
			ImGui::ListBoxHeader("Geometry Generators");
			for (const auto& pair : genTypesMap)
			{
				std::string name = pair.second;
				GeometryGeneratorTypes type = pair.first;

				if (ImGui::Selectable(name.c_str()))
				{
					pipelineMgr->SetGeometryGeneratorType(type);
				}
			}
			ImGui::ListBoxFooter();

			std::string buttonMsgNow = "Toggle Diameter Transformer ";
			buttonMsgNow.append((settings->IsDiameterTransformerActive()) ? "Off" : "On");
			if (ImGui::Button(buttonMsgNow.c_str()))					
			{
				pipelineMgr->SetDiameterTransformerActive(
					!(settings->IsDiameterTransformerActive())
				);
			}
			
			buttonMsgNow = "Toggle Whole Transformer ";
			buttonMsgNow.append((settings->IsWholeTransformerActive()) ? "Off" : "On");
			if (ImGui::Button(buttonMsgNow.c_str()))
			{
				pipelineMgr->SetWholeTransformerActive(
					!(settings->IsWholeTransformerActive())
				);
			}
			
			buttonMsgNow = "Toggle Electrifier ";
			buttonMsgNow.append((settings->IsElectrifierActive()) ? "Off" : "On");
			if (ImGui::Button(buttonMsgNow.c_str()))
			{
				pipelineMgr->SetElectifierActive(
					!(settings->IsElectrifierActive())
				);
			}
		}

		//Toggle renderers:
		if (ImGui::CollapsingHeader("Toggle Renderers"))
		{
			std::string buttonMsgNow = "Toggle Line Renderer ";
			buttonMsgNow.append((settings->IsLineRendererActive()) ? "Off" : "On");
			if (ImGui::Button(buttonMsgNow.c_str()))
			{
				pipelineMgr->SetLineRendererActive(
					!(settings->IsLineRendererActive())
				);
			}

			buttonMsgNow = "Toggle Cylinder Renderer ";
			buttonMsgNow.append((settings->IsCylinderRendererActive()) ? "Off" : "On");
			if (ImGui::Button(buttonMsgNow.c_str()))
			{
				pipelineMgr->SetCylinderRendererActive(
					!(settings->IsCylinderRendererActive())
				);
			}
		}
	}

	//Adjust Jitter+Fork Method Parameters
	{
		static MyFloat3 startPt        = DEFAULT_JFG_START_PT;
		static MyFloat3 endPt          = DEFAULT_JFG_END_PT;
		static int iterations          = DEFAULT_JFG_ITERATIONS;
		static float chaosMean         = DEFAULT_JFG_CHAOS_MEAN;
		static float chaosStdDev       = DEFAULT_JFG_CHAOS_STDDEV;
		static float midpointStdDev    = DEFAULT_JFG_MIDPOINT_STDDEV;
		static float baselineForkProb  = DEFAULT_JFG_BASELINE_FORK_PROB;
		static float forkProbScaledown = DEFAULT_JFG_FORK_PROB_SCALEDOWN;

		if (ImGui::CollapsingHeader("Set Jitter+Fork Parameters"))
		{
			bool changeNow = false;
			// TODO - gui for Start and End points
			changeNow = GuiSliderInt(&changeNow, "JFG iterations", &iterations, JFG_MIN_ITERATIONS, JFG_MAX_ITERATIONS);
			changeNow = GuiSliderFloat(&changeNow, "JFG chaos mean", &chaosMean, JFG_MIN_CHAOS_MEAN, JFG_MAX_CHAOS_MEAN);
			changeNow = GuiSliderFloat(&changeNow, "JFG chaos std dev", &chaosStdDev, JFG_MIN_CHAOS_STDDEV, JFG_MAX_CHAOS_STDDEV);
			changeNow = GuiSliderFloat(&changeNow, "JFG midpoint std dev", &midpointStdDev, JFG_MIN_MIDPOINT_STDDEV, JFG_MAX_MIDPOINT_STDDEV);
			changeNow = GuiSliderFloat(&changeNow, "JFG baseline fork probability", &baselineForkProb, JFG_MIN_BASELINE_FORK_PROB, JFG_MAX_BASELINE_FORK_PROB);
			changeNow = GuiSliderFloat(&changeNow, "JFG fork prob scaledown", &forkProbScaledown, JFG_MIN_FORK_PROB_SCALEDOWN, JFG_MAX_FORK_PROB_SCALEDOWN);

			if (changeNow)
			{
				pipelineMgr->InitJitterForkGenerator(
					startPt,
					endPt,
					iterations,
					chaosMean,
					chaosStdDev,
					midpointStdDev,
					baselineForkProb,
					forkProbScaledown
				);
			}
		}
	}

	//Adjust streamer parameters:
	{
		static MyFloat3 startPt          = DEFAULT_SG_START_PT;
		static MyFloat3 initialDirection = DEFAULT_SG_INITIAL_DIRECTION;
		static float initialVoltage      = DEFAULT_SG_INITIAL_VOLTAGE;
		static float initialPressure     = DEFAULT_SG_INITIAL_PRESSURE;
		static float pressureGradient    = DEFAULT_SG_PRESSURE_GRADIENT;
		static int   maxNumLayers        = DEFAULT_SG_MAX_NUM_LAYERS;

		if (ImGui::CollapsingHeader("Set Streamer Parameters"))
		{

			bool changeNow = false;
			//TODO MyFloat3 gui
			changeNow = GuiSliderFloat(&changeNow, "SG initial voltage", &initialVoltage, SG_MIN_INITIAL_VOLTAGE, SG_MAX_INITIAL_VOLTAGE);
			changeNow = GuiSliderFloat(&changeNow, "SG initial pressure", &initialPressure, SG_MIN_INITIAL_PRESSURE, SG_MAX_INITIAL_PRESSURE);
			changeNow = GuiSliderFloat(&changeNow, "SG pressure gradient", &pressureGradient, SG_MIN_PRESSURE_GRADIENT, SG_MAX_PRESSURE_GRADIENT);
			changeNow = GuiSliderInt(&changeNow, "SG max num layers", &maxNumLayers, SG_MIN_MAX_NUM_LAYERS, SG_MAX_MAX_NUM_LAYERS);

			if (changeNow)
			{
				pipelineMgr->InitStreamerGenerator(
					startPt,
					initialDirection,
					initialVoltage,
					initialPressure,
					pressureGradient,
					maxNumLayers
				);
			}
		}		
	}

	//Adjust Diameter Transformer Parameters
	{
		static float initialDiameter   = DEFAULT_DT_INITIAL_DIAMETER;
		static float diameterScaledown = DEFAULT_DT_DIAMETER_SCALEDOWN;
		static int maxNumBranchLevels  = DEFAULT_DT_MAX_NUM_BRANCH_LEVELS;

		if (ImGui::CollapsingHeader("Set Diameter Transformer Parameters"))
		{
			bool changeNow = false;
			changeNow = GuiSliderFloat(&changeNow, "DT initial diameter", &initialDiameter, DT_MIN_INITIAL_DIAMETER, DT_MAX_INITIAL_DIAMETER);
			changeNow = GuiSliderFloat(&changeNow, "DT diameter scaledown", &diameterScaledown, DT_MIN_DIAMETER_SCALEDOWN, DT_MAX_DIAMETER_SCALEDOWN);
			changeNow = GuiSliderInt(&changeNow, "DT max num branch levels", &maxNumBranchLevels, DT_MIN_MAX_NUM_BRANCH_LEVELS, DT_MAX_MAX_NUM_BRANCH_LEVELS);

			if (changeNow)
			{
				pipelineMgr->InitDiameterTransformer(
					initialDiameter,
					diameterScaledown,
					maxNumBranchLevels
				);
			}
		}
	}

	//Adjust Electrifier Parameters
	{
		static float maxSegmentLength = DEFAULT_E_MAX_SEG_LENGTH;
		static float chaosMean        = DEFAULT_E_CHAOS_MEAN;
		static float chaosStdDev      = DEFAULT_E_CHAOS_STDDEV;

		if (ImGui::CollapsingHeader("Set Electrifier Parameters"))
		{

			bool changeNow = false;
			changeNow = GuiSliderFloat(&changeNow, "E max len", &maxSegmentLength, E_MIN_MAX_SEG_LENGTH, E_MAX_MAX_SEG_LENGTH);
			changeNow = GuiSliderFloat(&changeNow, "E chaos mean", &chaosMean, E_MIN_CHAOS_MEAN, E_MAX_CHAOS_MEAN);
			changeNow = GuiSliderFloat(&changeNow, "E chaos std dev", &chaosStdDev, E_MIN_CHAOS_STDDEV, E_MAX_CHAOS_STDDEV);

			if (changeNow)
			{
				pipelineMgr->InitElectrifier(
					maxSegmentLength,
					chaosMean,
					chaosStdDev
				);
			}						
		}
	}



// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool App1::GuiSliderInt(bool* changeFlag, const char* msg, int* i, int min, int max)
{
	return ImGui::SliderInt(msg, i, min, max) || *changeFlag;
}

bool App1::GuiSliderFloat(bool* changeFlag, const char* msg, float* f, float min, float max)
{
	return ImGui::SliderFloat(msg, f, min, max) || *changeFlag;
}
