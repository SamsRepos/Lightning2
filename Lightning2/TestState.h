#pragma once

#include "BaseState.h"

#include <mutex>
#include <sstream>

#include "PipelineMgr.h"
#include "MyTimer.h"

typedef std::unique_lock<std::mutex> UL;
typedef std::stringstream SS;

enum TestTypes
{
	STREAMER_LAYERS
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
	PipelineMgrDefaultSettings defaultSettings;
	PipelineMgr* pipelineMgr;

	MyTimer timer;

	int iterationsPerTest;
	int iterationsDone;
	bool testRunning;
	TestTypes currentTest;

	std::thread testThread;
	std::mutex infoMutex;
	std::condition_variable testCv;

	void InitPipelineMgr();

	void TestBegin();
	void TestEnd();

	std::vector<std::string> currentTestInfo;

	void TestStreamerLayers(std::string filePath = "testResStreamerLevels.csv");
	
	void ThreadFunction();
};

