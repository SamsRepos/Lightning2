#pragma once

#include "D3D.h"
#include "FPCamera.h"

class BaseState
{
public:
	BaseState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight, Input* _input);

	virtual void Init() = 0;
	virtual void Update(float _dt);
	virtual void Render() = 0;
	virtual void Gui() = 0;

protected:
	D3D* renderer;
	HWND hwnd;
	int screenWidth;
	int screenHeight;
	Input* input;

	float dt;
};

