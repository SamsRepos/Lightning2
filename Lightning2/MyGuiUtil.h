#pragma once

#include "imGUI/imgui.h"

#include <string>
#include <map>

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