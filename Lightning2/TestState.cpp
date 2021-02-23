#include "TestState.h"

#include "DefaultParameters.h"
#include "MyGuiUtil.h"


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
	defaultSettings.blurRenderingActive       = false;
	defaultSettings.lineRenderingActive       = false;
	defaultSettings.cylinderRenderingActive   = false;

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

	iterationsPerTest = 100;
	iterationsDone    = 0;
	testRunning = false;
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
			numLayers
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

	meansOutFile << " , , GEN: JITTER + FORK, , , GEN: STREAMER, \n";
	meansOutFile << " , MAX SEG LEN, TIME (MS), NUM SEGMENTS, , TIME (MS), NUM SEGMENTS, \n";
	
	pipelineMgr->SetElectifierActive(true);

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
		rawOutFile << "GEN: JITTER + FORK, , , GEN: STREAMER, \n";
		rawOutFile << "TIME (MS), NUM SEGMENTS, , TIME (MS), NUM SEGMENTS, \n";

		float jitterForkTimeRunningTotal   = 0.f;
		int jitterForkSegmentsRunningTotal = 0;
		float streamerTimeRunningTotal     = 0.f;
		int streamerSegmentsRunningTotal   = 0;
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
			

			// JITTER-FORK:
			pipelineMgr->SetGeometryGeneratorType(GeometryGeneratorTypes::JITTER_FORK);
			timer.Start();
				pipelineMgr->RunProcess();
			timer.Stop();
			//Raw data output:
			rawOutFile << timer.GetDurationMs() << ", ";
			rawOutFile << pipelineMgr->GetSegments()->size() << ", ";
			rawOutFile << ", ";
			//For means output:
			jitterForkTimeRunningTotal     += timer.GetDurationMs();
			jitterForkSegmentsRunningTotal += pipelineMgr->GetSegments()->size();

			// STREAMER:
			pipelineMgr->SetGeometryGeneratorType(GeometryGeneratorTypes::STREAMER);
			timer.Start();
				pipelineMgr->RunProcess();
			timer.Stop();
			//Raw data output:
			rawOutFile << timer.GetDurationMs() << ", ";
			rawOutFile << pipelineMgr->GetSegments()->size() << ", ";
			rawOutFile << '\n';
			//For means output:
			streamerTimeRunningTotal     += timer.GetDurationMs();
			streamerSegmentsRunningTotal += pipelineMgr->GetSegments()->size();
		}
		rawOutFile << '\n';
		
		meansOutFile << ", ";
		meansOutFile << maxSegLength << ", ";
		meansOutFile << jitterForkTimeRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << jitterForkSegmentsRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << ", " << ", ";
		meansOutFile << streamerTimeRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << streamerSegmentsRunningTotal / float(iterationsPerTest) << ", ";
		meansOutFile << '\n';
	}
}
