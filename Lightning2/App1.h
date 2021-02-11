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
	bool GuiToggleButton(std::string buttonMsg, bool currentlyOn);
	bool GuiCheckBox(bool changeFlag, const char* msg, bool* b);
	bool GuiSliderInt(bool changeFlag, const char* msg, int* i, int min, int max);
	bool GuiSliderFloat(bool changeFlag, const char* msg, float* f, float min, float max);
	
	template<class T>
	bool GuiListBox(bool changeFlag, std::map<std::string, T> map, std::string header, std::string* current)
	{
		bool changed = false;

		ImGui::ListBoxHeader(header.c_str());
		for (const auto& pair : map)
		{
			std::string name = pair.first;
			T type = pair.second;

			if (ImGui::Selectable(name.c_str()))
			{
				(*current) = name;
				changed = true;
				break;
			}
		}
		ImGui::ListBoxFooter();

		return changed || changeFlag;
	}

	//Shaders:
	LightShader* lightShader;
	
	//Lights:
	Light* light;

	//Meshes:
	PlaneMesh* planeMesh;
	
	XMMATRIX planeMatrix;

	PipelineMgr* pipelineMgr;

	std::vector<Segment*> testSegments;
	PathIdentifier testPi;
	WholeTransformer testWt;
};

#endif