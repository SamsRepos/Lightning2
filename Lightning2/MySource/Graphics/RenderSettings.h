#pragma once

#include <map>

enum LightningRenderModes
{
	ANIMATED,
	STATIC
};

const std::map<std::string, LightningRenderModes> RENDER_MODE_OPTIONS = {
	{"animated", LightningRenderModes::ANIMATED},
	{"static", LightningRenderModes::STATIC}
};