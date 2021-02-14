#include "MyGuiUtil.h"

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
