#pragma once

#include <chrono>

#include <iostream>
#include <thread>
#include <algorithm>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

typedef std::chrono::steady_clock the_clock;

class MyTimer {
	the_clock::time_point startTime;
	float duration;
public:

	inline void start() {
		startTime = the_clock::now();
	}

	inline void stop() {
		duration = duration_cast<milliseconds>(the_clock::now() - startTime).count();
	}

	inline float getDuration() {
		return duration;
	}
};
