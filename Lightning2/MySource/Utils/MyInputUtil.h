#pragma once

#include "Input.h"

class MyInputUtil
{
public:
	MyInputUtil(Input* input);

	inline bool IsKeyPressedNow(int key) {
		return currentInput->isKeyDown(key) && !(previousInput.isKeyDown(key));
	};
	
	inline void EndFrame() { previousInput = *currentInput; };

private:
	Input* currentInput;
	Input previousInput;
};
