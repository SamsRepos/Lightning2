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
	TestState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight, Input* _input, std::string _directoryPath = "../csv/");
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

	std::string FilePath(std::string fileName) { return (directoryPath + fileName); };

	void InitOfstream(std::ofstream* stream);

	void TestStreamerVsJitterfork(std::string fileName = "streamerVsJitter.csv");

	void TestStreamerLayers(std::string fileName = "streamerLayers.csv");

	void TestElectrifierByGenType(std::string fileName = "electrifier.csv");

	template<class T>
	inline constexpr void SortVector(std::vector<T>* samples)
	{
		std::sort(samples->begin(), samples->end());
	}

	template<class T>
	inline constexpr const T& Min(std::vector<T>& samples)
	{
		return samples.front();
	}
	
	template<class T>
	inline constexpr const T& Q1(std::vector<T>& samples)
	{
		return samples[samples.size() / 4];
	}

	template<class T>
	inline constexpr const T& Median(std::vector<T>& samples)
	{
		return samples[samples.size() / 2];
	}

	template<class T>
	inline constexpr const T& Q3(std::vector<T>& samples)
	{
		return samples[(samples.size() / 4) * 3];
	}

	template<class T>
	inline constexpr const T& Max(std::vector<T>& samples)
	{
		return samples.back();
	}

	PipelineMgrDefaultSettings defaultSettings;
	PipelineMgr* pipelineMgr;

	MyTimer timer;
	MyMemoryMeasurer segmentMeasurer;

	int iterationsPerTest;
	int iterationsDone;
	bool testRunning;
	TestTypes currentTest;

	std::thread testThread;
	std::mutex infoMutex;
	std::condition_variable testCv;

	std::vector<std::string> currentTestInfo;

	std::string directoryPath;
};

