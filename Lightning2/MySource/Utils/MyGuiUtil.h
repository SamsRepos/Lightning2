#pragma once

#include "imGUI/imgui.h"

#include <string>
#include <map>

#include "Maths/MyFloat3.h"

const float GUI_MY_FLOAT_3_MIN = -100.f;
const float GUI_MY_FLOAT_3_MAX = 100.f;

// These functions all return true if...
// 1) there's been a change/interaction
// or..
// 2) changeFlag is true

bool GuiToggleBox(std::string msg, bool b);

//bool GuiToggleButton(std::string buttonMsg, bool currentlyOn);

bool GuiCheckBox(bool changeFlag, const char* msg, bool* b);

bool GuiSliderInt(bool changeFlag, const char* msg, int* i, int min, int max);

bool GuiSliderFloat(bool changeFlag, const char* msg, float* f, float min, float max);

template<class T>
bool GuiListBox(bool changeFlag, std::map<std::string, T> map, std::string header, std::string* current)
{
	bool changed = false;

	std::string msg = "Current: " + *current;
	ImGui::Text(msg.c_str());

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

bool GuiMyFloat3(bool changeFlag, const char* msg, MyFloat3* float3);