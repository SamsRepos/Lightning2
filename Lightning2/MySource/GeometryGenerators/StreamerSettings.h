#pragma once

#include <map>

enum AngleFixMethods
{
	LOWEST_Y,
	FARTHEST_FROM_PARENT,
	NONE
};

const std::map<std::string, AngleFixMethods> ANGLE_FIX_OPTIONS = {
	{"none", AngleFixMethods::NONE},
	{"lowest_y", AngleFixMethods::LOWEST_Y},
	{"parent_dist", AngleFixMethods::FARTHEST_FROM_PARENT}
};

enum GasCompositions
{
	AIR,
	N2
};

const std::map<std::string, GasCompositions> GAS_COMPOSITION_OPTIONS = {
	{"air", GasCompositions::AIR},
	{"n2", GasCompositions::N2}
};