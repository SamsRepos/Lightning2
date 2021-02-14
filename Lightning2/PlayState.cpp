#include "PlayState.h"

#include "MyClamp.h"
#include "DefaultParameters.h"
#include "DebugCsvWriter.h"
#include "MyGuiUtil.h"

PlayState::PlayState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight, FPCamera* _camera, TextureManager* _textureMgr)
	:
	BaseState::BaseState(_renderer, _hwnd, _screenWidth, _screenHeight),
	pipelineMgr(NULL),
	lightShader(NULL),
	light(NULL),
	planeMesh(NULL),
	camera(_camera),
	textureMgr(_textureMgr)
{
	//PIPELINE MANAGER:
	defaultSettings.geometryGenerator         = GeometryGeneratorTypes::STREAMER;
	defaultSettings.diameterThinnerActive     = false;
	defaultSettings.wholeTransformerActive    = false;
	defaultSettings.diameterTransformerActive = false;
	defaultSettings.electrifierActive         = false;
	defaultSettings.blurRenderingActive       = true;
	defaultSettings.lineRenderingActive       = true;
	defaultSettings.cylinderRenderingActive   = true;

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
		DEFAULT_SG_MAX_NUM_LAYERS
	);

	pipelineMgr->InitDiameterThinner(
		DEFAULT_DTHIN_SCALEDOWN
	);

	pipelineMgr->InitWholeTransformer(
		DEFAULT_WT_START_POINT,
		DEFAULT_WT_END_POINT
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

	pipelineMgr->InitLineRenderer(
		COLOUR_OPTIONS.at(DEFAULT_LINE_COLOUR)
	);

	pipelineMgr->InitCylinderRenderer(
		COLOUR_OPTIONS.at(DEFAULT_BLUR_COLOUR),
		COLOUR_OPTIONS.at(DEFAULT_BLUR_BACKGROUND_COLOUR),
		COLOUR_OPTIONS.at(DEFAULT_CYLINDER_COLOUR),
		DEFAULT_BLUR_EXTENT,
		DEFAULT_BLUR_RANGE
	);
}

void PlayState::Update(float _dt)
{
	BaseState::Update(_dt);
}

void PlayState::Render()
{
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	if (!(pipelineMgr->GetSettings()->IsBlurRenderingActive()))
	{
		planeMesh->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), planeMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"metal"), light);
		lightShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());
	}

	pipelineMgr->RenderOutput(
		renderer,
		camera,
		worldMatrix,
		viewMatrix,
		projectionMatrix
	);
}

void PlayState::Gui()
{
	ImGui::Text("PLAY STATE");

	//TODO consider this:
#if 0
	ImGui::SliderInt(
		"Debug lines to render",
		&linesToRender,
		0,
		lineMesh->GetLineCount()
	);
#endif

	static bool debugCsv = true;
	ImGui::Checkbox("Write debug CSV", &debugCsv);

	if (ImGui::Button("Run whole process"))
	{
		pipelineMgr->RunProcess();
		if (debugCsv)
		{
			DebugWriteCsv(pipelineMgr->GetSegments());
		}
	}

	static bool zappy = false;
	ImGui::Checkbox("ZAPPY", &zappy);
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

	// Pipeline stages:
	{
		PipelineMgrSettings* settings = pipelineMgr->GetSettings();

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
		if (settings->IsDiameterThinnerActive())
		{
			ImGui::Text("- Diameter Thinner");
		}
		if (settings->IsPathIdentifierActive())
		{
			ImGui::Text("- Path Identifier (auto)");
		}
		if (settings->IsWholeTransformerActive())
		{
			ImGui::Text("- Whole Transformer");
		}
		if (settings->IsDiameterTransformerActive())
		{
			ImGui::Text("- Diameter Transformer");
		}
		if (settings->IsElectrifierActive())
		{
			ImGui::Text("- Electrifier");
		}
		ImGui::Unindent();

		//Renderers:
		ImGui::Text("Active Rendering Stages: ");
		ImGui::Indent();
		if (settings->IsBlurRenderingActive())
		{
			ImGui::Text("- Blur Rendering");
		}
		if (settings->IsLineRenderingActive())
		{
			ImGui::Text("- Line Rendering");
		}
		if (settings->IsCylinderRenderingActive())
		{
			ImGui::Text("- Cylinder Rendering");
		}
		ImGui::Unindent();

		//Geometry Generator:
		static std::map<std::string, GeometryGeneratorTypes> genTypesMap = {
				{"Jitter + Fork", GeometryGeneratorTypes::JITTER_FORK},
				{"Streamer", GeometryGeneratorTypes::STREAMER}
		};

		if (ImGui::CollapsingHeader("Select Geometry Generator"))
		{
			bool changeNow = false;
			changeNow = GuiListBox(&changeNow, genTypesMap, "Geometry Generator", &currentGenerator);

			if (changeNow)
			{
				settings->SetGeometryGeneratorType(genTypesMap[currentGenerator]);
			}
		}

		// Toggle pipeline stages
		if (ImGui::CollapsingHeader("Toggle Transformer Stages On/Off"))
		{
			if (GuiToggleButton("Toggle Diameter Thinner", settings->IsDiameterThinnerActive()))
			{
				pipelineMgr->SetDiameterThinnerActive(
					!(settings->IsDiameterThinnerActive())
				);
			}

			if (GuiToggleButton("Toggle Whole Transformer", settings->IsWholeTransformerActive()))
			{
				pipelineMgr->SetWholeTransformerActive(
					!(settings->IsWholeTransformerActive())
				);
			}

			if (GuiToggleButton("Toggle Diameter Transformer", settings->IsDiameterTransformerActive()))
			{
				pipelineMgr->SetDiameterTransformerActive(
					!(settings->IsDiameterTransformerActive())
				);
			}

			if (GuiToggleButton("Toggle Electrifier", settings->IsElectrifierActive()))
			{
				pipelineMgr->SetElectifierActive(
					!(settings->IsElectrifierActive())
				);
			}
		}

		//Toggle renderers:
		if (ImGui::CollapsingHeader("Toggle Rendering Stages On/Off"))
		{
			if (GuiToggleButton("Toggle Blur Rendering", settings->IsBlurRenderingActive()))
			{
				pipelineMgr->SetBlurRenderingActive(
					!(settings->IsBlurRenderingActive())
				);
			}

			if (GuiToggleButton("Toggle Line Rendering", settings->IsLineRenderingActive()))
			{
				pipelineMgr->SetLineRenderingActive(
					!(settings->IsLineRenderingActive())
				);
			}

			if (GuiToggleButton("Toggle Cylinder Rendering", settings->IsCylinderRenderingActive()))
			{
				pipelineMgr->SetCylinderRenderingActive(
					!(settings->IsCylinderRenderingActive())
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
		static MyFloat3 startPt = DEFAULT_SG_START_PT;
		static MyFloat3 initialDirection = DEFAULT_SG_INITIAL_DIRECTION;
		static float voltage = DEFAULT_SG_VOLTAGE;
		static float initialPressure = DEFAULT_SG_INITIAL_PRESSURE;
		static float pressureGradient = DEFAULT_SG_PRESSURE_GRADIENT;
		static int   maxNumLayers = DEFAULT_SG_MAX_NUM_LAYERS;

		if (ImGui::CollapsingHeader("Set Streamer Parameters"))
		{

			bool changeNow = false;
			//TODO MyFloat3 gui
			changeNow = GuiSliderFloat(&changeNow, "SG initial voltage", &voltage, SG_MIN_VOLTAGE, SG_MAX_VOLTAGE);
			changeNow = GuiSliderFloat(&changeNow, "SG initial pressure", &initialPressure, SG_MIN_INITIAL_PRESSURE, SG_MAX_INITIAL_PRESSURE);
			changeNow = GuiSliderFloat(&changeNow, "SG pressure gradient", &pressureGradient, SG_MIN_PRESSURE_GRADIENT, SG_MAX_PRESSURE_GRADIENT);
			changeNow = GuiSliderInt(&changeNow, "SG max num layers", &maxNumLayers, SG_MIN_MAX_NUM_LAYERS, SG_MAX_MAX_NUM_LAYERS);

			if (changeNow)
			{
				pipelineMgr->InitStreamerGenerator(
					startPt,
					initialDirection,
					voltage,
					initialPressure,
					pressureGradient,
					maxNumLayers
				);
			}
		}
	}

	//Adjust Diameter Thinner Parameters:
	{
		static float scaledown = DEFAULT_DTHIN_SCALEDOWN;

		if (ImGui::CollapsingHeader("Set Diameter Thinner Parameters"))
		{
			bool changeNow = false;
			changeNow = GuiSliderFloat(changeNow, "DTHIN scaledown", &scaledown, DTHIN_MIN_SCALEDOWN, DTHIN_MAX_SCALEDOWN);

			if (changeNow)
			{
				pipelineMgr->InitDiameterThinner(
					scaledown
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
			//

			if (changeNow)
			{
				pipelineMgr->InitWholeTransformer(
					startPoint,
					endPoint
				);
			}
		}
	}

	//Adjust Diameter Transformer Parameters
	{
		static float initialDiameter = DEFAULT_DT_INITIAL_DIAMETER;
		static float diameterScaledown = DEFAULT_DT_DIAMETER_SCALEDOWN;
		static int maxNumBranchLevels = DEFAULT_DT_MAX_NUM_BRANCH_LEVELS;

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
		static float chaosMean = DEFAULT_E_CHAOS_MEAN;
		static float chaosStdDev = DEFAULT_E_CHAOS_STDDEV;

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

	//Adjust Line Renderer Parameters:
	{
		static std::string lineColour = DEFAULT_LINE_COLOUR;

		if (ImGui::CollapsingHeader("Set Line Renderer Parameters"))
		{
			bool changeNow = false;
			changeNow = GuiListBox(&changeNow, COLOUR_OPTIONS, "Line Colour", &lineColour);

			if (changeNow)
			{
				pipelineMgr->InitLineRenderer(
					COLOUR_OPTIONS.at(lineColour)
				);
			}
		}
	}


	//Adjust Cylinder Renderer Parameters:
	{
		static std::string blurColour = DEFAULT_BLUR_COLOUR;
		static std::string blurBackgroundColour = DEFAULT_BLUR_BACKGROUND_COLOUR;
		static std::string cylinderColour = DEFAULT_CYLINDER_COLOUR;
		static float blurExtent = DEFAULT_BLUR_EXTENT;
		static float blurRange = DEFAULT_BLUR_RANGE;

		if (ImGui::CollapsingHeader("Set Cylinder Renderer Parameters"))
		{

			bool changeNow = false;
			changeNow = GuiListBox(changeNow, COLOUR_OPTIONS, "Blur Colour", &blurColour);
			changeNow = GuiListBox(changeNow, COLOUR_OPTIONS, "Blur Background Colour", &blurBackgroundColour);
			changeNow = GuiListBox(changeNow, COLOUR_OPTIONS, "Cylinder Colour", &cylinderColour);
			changeNow = GuiSliderFloat(&changeNow, "Blur extent", &blurExtent, BLUR_MIN_EXTENT, BLUR_MAX_EXTENT);
			changeNow = GuiSliderFloat(&changeNow, "Blur range", &blurRange, BLUR_MIN_RANGE, BLUR_MAX_RANGE);

			if (changeNow)
			{
				pipelineMgr->InitCylinderRenderer(
					COLOUR_OPTIONS.at(blurColour),
					COLOUR_OPTIONS.at(blurBackgroundColour),
					COLOUR_OPTIONS.at(cylinderColour),
					blurExtent,
					blurRange
				);
			}
		}
	}
}