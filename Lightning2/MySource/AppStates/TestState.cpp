#include "TestState.h"

#include "DefaultParameters.h"
#include "Utils/MyGuiUtil.h"


////
// PUBLIC:
////

TestState::TestState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight, Input* _input, std::string _directoryPath)
	:
	BaseState::BaseState(_renderer, _hwnd, _screenWidth, _screenHeight, _input),
	pipelineMgr(NULL),
	directoryPath(_directoryPath)
{
	defaultSettings.geometryGenerator      = GeometryGeneratorTypes::STREAMER;
	defaultSettings.diameterThinnerActive  = false;
	defaultSettings.wholeTransformerActive = false;
	defaultSettings.branchifierActive      = false;
	defaultSettings.electrifierActive      = false;
	defaultSettings.rendererBuildingActive = false;

	pipelineMgr = new PipelineMgr(
		defaultSettings,
		renderer,
		hwnd,
		screenWidth,
		screenHeight
	);

	testThread = std::thread(&TestState::ThreadFunction, this);
}

TestState::~TestState()
{
	testThread.join();

	if (pipelineMgr)
	{
		delete pipelineMgr;
		pipelineMgr = NULL;
	}
}

void TestState::Init()
{
	InitPipelineMgr();

	iterationsPerTest = 10;
	iterationsDone    = 0;
	testRunning       = false;

	segmentMeasurer.SetObjectSize(sizeof(Segment));
}

void TestState::Update(float _dt)
{
	BaseState::Update(_dt);
}

void TestState::Render()
{

}

void TestState::Gui()
{
	ImGui::Begin("TEST STATE");
		
	if (testRunning)
	{
		UL lock(infoMutex);

		for (std::string txt : currentTestInfo)
		{
			ImGui::Text(txt.c_str());
		}

		if (ImGui::Button("STOP TESTS"))
		{
			testRunning = false;
		}
	}
	else
	{
		ImGui::InputInt("Iterations Per Test", &iterationsPerTest);

		if (ImGui::Button("Run Streamer Layers Test"))
		{
			currentTest = TestTypes::STREAMER_LAYERS;
			testCv.notify_all();
		}
		if (ImGui::Button("Run Electrifier Test"))
		{
			currentTest = TestTypes::ELECTRIFIER_BY_GEN_TYPE;
			testCv.notify_all();
		}

	}

	ImGui::End();
}

////
// PRIVATE:
////

void TestState::InitPipelineMgr()
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

	lightningRenderer->SetEnergyParams(
		ENERGY_SCALE_OPTIONS.at(DEFAULT_ENERGY_SCALE),
		DEFAULT_ENERGY_FOR_BLUR,
		DEFAULT_ENERGY_FOR_BRIGHTNESS
	);
}

void TestState::TestBegin()
{
	testRunning = true;
	InitPipelineMgr();
}

void TestState::TestEnd()
{
	testRunning = false;
}

void TestState::ThreadFunction()
{
	do {
		//WAITING FOR ACTIVATION:
		{
			UL lock(infoMutex);
			while (!testRunning)
			{
				testCv.wait(lock);
				TestBegin();
			}
		}

		//RUNNING TESTS:
		switch (currentTest)
		{
		case(TestTypes::STREAMER_LAYERS):
			TestStreamerLayers();
			break;
		case(TestTypes::ELECTRIFIER_BY_GEN_TYPE):
			TestElectrifierByGenType();
			break;
		}

		TestEnd();

	} while(true);
}

void TestState::InitOfstream(std::ofstream* stream)
{
	assert(stream->is_open());

#ifdef _DEBUG
	(*stream) << "DEBUG, \n";
#endif

	(*stream) << "ITS PER TEST: " << iterationsPerTest << '\n' << '\n';
}

void TestState::TestStreamerVsJitterfork(std::string fileName)
{

}

void TestState::TestStreamerLayers(std::string fileName)
{
	std::ofstream outFile(FilePath(fileName).c_str());
	InitOfstream(&outFile);

	outFile << "          , , TIME (MS),   ,       ,   ,    , , NUM SEGMENTS, \n";
	outFile << "NUM LAYERS, , MIN      , Q1, MEDIAN, Q3, MAX, , MIN, Q1, MEDIAN, Q3, MAX, \n";

	std::vector<float> timeSamples;
	std::vector<size_t>numSegmentsSamples;

	for (size_t numLayers = SG_MIN_MAX_NUM_LAYERS; numLayers < SG_MAX_MAX_NUM_LAYERS; numLayers++)
	{
		pipelineMgr->InitStreamerGenerator(
			DEFAULT_SG_START_PT,
			DEFAULT_SG_INITIAL_DIRECTION,
			DEFAULT_SG_VOLTAGE,
			DEFAULT_SG_INITIAL_PRESSURE,
			DEFAULT_SG_PRESSURE_GRADIENT,
			numLayers,
			ANGLE_FIX_OPTIONS.at(DEFAULT_SG_ANGLE_FIX),
			GAS_COMPOSITION_OPTIONS.at(DEFAULT_SG_GAS_COMPOSITION)
		);
		
		for (size_t i = 0; i < iterationsPerTest; i++)
		{
			//updating info for gui about where we're at, before testing:
			{
				UL lock(infoMutex);
				currentTestInfo.clear();
				{
					SS ss;
					ss << "LAYERS: " << numLayers << " of " << SG_MAX_MAX_NUM_LAYERS;
					currentTestInfo.push_back(ss.str());
				}
				{
					SS ss;
					ss << "ITERATION: " << i + 1 << " of " << iterationsPerTest;
					currentTestInfo.push_back(ss.str());
				}

				// tests may have been cancelled in gui:
				if (!testRunning)
				{
					return;
				}
			}

			timer.Start();
				pipelineMgr->RunProcess();
			timer.Stop();

			timeSamples.push_back(timer.GetDurationMs());
			numSegmentsSamples.push_back(pipelineMgr->GetSegments()->size());
		}
		
		SortVector(&timeSamples);
		SortVector(&numSegmentsSamples);

		outFile << numLayers << ", ";
		outFile << ", ";
		outFile << Min(timeSamples) << ", " << Q1(timeSamples) << ", " << Median(timeSamples) << ", " << Q3(timeSamples) << ", " << Max(timeSamples) << ", ";
		outFile << ", ";
		outFile << Min(numSegmentsSamples) << ", " << Q1(numSegmentsSamples) << ", " << Median(numSegmentsSamples) << ", " << Q3(numSegmentsSamples) << ", " << Max(numSegmentsSamples) << ", ";
		outFile << '\n';

		timeSamples.clear();
		numSegmentsSamples.clear();
	}
}

void TestState::TestElectrifierByGenType(std::string fileName)
{
	std::ofstream outFile(FilePath(fileName).c_str());
	InitOfstream(&outFile);
	
	int branchCullingAmt = 4;

	outFile << "           , , , GEN: JITTER + FORK ,   ,       ,   ,    , ,             ,   ,       ,   ,    , , , GEN: STREAMER      ,   ,       ,   ,    , ,             ,   ,       ,   ,    , , , GEN: STREAMER                             , \n";
	outFile << "           , , , BRANCH CULLING: OFF,   ,       ,   ,    , ,             ,   ,       ,   ,    , , , BRANCH CULLING: OFF,   ,       ,   ,    , ,             ,   ,       ,   ,    , , , BRANCH CULLING: " << branchCullingAmt << ", \n";
	outFile << "MAX SEG LEN, , , TIME(MS)           ,   ,       ,   ,    , , NUM SEGMENTS,   ,       ,   ,    , , , TIME(MS)           ,   ,       ,   ,    , , NUM SEGMENTS,   ,       ,   ,    , , , TIME(MS)                                  ,   ,       ,   ,    , , NUM SEGMENTS, \n";
	outFile << "           , , , MIN                , Q1, MEDIAN, Q3, MAX, , MIN         , Q1, MEDIAN, Q3, MAX, , , MIN                , Q1, MEDIAN, Q3, MAX, , MIN         , Q1, MEDIAN, Q3, MAX, , , MIN                                       , Q1, MEDIAN, Q3, MAX, , MIN         , Q1, MEDIAN, Q3, MAX, , , \n";
	
	std::vector<float> jitterforkTimeSamples;
	std::vector<size_t>jitterforkNumSegmentsSamples;

	std::vector<float> streanerWithoutCullTimeSamples;
	std::vector<size_t>streamerWithoutCullNumSegmentsSamples;

	std::vector<float> streanerWithCullTimeSamples;
	std::vector<size_t>streamerWithCullNumSegmentsSamples;

	pipelineMgr->SetElectifierActive(true);

	pipelineMgr->InitBranchifier(
		DEFAULT_B_INITIAL_DIAMETER,
		DEFAULT_B_DIAMETER_SCALEDOWN,
		DEFUALT_B_ANIMATION_TIME,
		branchCullingAmt
	);

	int numSamples = 20;
	float delta = (E_MAX_MAX_SEG_LENGTH - E_MIN_MAX_SEG_LENGTH) / float(numSamples);	

	int currentSample = 0;
	for (
		float maxSegLength = E_MAX_MAX_SEG_LENGTH;
		maxSegLength >= E_MIN_MAX_SEG_LENGTH &&    currentSample < numSamples;
		maxSegLength -= delta,                     currentSample++ 
	)
	{
		pipelineMgr->InitElectrifier(
			maxSegLength,
			DEFAULT_E_CHAOS_MEAN,
			DEFAULT_E_CHAOS_STDDEV
		);
		
		for (size_t i = 0; i < iterationsPerTest; i++)
		{

			//updating info for the gui:
			{
				UL lock(infoMutex);
				currentTestInfo.clear();
				{
					SS ss;
					ss << "MAX SEG LEN: " << maxSegLength << " (" << (currentSample + 1) << " of " << numSamples << ")";
					currentTestInfo.push_back(ss.str());
				}
				{
					SS ss;
					ss << "ITERATION: " << (i + 1) << " of " << iterationsPerTest;
					currentTestInfo.push_back(ss.str());
				}

				// Test may have been cancelled in GUI
				if (!testRunning)
				{
					return;
				}
			}
			
			pipelineMgr->SetBranchifierActive(false);

			// JITTER-FORK:
			pipelineMgr->SetGeometryGeneratorType(GeometryGeneratorTypes::JITTER_FORK);
			timer.Start();
				pipelineMgr->RunProcess();
			timer.Stop();

			jitterforkTimeSamples.push_back(timer.GetDurationMs());
			jitterforkNumSegmentsSamples.push_back(pipelineMgr->GetSegments()->size());

			// STREAMER (BRANCHIFIER OFF):
			pipelineMgr->SetGeometryGeneratorType(GeometryGeneratorTypes::STREAMER);
			timer.Start();
				pipelineMgr->RunProcess();
			timer.Stop();

			streanerWithoutCullTimeSamples.push_back(timer.GetDurationMs());
			streamerWithoutCullNumSegmentsSamples.push_back(pipelineMgr->GetSegments()->size());
			
			// STREAMER (BRANCHIFIER ON):
			pipelineMgr->SetBranchifierActive(true);

			timer.Start();
				pipelineMgr->RunProcess();
			timer.Stop();
			
			streanerWithCullTimeSamples.push_back(timer.GetDurationMs());
			streamerWithCullNumSegmentsSamples.push_back(timer.GetDurationMs());
		}

		SortVector(&jitterforkTimeSamples);
		SortVector(&jitterforkNumSegmentsSamples);

		SortVector(&streanerWithoutCullTimeSamples);
		SortVector(&streamerWithoutCullNumSegmentsSamples);

		SortVector(&streanerWithCullTimeSamples);
		SortVector(&streamerWithCullNumSegmentsSamples);
		
		outFile << maxSegLength << ", ";
		outFile << ", , ";
			outFile << Min(jitterforkTimeSamples) << ", " << Q1(jitterforkTimeSamples) << ", " << Median(jitterforkTimeSamples) << ", " << Q3(jitterforkTimeSamples) << ", " << Max(jitterforkTimeSamples) << ", ";
			outFile << ", ";
			outFile << Min(jitterforkNumSegmentsSamples) << ", " << Q1(jitterforkNumSegmentsSamples) << ", " << Median(jitterforkNumSegmentsSamples) << ", " << Q3(jitterforkNumSegmentsSamples) << ", " << Max(jitterforkNumSegmentsSamples) << ", ";
		outFile << ", , ";
			outFile << Min(streanerWithoutCullTimeSamples) << ", " << Q1(streanerWithoutCullTimeSamples) << ", " << Median(streanerWithoutCullTimeSamples) << ", " << Q3(streanerWithoutCullTimeSamples) << ", " << Max(streanerWithoutCullTimeSamples) << ", ";
			outFile << ", ";
			outFile << Min(streamerWithoutCullNumSegmentsSamples) << ", " << Q1(streamerWithoutCullNumSegmentsSamples) << ", " << Median(streamerWithoutCullNumSegmentsSamples) << ", " << Q3(streamerWithoutCullNumSegmentsSamples) << ", " << Max(streamerWithoutCullNumSegmentsSamples) << ", ";
		outFile << ", , ";
			outFile << Min(streanerWithCullTimeSamples) << ", " << Q1(streanerWithCullTimeSamples) << ", " << Median(streanerWithCullTimeSamples) << ", " << Q3(streanerWithCullTimeSamples) << ", " << Max(streanerWithCullTimeSamples) << ", ";
			outFile << ", ";
			outFile << Min(streamerWithCullNumSegmentsSamples) << ", " << Q1(streamerWithCullNumSegmentsSamples) << ", " << Median(streamerWithCullNumSegmentsSamples) << ", " << Q3(streamerWithCullNumSegmentsSamples) << ", " << Max(streamerWithCullNumSegmentsSamples) << ", ";
		outFile << '\n';
	}
}
