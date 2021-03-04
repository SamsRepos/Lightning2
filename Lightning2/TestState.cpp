#include "TestState.h"

#include "DefaultParameters.h"
#include "MyGuiUtil.h"
#include "CylinderObject.h"


////
// PUBLIC:
////

TestState::TestState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight)
	:
	BaseState::BaseState(_renderer, _hwnd, _screenWidth, _screenHeight),
	pipelineMgr(NULL)
{
	defaultSettings.geometryGenerator         = GeometryGeneratorTypes::STREAMER;
	defaultSettings.diameterThinnerActive     = false;
	defaultSettings.wholeTransformerActive    = false;
	defaultSettings.diameterTransformerActive = false;
	defaultSettings.electrifierActive         = false;
	defaultSettings.renderingActive           = false;

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

	segMeasuerer.SetObjectSize(sizeof(Segment));
	cylMeasurer.SetObjectSize(sizeof(CylinderObject));
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
	ImGui::Text("TEST STATE");
	
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

	LightningRenderer* lightningRenderer = pipelineMgr->GetLightningRenderer();

	lightningRenderer->SetColours(
		COLOUR_OPTIONS.at(DEFAULT_BACKGROUND_COLOUR),
		COLOUR_OPTIONS.at(DEFAULT_BLUR_COLOUR),
		COLOUR_OPTIONS.at(DEFAULT_LINE_COLOUR),
		COLOUR_OPTIONS.at(DEFAULT_CYLINDER_COLOUR)
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

void TestState::InitOfsream(std::ofstream* stream)
{
	assert(stream->is_open());

#ifdef _DEBUG
	(*stream) << "DEBUG, \n";
#endif

	(*stream) << "ITS PER TEST: " << iterationsPerTest << '\n' << '\n';
}

void TestState::TestStreamerLayers(const char* rawFilePath, const char* meansFilePath)
{
	std::ofstream rawOutFile(rawFilePath);
	InitOfsream(&rawOutFile);

	std::ofstream meansOutFile(meansFilePath);
	InitOfsream(&meansOutFile);	
	
	meansOutFile << "NUM LAYERS, TIME (MS), NUM SEGMENTS, \n";

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

		rawOutFile << "LAYERS:, " << numLayers << '\n';

		rawOutFile << "TIME (MS), NUM SEGMENTS, \n";

		float timeRunningTotal      = 0.f;
		int numSegmentsRunningTotal = 0;
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

			//Raw data output:
			rawOutFile << timer.GetDurationMs() << ", ";
			rawOutFile << pipelineMgr->GetSegments()->size() << ", ";
			rawOutFile << '\n';

			//For means output:
			timeRunningTotal += timer.GetDurationMs();
			numSegmentsRunningTotal += pipelineMgr->GetSegments()->size();
		}
		rawOutFile << '\n';

		float meanTime        = timeRunningTotal / float(iterationsPerTest);
		float meanNumSegments = numSegmentsRunningTotal / float(iterationsPerTest);
		meansOutFile << numLayers << ", ";
		meansOutFile << meanTime << ", ";
		meansOutFile << meanNumSegments << ", ";
		meansOutFile << '\n';
	}
}

void TestState::TestElectrifierByGenType(const char* rawFilePath, const char* meansFilePath)
{
	std::ofstream rawOutFile(rawFilePath);
	InitOfsream(&rawOutFile);

	std::ofstream meansOutFile(meansFilePath);
	InitOfsream(&meansOutFile);

	// Gen type row:
	meansOutFile << " , , ";
	meansOutFile << "GEN: JITTER + FORK, , , ";
	meansOutFile << "GEN: STREAMER, , , ";
	meansOutFile << "GEN: STREAMER, , , ";
	meansOutFile << '\n';

	// Branch culling row:
	meansOutFile << " , , ";
	meansOutFile << "BRANCH CULLING: OFF, , , ";
	meansOutFile << "BRANCH CULLING: OFF, , , ";
	meansOutFile << "BRANCH CULLING: 3, , , ";
	meansOutFile << "\n";

	// Output row:
	meansOutFile << "MAX SEG LEN, , ";
	meansOutFile << "TIME(MS), NUM SEGMENTS, Seg KB, , ";
	meansOutFile << "TIME(MS), NUM SEGMENTS, Seg KB, , ";
	meansOutFile << "TIME(MS), NUM SEGMENTS, Seg KB, , ";
	meansOutFile << '\n';
	
	pipelineMgr->SetElectifierActive(true);

	pipelineMgr->InitDiameterTransformer(
		DEFAULT_DT_INITIAL_DIAMETER,
		DEFAULT_DT_DIAMETER_SCALEDOWN,
		4
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

		rawOutFile << "MAX SEG LEN: " << maxSegLength << '\n';
		rawOutFile << "GEN: JITTER + FORK, , , ";
		rawOutFile << "GEN: STREAMER, , , ";
		rawOutFile << "GEN: STREAMER, , , ";
		rawOutFile << '\n';

		// Branch culling row:
		rawOutFile << "BRANCH CULLING: OFF, , , ";
		rawOutFile << "BRANCH CULLING: OFF, , , ";
		rawOutFile << "BRANCH CULLING: 3, , , ";
		rawOutFile << "\n";

		// Output row:
		rawOutFile << "TIME(MS), NUM SEGMENTS, Seg KB, , ";
		rawOutFile << "TIME(MS), NUM SEGMENTS, Seg KB, , ";
		rawOutFile << "TIME(MS), NUM SEGMENTS, Seg KB, , ";
		rawOutFile << '\n';

		float jitterForkTimeRunningTotal       = 0.f;
		int jitterForkSegmentsRunningTotal     = 0;
		size_t jitterForkKbRunningTotal        = 0;
		float streamerNoCullTimeRunningTotal   = 0.f;
		int streamerNoCullSegmentsRunningTotal = 0;
		size_t streamerNoCullKbRunningTotal    = 0;
		float streamerCullTimeRunningTotal     = 0.f;
		int streamerCullSegmentsRunningTotal   = 0;
		size_t streamerCullKbRunningTotal      = 0;

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
			}
			

			pipelineMgr->SetDiameterTransformerActive(false);

			// JITTER-FORK:
			pipelineMgr->SetGeometryGeneratorType(GeometryGeneratorTypes::JITTER_FORK);
			timer.Start();
				pipelineMgr->RunProcess();
			timer.Stop();
			segMeasuerer.CalculateMemoryUsed(pipelineMgr->GetSegments()->size());
			//Raw data output:
			rawOutFile << timer.GetDurationMs() << ", ";
			rawOutFile << pipelineMgr->GetSegments()->size() << ", ";
			rawOutFile << segMeasuerer.GetKbUsed() << ", ";
			rawOutFile << ", ";
			//For means output:
			jitterForkTimeRunningTotal     += timer.GetDurationMs();
			jitterForkSegmentsRunningTotal += pipelineMgr->GetSegments()->size();
			jitterForkKbRunningTotal       += segMeasuerer.GetKbUsed();

			// STREAMER:
			pipelineMgr->SetGeometryGeneratorType(GeometryGeneratorTypes::STREAMER);
			timer.Start();
				pipelineMgr->RunProcess();
			timer.Stop();
			segMeasuerer.CalculateMemoryUsed(pipelineMgr->GetSegments()->size());
			//Raw data output:
			rawOutFile << timer.GetDurationMs() << ", ";
			rawOutFile << pipelineMgr->GetSegments()->size() << ", ";
			rawOutFile << segMeasuerer.GetKbUsed() << ", ";
			rawOutFile << ", ";
			//For means output:
			streamerNoCullTimeRunningTotal     += timer.GetDurationMs();
			streamerNoCullSegmentsRunningTotal += pipelineMgr->GetSegments()->size();
			streamerNoCullKbRunningTotal       += segMeasuerer.GetKbUsed();

			pipelineMgr->SetDiameterTransformerActive(true);

			timer.Start();
				pipelineMgr->RunProcess();
			timer.Stop();
			segMeasuerer.CalculateMemoryUsed(pipelineMgr->GetSegments()->size());
			//Raw data output:
			rawOutFile << timer.GetDurationMs() << ", ";
			rawOutFile << pipelineMgr->GetSegments()->size() << ", ";
			rawOutFile << segMeasuerer.GetKbUsed() << ", ";
			rawOutFile << '\n';
			//For means output:
			streamerCullTimeRunningTotal     += timer.GetDurationMs();
			streamerCullSegmentsRunningTotal += pipelineMgr->GetSegments()->size();
			streamerCullKbRunningTotal       += segMeasuerer.GetKbUsed();
		}
		rawOutFile << '\n';
		
		meansOutFile << maxSegLength << ", ";
		meansOutFile << " , ";
		meansOutFile << jitterForkTimeRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << jitterForkSegmentsRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << jitterForkKbRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << " , ";
		meansOutFile << streamerNoCullTimeRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << streamerNoCullSegmentsRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << streamerNoCullKbRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << " , ";
		meansOutFile << streamerCullTimeRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << streamerCullSegmentsRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << streamerCullKbRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << '\n';
	}
}
