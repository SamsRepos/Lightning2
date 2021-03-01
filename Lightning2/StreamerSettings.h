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
	{"lowest Y", AngleFixMethods::LOWEST_Y},
	{"farthest from parent", AngleFixMethods::FARTHEST_FROM_PARENT}
};