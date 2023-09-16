#pragma once

#include <random>

float GetRandFloat(float min, float max)
{
	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_real_distribution<float> distribution(min, max);

	return distribution(engine);
}