#pragma once

#include "DXF.h"

float magnitude(const XMFLOAT3& vector);
XMFLOAT3 normalised(XMFLOAT3& vector);

XMFLOAT3 crossProduct(const XMFLOAT3& u, const XMFLOAT3& v);
XMFLOAT3 operator+(const XMFLOAT3& a, const XMFLOAT3& b);
XMFLOAT3 operator-(const XMFLOAT3& minuend, const XMFLOAT3& subtrahend);

XMFLOAT3 operator*(const XMFLOAT3& vector, const float scale);
XMFLOAT3 operator/(const XMFLOAT3& vector, const float denominator);

#define DEFAULT_RANDOM_VECTOR_RESOLUTION 1000
XMFLOAT3 randomNormalisedVector(int resolution);
XMFLOAT3 randomNormalisedVector();
