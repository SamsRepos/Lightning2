// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "BaseApplication.h"

#include "Lightshader.h"
#include "LineShader.h"

#include "Light.h"

#include "PlaneMesh.h"

#include "PipelineMgr.h"

const XMFLOAT4 BLUE_BACKGROUND_COLOUR = XMFLOAT4(0.39f, 0.58f, 0.92f, 1.0f);
const XMFLOAT4 NIGHT_BACKGROUND_COLOUR = XMFLOAT4(.05f, 0.f, .1f, 1.f);

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
	bool GuiSliderInt(bool* changeFlag, const char* msg, int* i, int min, int max);
	bool GuiSliderFloat(bool* changeFlag, const char* msg, float* f, float min, float max);
	
	//Shaders:
	LightShader* lightShader;
		
	//Lights:
	Light* light;

	//Meshes:
	PlaneMesh* planeMesh;
	
	XMMATRIX planeMatrix;

	PipelineMgr* pipelineMgr;
};

#endif