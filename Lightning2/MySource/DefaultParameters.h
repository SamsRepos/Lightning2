#pragma once

#include <map>

#include "D3D.h"

#include "Maths/MyFloat3.h"
#include "GeometryGenerators/StreamerSettings.h"
#include "ColourValues.h"

////
// GENERAL:
////

const size_t RECURSIVE_CAP = 5000; // Avoiding stack overflows

////
// DEFAULT VALUES:
////

// Jitter+Fork Generator
const MyFloat3 DEFAULT_JFG_START_PT            = MyFloat3(0.f, 100.f, 0.f);
const MyFloat3 DEFAULT_JFG_END_PT              = MyFloat3(0.f, 0.f, 0.f);
const size_t   DEFAULT_JFG_ITERATIONS          = 4;
const float    DEFAULT_JFG_CHAOS_MEAN          = .1f;
const float    DEFAULT_JFG_CHAOS_STDDEV        = .05f;
const float    DEFAULT_JFG_MIDPOINT_STDDEV     = .1f;
const float    DEFAULT_JFG_BASELINE_FORK_PROB  = .7f;
const float    DEFAULT_JFG_FORK_PROB_SCALEDOWN = .6f;

// Streamer Generator
const MyFloat3    DEFAULT_SG_START_PT          = MyFloat3(0.f, 100.f, 0.f);
const MyFloat3    DEFAULT_SG_INITIAL_DIRECTION = MyFloat3(0.f, -1.f, 0.f);
const float       DEFAULT_SG_VOLTAGE           = 42.f;
const float       DEFAULT_SG_INITIAL_PRESSURE  = 5.f;
const float       DEFAULT_SG_PRESSURE_GRADIENT = 0.f;
const size_t      DEFAULT_SG_MAX_NUM_LAYERS    = 10;
const std::string DEFAULT_SG_ANGLE_FIX         = "parent_dist";
const std::string DEFAULT_SG_GAS_COMPOSITION   = "air";

// Diameter Thinner:
const float DEFAULT_DT_SCALEDOWN = .5f;

// Whole Transformer:
const MyFloat3 DEFAULT_WT_START_POINT = MyFloat3(0.f, 100.f, 0.f);
const MyFloat3 DEFAULT_WT_END_POINT   = MyFloat3(0.f, 0.f, 0.f);

// Branchifier
const float  DEFAULT_B_INITIAL_DIAMETER      = .2f;
const float  DEFAULT_B_DIAMETER_SCALEDOWN    = .5f;
const float  DEFUALT_B_ANIMATION_TIME        = .5f;
const size_t DEFAULT_B_MAX_NUM_BRANCH_LEVELS = 4;

// Electrifier
const float DEFAULT_E_MAX_SEG_LENGTH = 1.f;
const float DEFAULT_E_CHAOS_MEAN     = .1f;
const float DEFAULT_E_CHAOS_STDDEV   = .05f;

// Render Mode:
const std::string DEFAULT_RENDER_MODE = "animated";

// Renderer Colours
const std::string DEFAULT_BACKGROUND_COLOUR = "background night";
const std::string DEFAULT_BLUR_COLOUR       = "lightning blue";
const std::string DEFAULT_LINE_COLOUR       = "lightning white";
const std::string DEFAULT_CAPSULE_COLOUR    = "lightning white";

// Blur Parameters
const float DEFAULT_BLUR_DIRECTIONS = 16.f;
const float DEFAULT_BLUR_QUALITY    = 30.f;
const float DEFAULT_BLUR_SIZE       = 30.f;
const float DEFAULT_BLUR_ADJUSTMENT = 15.f;

// Renderer Energy Parameters
const std::string DEFAULT_ENERGY_SCALE          = "linear";
const bool        DEFAULT_ENERGY_FOR_BLUR       = true;
const bool        DEFAULT_ENERGY_FOR_BRIGHTNESS = true;

// Animation parameters:
const float DEFAULT_ANIM_SPEED = 400.f;

////
// MIN & MAX VALUES:
////

// Jitter+Fork Generator
const size_t JFG_MIN_ITERATIONS = 1;
const size_t JFG_MAX_ITERATIONS = 10;

const float JFG_MIN_CHAOS_MEAN = 0.f;
const float JFG_MAX_CHAOS_MEAN = .2f;

const float JFG_MIN_CHAOS_STDDEV = 0.f;
const float JFG_MAX_CHAOS_STDDEV = .2f;

const float JFG_MIN_MIDPOINT_STDDEV = 0.f;
const float JFG_MAX_MIDPOINT_STDDEV = .3f;

const float JFG_MIN_BASELINE_FORK_PROB = 0.f;
const float JFG_MAX_BASELINE_FORK_PROB = 1.f;

const float JFG_MIN_FORK_PROB_SCALEDOWN = 0.f;
const float JFG_MAX_FORK_PROB_SCALEDOWN = 1.f;

// Streamer Generator
const float SG_MIN_VOLTAGE = 5.f;
const float SG_MAX_VOLTAGE = 100.f;

const float SG_MIN_INITIAL_PRESSURE = 0.f;
const float SG_MAX_INITIAL_PRESSURE = 50.f;

const float SG_MIN_PRESSURE_GRADIENT = -1.f;
const float SG_MAX_PRESSURE_GRADIENT = 1.f;

const size_t SG_MIN_MAX_NUM_LAYERS = 1;
const size_t SG_MAX_MAX_NUM_LAYERS = 10;

// Diameter Thinner
const float DT_MIN_SCALEDOWN = 0.f;
const float DT_MAX_SCALEDOWN = 1.f;

// Branchifier
const float B_MIN_INITIAL_DIAMETER = .1f;
const float B_MAX_INITIAL_DIAMETER = 2.f;

const float B_MIN_DIAMETER_SCALEDOWN = 0.f;
const float B_MAX_DIAMETER_SCALEDOWN = 1.f;

const float B_MIN_ANIMATION_TIME = 0.f;
const float B_MAX_ANIMATION_TIME = 1.f;

const size_t B_MIN_MAX_NUM_BRANCH_LEVELS = 1;
const size_t B_MAX_MAX_NUM_BRANCH_LEVELS = 10;

// Electrifier
const float E_MIN_MAX_SEG_LENGTH = .1f;
const float E_MAX_MAX_SEG_LENGTH = 10.f;

const float E_MIN_CHAOS_MEAN = 0.f;
const float E_MAX_CHAOS_MEAN = .2f;

const float E_MIN_CHAOS_STDDEV = 0.f;
const float E_MAX_CHAOS_STDDEV = .3f;

// Blur parameters
const float BLUR_MIN_DIRECTIONS = 1.f;
const float BLUR_MAX_DIRECTIONS = 64.f;

const float BLUR_MIN_QUALITY = 1.f;
const float BLUR_MAX_QUALITY = 50.f;

const float BLUR_MIN_SIZE = 1.f;
const float BLUR_MAX_SIZE = 100.f;

const float BLUR_MIN_ADJUSTMENT = 0.f;
const float BLUR_MAX_ADJUSTMENT = 50.f;

// Animation parameters:
const float ANIM_MIN_SPEED = 1.f;
const float ANIM_MAX_SPEED = 1000.f;
