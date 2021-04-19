#include "MyGuiUtil.h"

bool GuiToggleBox(std::string msg, bool b)
{
	return ImGui::Checkbox(msg.c_str(), &b);	
}

bool GuiToggleButton(std::string buttonMsg, bool currentlyOn)
{
	buttonMsg.append(currentlyOn ? " to Off" : " to On");
	return ImGui::Button(buttonMsg.c_str());
}

bool GuiCheckBox(bool changeFlag, const char* msg, bool* b)
{
	return ImGui::Checkbox(msg, b) || changeFlag;
}

bool GuiSliderInt(bool changeFlag, const char* msg, int* i, int min, int max)
{
	return ImGui::SliderInt(msg, i, min, max) || changeFlag;
}

bool GuiSliderFloat(bool changeFlag, const char* msg, float* f, float min, float max)
{
	return ImGui::SliderFloat(msg, f, min, max) || changeFlag;
}

bool GuiMyFloat3(bool changeFlag, const char* msg, MyFloat3* float3, float min, float max)
{
	float vals[3] = {
		float3->x,
		float3->y,
		float3->z
	};

	bool changeNow = ImGui::SliderFloat3(msg, vals, min, max);

	if (changeNow)
	{
		float3->x = vals[0];
		float3->y = vals[1];
		float3->z = vals[2];
	}

	return changeNow || changeFlag;
}