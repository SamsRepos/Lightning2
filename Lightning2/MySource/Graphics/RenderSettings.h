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

enum EnergyScales {
	LINEAR,
	LN,
	LOG_10
};

const std::map<std::string, EnergyScales>ENERGY_SCALE_OPTIONS = {
	{"linear", EnergyScales::LINEAR},
	{"ln", EnergyScales::LN},
	{"log_10", EnergyScales::LOG_10}
};