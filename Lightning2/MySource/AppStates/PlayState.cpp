#include "PlayState.h"

#include "Maths/MyClamp.h"
#include "Utils/DebugCsvWriter.h"
#include "Utils/MyGuiUtil.h"
#include "Utils/MyInputUtil.h"

////
// PUBLIC:
////

PlayState::PlayState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight, Input* _input, FPCamera* _camera, TextureManager* _textureMgr)
	:
	BaseState::BaseState(_renderer, _hwnd, _screenWidth, _screenHeight, _input),
	pipelineMgr(NULL),
	lightShader(NULL),
	light(NULL),
	planeMesh(NULL),
	camera(_camera),
	textureMgr(_textureMgr)
{
	//PIPELINE MANAGER:
	defaultSettings.geometryGenerator      = GeometryGeneratorTypes::STREAMER;
	defaultSettings.diameterThinnerActive  = false;
	defaultSettings.wholeTransformerActive = false;
	defaultSettings.branchifierActive      = false;
	defaultSettings.electrifierActive      = false;
	defaultSettings.renderingActive        = true;	

	pipelineMgr = new PipelineMgr(
		defaultSettings,
		renderer,
		hwnd,
		screenWidth,
		screenHeight
	);

	//PLANE MESH:
	// Shaders:
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	// Light:
	light = new Light;
	light->setAmbientColour(0.25f, 0.25f, 0.25f, 1.0f);
	light->setDiffuseColour(0.75f, 0.75f, 0.75f, 1.0f);
	light->setDirection(-.5f, -1.0f, 0.0f);
	// Mesh
	planeMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	planeMatrix = DirectX::XMMatrixTranslation(-50.f, 0.f, -50.f);
}

PlayState::~PlayState()
{
	if (pipelineMgr)
	{
		delete pipelineMgr;
		pipelineMgr = NULL;
	}

	if (lightShader)
	{
		delete lightShader;
		lightShader = NULL;
	}
	if (light)
	{
		delete light;
		light = NULL;
	}
	if (planeMesh)
	{
		delete planeMesh;
		planeMesh = NULL;
	}
}

void PlayState::Init()
{
	pipelineMgr->GetSettings()->Init(defaultSettings);	

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
		DEFAULT_SG_VOLTAGE,
		DEFAULT_SG_INITIAL_PRESSURE,
		DEFAULT_SG_PRESSURE_GRADIENT,
		DEFAULT_SG_MAX_NUM_LAYERS,
		ANGLE_FIX_OPTIONS.at(DEFAULT_SG_ANGLE_FIX),
		GAS_COMPOSITION_OPTIONS.at(DEFAULT_SG_GAS_COMPOSITION)
	);

	pipelineMgr->InitDiameterThinner(
		DEFAULT_DT_SCALEDOWN
	);

	pipelineMgr->InitWholeTransformer(
		DEFAULT_WT_START_POINT,
		DEFAULT_WT_END_POINT
	);

	pipelineMgr->InitBranchifier(
		DEFAULT_B_INITIAL_DIAMETER,
		DEFAULT_B_DIAMETER_SCALEDOWN,
		DEFUALT_B_ANIMATION_TIME,
		DEFAULT_B_MAX_NUM_BRANCH_LEVELS
	);

	pipelineMgr->InitElectrifier(
		DEFAULT_E_MAX_SEG_LENGTH,
		DEFAULT_E_CHAOS_MEAN,
		DEFAULT_E_CHAOS_STDDEV
	);

	LightningRenderer* lightningRenderer = pipelineMgr->GetLightningRenderer();
	
	lightningRenderer->SetRenderMode(RENDER_MODE_OPTIONS.at(DEFAULT_RENDER_MODE));

	lightningRenderer->SetColours(
		COLOUR_OPTIONS.at(DEFAULT_BACKGROUND_COLOUR),
		COLOUR_OPTIONS.at(DEFAULT_BLUR_COLOUR),
		COLOUR_OPTIONS.at(DEFAULT_LINE_COLOUR),
		COLOUR_OPTIONS.at(DEFAULT_CAPSULE_COLOUR)
	);

	lightningRenderer->SetBlurParams(
		DEFAULT_BLUR_DIRECTIONS,
		DEFAULT_BLUR_QUALITY,
		DEFAULT_BLUR_SIZE,
		DEFAULT_BLUR_ADJUSTMENT
	);

	lightningRenderer->SetAnimationParams(
		DEFAULT_ANIM_SPEED
	);

	// Running the process at init
	// This helps with profiling
	pipelineMgr->RunProcess();
}

void PlayState::Update(float _dt)
{
	BaseState::Update(_dt);

	HandleInput();
	
	if (animatingNow)
	{
		pipelineMgr->UpdateAnimation(_dt);
	}
		
}

void PlayState::Render()
{
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	if (!(pipelineMgr->GetLightningRenderer()->IsBlurRenderingActive()))
	{
		planeMesh->sendData(renderer->getDeviceContext());
		lightShader->SetShaderParameters(renderer->getDeviceContext(), planeMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"metal"));
		lightShader->SetLight(renderer->getDeviceContext(), light);
		lightShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());
	}

	pipelineMgr->RenderOutput(
		camera,
		worldMatrix,
		viewMatrix,
		projectionMatrix
	);
}

void PlayState::Gui()
{
	ImGui::Text("PLAY STATE");

	ImGui::Checkbox("Write debug CSV", &debugCsv);

	if (ImGui::Button("[R]un process"))
	{
		pipelineMgr->RunProcess();
		if (debugCsv)
		{
			DebugWriteCsv(pipelineMgr->GetSegments());
		}
	}

	if (ImGui::Button("[C]lear segments"))
	{
		pipelineMgr->Clear();
	}

	ImGui::Checkbox("A[N]imating now", &animatingNow);
	
	ImGui::Checkbox("[Z]APPY", &zappy);
	
	if (zappy)
	{
		static float currentTime = 0.f;
		static float period = 0.5f;
		ImGui::SliderFloat("period", &period, 0.01f, .5f);

		currentTime += dt;
		if (currentTime >= period)
		{
			currentTime = 0.f;
			pipelineMgr->RunProcess();
			if (debugCsv)
			{
				DebugWriteCsv(pipelineMgr->GetSegments());
			}
		}
	}

	PipelineMgrSettings* settings = pipelineMgr->GetSettings();
	LightningRenderer* lightningRenderer = pipelineMgr->GetLightningRenderer();

	if (ImGui::Button("[I]nit Animation"))
	{
		lightningRenderer->InitAnimation();
	}

	// Pipeline stages:
	{

		//Geometry generator:
		GeometryGeneratorTypes genType = settings->GetGeometryGeneratorType();
		static std::string currentGenerator = "";
		switch (genType)
		{
		case(GeometryGeneratorTypes::JITTER_FORK):
			currentGenerator = "Jitter + Fork";
			break;
		case(GeometryGeneratorTypes::STREAMER):
			currentGenerator = "Streamer";
			break;
		}

		ImGui::Text("Current Generator: ");
		ImGui::Indent();
		ImGui::Text(("- " + currentGenerator).c_str());
		ImGui::Unindent();

		//Transform stages:
		ImGui::Text("Active Transformers: ");
		ImGui::Indent();		
		if (settings->IsPathIdentifierActive())
		{
			ImGui::Text("- Path Identifier (auto)");
		}
		if (settings->IsWholeTransformerActive())
		{
			ImGui::Text("- Whole Transformer");
		}
		if (settings->IsBranchifierActive())
		{
			ImGui::Text("- Branchifier");
		}
		if (settings->IsDiameterThinnerActive())
		{
			ImGui::Text("- Diameter Thinner");
		}
		if (settings->IsElectrifierActive())
		{
			ImGui::Text("- Electrifier");
		}
		ImGui::Unindent();

		//Renderers:
		ImGui::Text("Active Rendering Stages: ");
		ImGui::Indent();
		if (lightningRenderer->IsBlurRenderingActive())
		{
			ImGui::Text("- Blur Rendering");
		}
		if (lightningRenderer->IsLineRenderingActive())
		{
			ImGui::Text("- Line Rendering");
		}
		if (lightningRenderer->IsCapsuleRenderingActive())
		{
			ImGui::Text("- Capsule Rendering");
		}
		ImGui::Unindent();

		//Warning that recursive cap has been hit:
		if (pipelineMgr->WasRecursCapHit())
		{
			ImGui::Text("**************************");
			ImGui::Text("WARNING: Recursive cap hit by the following stages:");
			ImGui::Text(pipelineMgr->WhichStagesHitRecursCap().c_str());
			ImGui::Text("**************************");
		}

		//Geometry Generator:
		static std::map<std::string, GeometryGeneratorTypes> genTypesMap = {
				{"Jitter + Fork", GeometryGeneratorTypes::JITTER_FORK},
				{"Streamer", GeometryGeneratorTypes::STREAMER}
		};

		if (ImGui::CollapsingHeader("Select Geometry Generator"))
		{
			bool changeNow = false;
			changeNow = GuiListBox(changeNow, genTypesMap, "Geometry Generator", &currentGenerator);

			if (changeNow)
			{
				settings->SetGeometryGeneratorType(genTypesMap[currentGenerator]);
			}
		}

		// Toggle pipeline stages
		if (ImGui::CollapsingHeader("Toggle Transformer Stages"))
		{
			if (GuiToggleBox("Whole Transformer", settings->IsWholeTransformerActive()))
			{
				pipelineMgr->SetWholeTransformerActive(
					!(settings->IsWholeTransformerActive())
				);
			}

			if (GuiToggleBox("Branchifier", settings->IsBranchifierActive()))
			{
				pipelineMgr->SetBranchifierActive(
					!(settings->IsBranchifierActive())
				);
			}

			if (GuiToggleBox("Diameter Thinner", settings->IsDiameterThinnerActive()))
			{
				pipelineMgr->SetDiameterThinnerActive(
					!(settings->IsDiameterThinnerActive())
				);
			}

			if (GuiToggleBox("Electrifier", settings->IsElectrifierActive()))
			{
				pipelineMgr->SetElectifierActive(
					!(settings->IsElectrifierActive())
				);
			}
		}

		//Toggle renderers:
		if (ImGui::CollapsingHeader("Toggle Rendering Stages"))
		{
			if (GuiToggleBox("Blur Rendering", lightningRenderer->IsBlurRenderingActive()))
			{
				pipelineMgr->SetBlurRenderingActive(
					!(lightningRenderer->IsBlurRenderingActive())
				);
			}

			if (GuiToggleBox("Line Rendering", lightningRenderer->IsLineRenderingActive()))
			{
				pipelineMgr->SetLineRenderingActive(
					!(lightningRenderer->IsLineRenderingActive())
				);
			}

			if (GuiToggleBox("Capsule Rendering", lightningRenderer->IsCapsuleRenderingActive()))
			{
				pipelineMgr->SetCapsuleRenderingActive(
					!(lightningRenderer->IsCapsuleRenderingActive())
				);
			}
		}
	}

	//Adjust Jitter+Fork Method Parameters
	{
		static MyFloat3 startPt = DEFAULT_JFG_START_PT;
		static MyFloat3 endPt = DEFAULT_JFG_END_PT;
		static int iterations = DEFAULT_JFG_ITERATIONS;
		static float chaosMean = DEFAULT_JFG_CHAOS_MEAN;
		static float chaosStdDev = DEFAULT_JFG_CHAOS_STDDEV;
		static float midpointStdDev = DEFAULT_JFG_MIDPOINT_STDDEV;
		static float baselineForkProb = DEFAULT_JFG_BASELINE_FORK_PROB;
		static float forkProbScaledown = DEFAULT_JFG_FORK_PROB_SCALEDOWN;

		if (ImGui::CollapsingHeader("Set Jitter+Fork Parameters"))
		{
			bool changeNow = false;
			
			changeNow = GuiMyFloat3(changeNow, "JFG start pt", &startPt);
			changeNow = GuiMyFloat3(changeNow, "JFG end pt", &endPt);
			changeNow = GuiSliderInt(changeNow, "JFG iterations", &iterations, JFG_MIN_ITERATIONS, JFG_MAX_ITERATIONS);
			changeNow = GuiSliderFloat(changeNow, "JFG chaos mean", &chaosMean, JFG_MIN_CHAOS_MEAN, JFG_MAX_CHAOS_MEAN);
			changeNow = GuiSliderFloat(changeNow, "JFG chaos std dev", &chaosStdDev, JFG_MIN_CHAOS_STDDEV, JFG_MAX_CHAOS_STDDEV);
			changeNow = GuiSliderFloat(changeNow, "JFG midpoint std dev", &midpointStdDev, JFG_MIN_MIDPOINT_STDDEV, JFG_MAX_MIDPOINT_STDDEV);
			changeNow = GuiSliderFloat(changeNow, "JFG baseline fork probability", &baselineForkProb, JFG_MIN_BASELINE_FORK_PROB, JFG_MAX_BASELINE_FORK_PROB);
			changeNow = GuiSliderFloat(changeNow, "JFG fork prob scaledown", &forkProbScaledown, JFG_MIN_FORK_PROB_SCALEDOWN, JFG_MAX_FORK_PROB_SCALEDOWN);

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
		static MyFloat3 startPt = DEFAULT_SG_START_PT;
		static MyFloat3 initialDirection = DEFAULT_SG_INITIAL_DIRECTION;
		static float voltage = DEFAULT_SG_VOLTAGE;
		static float initialPressure = DEFAULT_SG_INITIAL_PRESSURE;
		static float pressureGradient = DEFAULT_SG_PRESSURE_GRADIENT;
		static int   maxNumLayers = DEFAULT_SG_MAX_NUM_LAYERS;
		static std::string angleFixMethod = DEFAULT_SG_ANGLE_FIX;
		static std::string gasComposition = DEFAULT_SG_GAS_COMPOSITION;

		if (ImGui::CollapsingHeader("Set Streamer Parameters"))
		{

			bool changeNow = false;

			changeNow = GuiMyFloat3(changeNow, "SG start pt", &startPt);
			changeNow = GuiMyFloat3(changeNow, "SG initial direction", &initialDirection, -1.f, 1.f);
			changeNow = GuiSliderFloat(changeNow, "SG initial voltage", &voltage, SG_MIN_VOLTAGE, SG_MAX_VOLTAGE);
			changeNow = GuiSliderFloat(changeNow, "SG initial pressure", &initialPressure, SG_MIN_INITIAL_PRESSURE, SG_MAX_INITIAL_PRESSURE);
			changeNow = GuiSliderFloat(changeNow, "SG pressure gradient", &pressureGradient, SG_MIN_PRESSURE_GRADIENT, SG_MAX_PRESSURE_GRADIENT);
			changeNow = GuiSliderInt(changeNow, "SG max num layers", &maxNumLayers, SG_MIN_MAX_NUM_LAYERS, SG_MAX_MAX_NUM_LAYERS);
			changeNow = GuiListBox(changeNow, ANGLE_FIX_OPTIONS, "SG angle fix", &angleFixMethod);
			changeNow = GuiListBox(changeNow, GAS_COMPOSITION_OPTIONS, "SG gas composition", &gasComposition);

			if (changeNow)
			{
				pipelineMgr->InitStreamerGenerator(
					startPt,
					initialDirection,
					voltage,
					initialPressure,
					pressureGradient,
					maxNumLayers,
					ANGLE_FIX_OPTIONS.at(angleFixMethod),
					GAS_COMPOSITION_OPTIONS.at(gasComposition)
				);
			}
		}
	}

	//Adjust Whole Transformer Parameters:
	{
		static MyFloat3 startPoint = DEFAULT_WT_START_POINT;
		static MyFloat3 endPoint = DEFAULT_WT_END_POINT;

		if (ImGui::CollapsingHeader("Set Whole Transformer Parameters"))
		{
			bool changeNow = false;
			
			changeNow = GuiMyFloat3(changeNow, "WT start pt", &startPoint);
			changeNow = GuiMyFloat3(changeNow, "WT end pt", &endPoint);

			if (changeNow)
			{
				pipelineMgr->InitWholeTransformer(
					startPoint,
					endPoint
				);
			}
		}
	}

	//Adjust Branchifier Parameters
	{
		static float initialDiameter = DEFAULT_B_INITIAL_DIAMETER;
		static float diameterScaledown = DEFAULT_B_DIAMETER_SCALEDOWN;
		static float animationTime = DEFUALT_B_ANIMATION_TIME;
		static int maxNumBranchLevels = DEFAULT_B_MAX_NUM_BRANCH_LEVELS;

		if (ImGui::CollapsingHeader("Set Branchifier Parameters"))
		{
			bool changeNow = false;

			changeNow = GuiSliderFloat(changeNow, "B initial diameter", &initialDiameter, B_MIN_INITIAL_DIAMETER, B_MAX_INITIAL_DIAMETER);
			changeNow = GuiSliderFloat(changeNow, "B diameter scaledown", &diameterScaledown, B_MIN_DIAMETER_SCALEDOWN, B_MAX_DIAMETER_SCALEDOWN);
			changeNow = GuiSliderFloat(changeNow, "B animation time", &animationTime, B_MIN_ANIMATION_TIME, B_MAX_ANIMATION_TIME);
			changeNow = GuiSliderInt(changeNow, "B max num branch levels", &maxNumBranchLevels, B_MIN_MAX_NUM_BRANCH_LEVELS, B_MAX_MAX_NUM_BRANCH_LEVELS);
			

			if (changeNow)
			{
				pipelineMgr->InitBranchifier(
					initialDiameter,
					diameterScaledown,
					animationTime,
					maxNumBranchLevels
				);
			}
		}
	}

	//Adjust Diameter Thinner Parameters:
	{
		static float scaledown = DEFAULT_DT_SCALEDOWN;

		if (ImGui::CollapsingHeader("Set Diameter Thinner Parameters"))
		{
			bool changeNow = false;

			changeNow = GuiSliderFloat(changeNow, "DT scaledown", &scaledown, DT_MIN_SCALEDOWN, DT_MAX_SCALEDOWN);

			if (changeNow)
			{
				pipelineMgr->InitDiameterThinner(
					scaledown
				);
			}
		}
	}

	//Adjust Electrifier Parameters
	{
		static float maxSegmentLength = DEFAULT_E_MAX_SEG_LENGTH;
		static float chaosMean = DEFAULT_E_CHAOS_MEAN;
		static float chaosStdDev = DEFAULT_E_CHAOS_STDDEV;

		if (ImGui::CollapsingHeader("Set Electrifier Parameters"))
		{
			bool changeNow = false;

			changeNow = GuiSliderFloat(changeNow, "E max len", &maxSegmentLength, E_MIN_MAX_SEG_LENGTH, E_MAX_MAX_SEG_LENGTH);
			changeNow = GuiSliderFloat(changeNow, "E chaos mean", &chaosMean, E_MIN_CHAOS_MEAN, E_MAX_CHAOS_MEAN);
			changeNow = GuiSliderFloat(changeNow, "E chaos std dev", &chaosStdDev, E_MIN_CHAOS_STDDEV, E_MAX_CHAOS_STDDEV);

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

	// Set Render Mode:
	{
		if (ImGui::CollapsingHeader("Set Render [M]ode"))
		{
			bool changeNow = false;

			changeNow = GuiListBox(changeNow, RENDER_MODE_OPTIONS, "Render Mode", &renderMode);

			if (changeNow)
			{
				lightningRenderer->SetRenderMode(
					RENDER_MODE_OPTIONS.at(renderMode)
				);
			}
		}
	}

	//Adjust Colour Parameters:
	{
		static std::string backgroundColour = DEFAULT_BACKGROUND_COLOUR;
		static std::string blurColour = DEFAULT_BLUR_COLOUR;
		static std::string lineColour = DEFAULT_LINE_COLOUR;
		static std::string cylinderColour = DEFAULT_CAPSULE_COLOUR;

		if (ImGui::CollapsingHeader("Set Colour Parameters"))
		{
			bool changeNow = false;

			changeNow = GuiListBox(changeNow, COLOUR_OPTIONS, "Background Colour", &backgroundColour);
			changeNow = GuiListBox(changeNow, COLOUR_OPTIONS, "Blur Colour", &blurColour);
			changeNow = GuiListBox(changeNow, COLOUR_OPTIONS, "Line Colour", &lineColour);
			changeNow = GuiListBox(changeNow, COLOUR_OPTIONS, "Cylinder Colour", &cylinderColour);

			if (changeNow)
			{
				lightningRenderer->SetColours(
					COLOUR_OPTIONS.at(backgroundColour),
					COLOUR_OPTIONS.at(blurColour),
					COLOUR_OPTIONS.at(lineColour),
					COLOUR_OPTIONS.at(cylinderColour)
				);
			}
		}
	}
	
	//Adjust Blur Parameters:
	{
		static float blurDirections = DEFAULT_BLUR_DIRECTIONS;
		static float blurQuality = DEFAULT_BLUR_QUALITY;
		static float blurSize = DEFAULT_BLUR_SIZE;
		static float blurAdjustment= DEFAULT_BLUR_ADJUSTMENT;

		if (ImGui::CollapsingHeader("Set Blur Parameters"))
		{
			bool changeNow = false;

			changeNow = GuiSliderFloat(changeNow, "Blur directions", &blurDirections, BLUR_MIN_DIRECTIONS, BLUR_MAX_DIRECTIONS);
			changeNow = GuiSliderFloat(changeNow, "Blur quality", &blurQuality, BLUR_MIN_QUALITY, BLUR_MAX_QUALITY);
			changeNow = GuiSliderFloat(changeNow, "Blur size", &blurSize, BLUR_MIN_SIZE, BLUR_MAX_SIZE);
			changeNow = GuiSliderFloat(changeNow, "Blur final adjustment", &blurAdjustment, BLUR_MIN_ADJUSTMENT, BLUR_MAX_ADJUSTMENT);

			if (changeNow)
			{
				lightningRenderer->SetBlurParams(					
					blurDirections,
					blurQuality,
					blurSize,
					blurAdjustment
				);
			}
		}
	}

	//Adjust animation parameters:
	{
		static float speed = DEFAULT_ANIM_SPEED;

		if (ImGui::CollapsingHeader("Set Animation Parameters"))
		{
			bool changeNow = false;

			changeNow = GuiSliderFloat(changeNow, "Anim Speed", &speed, ANIM_MIN_SPEED, ANIM_MAX_SPEED);

			if (changeNow)
			{
				lightningRenderer->SetAnimationParams(
					speed
				);
			}
		}
	}
}

////
// PRIVATE:
////

void PlayState::HandleInput()
{
	static MyInputUtil inputUtil(input);

	if (inputUtil.IsKeyPressedNow('R'))
	{
		pipelineMgr->RunProcess();
		if (debugCsv)
		{
			DebugWriteCsv(pipelineMgr->GetSegments());
		}
	}
	if (inputUtil.IsKeyPressedNow('C'))
	{
		pipelineMgr->Clear();
	}
	if (inputUtil.IsKeyPressedNow('N'))
	{
		animatingNow = !animatingNow;
	}

	if (inputUtil.IsKeyPressedNow('M'))
	{
		renderMode = renderMode == "animated" ? "static" : "animated";
		LightningRenderer* lightningRenderer = pipelineMgr->GetLightningRenderer();
		lightningRenderer->SetRenderMode(RENDER_MODE_OPTIONS.at(renderMode));
	}

	if (inputUtil.IsKeyPressedNow('I'))
	{
		LightningRenderer* lightningRenderer = pipelineMgr->GetLightningRenderer();
		lightningRenderer->InitAnimation();
	}

	if (inputUtil.IsKeyPressedNow('Z'))
	{
		zappy = !zappy;
	}

	// Abandoning these for now, they conflict with camera controls
	/*PipelineMgrSettings* settings = pipelineMgr->GetSettings();

	if (inputUtil.IsKeyPressedNow('T'))
	{
		pipelineMgr->SetDiameterThinnerActive(
			!(settings->IsDiameterThinnerActive())
		);
	}
	if (inputUtil.IsKeyPressedNow('W'))
	{
		pipelineMgr->SetWholeTransformerActive(
			!(settings->IsWholeTransformerActive())
		);
	}
	if (inputUtil.IsKeyPressedNow('B'))
	{
		pipelineMgr->SetBranchifierActive(
			!(settings->IsBranchifierActive())
		);
	}
	if (inputUtil.IsKeyPressedNow('E'))
	{
		pipelineMgr->SetElectifierActive(
			!(settings->IsElectrifierActive())
		);
	}*/

	inputUtil.EndFrame();
}