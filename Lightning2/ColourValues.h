#pragma once

#include "D3D.h"

const XMFLOAT4 LIGHTNING_WHITE  = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
const XMFLOAT4 LIGHTNING_YELLOW = XMFLOAT4(1.f, .749f, .122f, 1.f);
const XMFLOAT4 LIGHTNING_BLUE   = XMFLOAT4(.45f, .35f, 1.f, 1.f);

const XMFLOAT4 NIGHT_BACKGROUND_COLOUR = XMFLOAT4(.05f, 0.f, .1f, 1.f);
const XMFLOAT4 BLUE_BACKGROUND_COLOUR  = XMFLOAT4(0.39f, 0.58f, 0.92f, 1.0f);

const std::map<std::string, XMFLOAT4> COLOUR_OPTIONS{
	{"lightning white",  LIGHTNING_WHITE},
	{"lightning yellow", LIGHTNING_YELLOW},
	{"lightning blue",   LIGHTNING_BLUE},
	{"background night", NIGHT_BACKGROUND_COLOUR},
	{"background blue",  BLUE_BACKGROUND_COLOUR}
};