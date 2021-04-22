#pragma once

#include <chrono>

#include <iostream>
#include <thread>
#include <algorithm>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;

typedef std::chrono::steady_clock the_clock;

class MyTimer {	
public:
	inline void Start() { startTime = the_clock::now(); };
	inline void Stop() { 
		durationMilliseconds = duration_cast<milliseconds>(the_clock::now() - startTime).count();
		durationMicroseconds = duration_cast<microseconds>(the_clock::now() - startTime).count();
	};
	inline float GetDurationMilliseconds() { return durationMilliseconds; };
	inline float GetDurationMicroseconds() { return durationMicroseconds; };

private:
	the_clock::time_point startTime;
	float durationMilliseconds;
	float durationMicroseconds;
};
