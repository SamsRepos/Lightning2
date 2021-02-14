#pragma once

#include "BaseState.h"

class TestState : public BaseState
{
public:
	TestState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight);

	void Init();
	void Update(float _dt);
	void Render();
	void Gui();

private:
};

