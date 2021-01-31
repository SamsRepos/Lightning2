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
	bool GuiCheckBox(bool* changeFlag, const char* msg, bool* b);
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