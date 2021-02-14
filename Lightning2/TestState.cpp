#include "TestState.h"


TestState::TestState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight)
	:
	BaseState::BaseState(_renderer, _hwnd, _screenWidth, _screenHeight)
{
}

void TestState::Init()
{

}

void TestState::Update(float _dt)
{
	BaseState::Update(_dt);
}

void TestState::Render()
{

}

void TestState::Gui()
{

}