#pragma once

#include "Input.h"

class MyInputUtil
{
public:
	MyInputUtil(Input* _currentInput, Input* _previousInput);

	inline bool IsKeyPressedNow(int key) {
		return currentInput->isKeyDown(key) && !previousInput->isKeyDown(key);
	};
	
	inline void EndFrame() { *previousInput = *currentInput; };

private:
	Input* currentInput;
	Input* previousInput;
};

