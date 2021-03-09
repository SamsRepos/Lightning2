#pragma once

class MyMemoryMeasurer
{
public:
	inline void SetObjectSize(size_t _objectSize) { objectSize = _objectSize; };
	inline void CalculateMemoryUsed(size_t numObjects) { memoryUsed = numObjects * objectSize; };

	inline size_t GetKbUsed() { return memoryUsed / 1024; };
	inline size_t GetMbUsed() { return GetKbUsed() / 1024; };
	inline size_t GetGbUsed() { return GetGbUsed() / 1024; };

private:
	size_t objectSize;
	size_t memoryUsed;
};

