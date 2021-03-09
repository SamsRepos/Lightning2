#pragma once

#include "BaseState.h"

#include <mutex>
#include <sstream>

#include "PipelineMgr.h"
#include "PerformanceTools/MyTimer.h"
#include "PerformanceTools/MyMemoryMeasurer.h"

typedef std::unique_lock<std::mutex> UL;
typedef std::stringstream SS;

enum TestTypes
{
	STREAMER_LAYERS,
	ELECTRIFIER_BY_GEN_TYPE
};

class TestState : public BaseState
{
public:
	TestState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight);
	~TestState();

	void Init();
	void Update(float _dt);
	void Render();
	void Gui();

private:
	void InitPipelineMgr();

	void TestBegin();
	void TestEnd();

	void ThreadFunction();

	void InitOfsream(std::ofstream* stream);

	void TestStreamerLayers(const char* rawFilePath = "../csv/testResStreamerLayers.csv", const char* meansFilePath = "../csv/testMeansStreamerLayers.csv");
	void TestElectrifierByGenType(const char* rawFilePath = "../csv/testResElecByGenType.csv", const char* meansFilePath = "../csv/testMeansElecByGenType.csv");
	
	PipelineMgrDefaultSettings defaultSettings;
	PipelineMgr* pipelineMgr;

	MyTimer timer;
	MyMemoryMeasurer segMeasuerer;
	MyMemoryMeasurer cylMeasurer;

	int iterationsPerTest;
	int iterationsDone;
	bool testRunning;
	TestTypes currentTest;

	std::thread testThread;
	std::mutex infoMutex;
	std::condition_variable testCv;

	std::vector<std::string> currentTestInfo;	
};

