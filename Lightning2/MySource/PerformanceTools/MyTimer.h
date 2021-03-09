#pragma once

#include <chrono>

#include <iostream>
#include <thread>
#include <algorithm>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

typedef std::chrono::steady_clock the_clock;

class MyTimer {	
public:
	inline void Start() { startTime = the_clock::now(); };
	inline void Stop() { durationMs = duration_cast<milliseconds>(the_clock::now() - startTime).count(); };
	inline float GetDurationMs() { return durationMs; };

private:
	the_clock::time_point startTime;
	float durationMs;
};
