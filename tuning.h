#pragma once
#include "arm_math_types.h"


typedef struct {
	float32_t lowerBounds[6];
	float32_t upperBounds[6];
	float32_t tunedStrings[6];
} Tuning;


Tuning standardTuning = {
	{77.8f, 103.8f, 138.6f, 185.0f, 233.1f, 311.1f},
	{87.3f, 116.5f, 155.6f, 207.7f, 261.6f, 349.2f},
	{82.4f, 110.0f, 146.8f, 196.0f, 246.9f, 329.6f}
};
