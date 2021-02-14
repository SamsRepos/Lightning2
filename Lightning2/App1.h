// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "BaseApplication.h"

#include "BaseState.h"
#include "PlayState.h"
#include "TestState.h"

class App1 : public BaseApplication
{
public:
	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);
	bool frame();
	bool render();
	
private:
	void Gui();

	BaseState* currentState;

	PlayState* playState;
	TestState* testState;
};

#endif