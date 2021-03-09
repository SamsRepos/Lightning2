#include "BaseState.h"

BaseState::BaseState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight, Input* _input)
	:
	renderer(_renderer),
	hwnd(_hwnd),
	screenWidth(_screenWidth),
	screenHeight(_screenHeight),
	input(_input)
{
	dt = 0.f;
}

void BaseState::Update(float _dt)
{
	dt = _dt;
}