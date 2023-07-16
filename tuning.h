#pragma once
#include "arm_math_types.h"


#define NUMBER_OF_TUNINGS 3


typedef struct {
	float32_t lowerBounds[6];
	float32_t upperBounds[6];
	float32_t tunedStrings[6];
	uint16_t tolerance;
} Tuning;


Tuning standardTuning = {
	{77.8f, 103.8f, 138.6f, 185.0f, 233.1f, 311.1f},
	{87.3f, 116.5f, 155.6f, 207.7f, 261.6f, 349.2f},
	{82.4f, 110.0f, 146.8f, 196.0f, 246.9f, 329.6f},
	0
};


Tuning dropDTuning = {
	{69.3f, 103.8f, 138.6f, 185.0f, 233.1f, 311.1f},
	{77.8f, 116.5f, 155.6f, 207.7f, 261.6f, 349.2f},
	{73.4f, 110.0f, 146.8f, 196.0f, 246.9f, 329.6f},
	1
};


Tuning fullStepDownTuning = {
	{69.3f, 92.5f, 123.5f, 164.8f, 207.7f, 277.2},
	{77.8f, 103.8f, 138.6f, 185.0f, 233.1f, 311.1f},
	{73.4f, 98.0f, 130.8f, 174.6f, 220.0f, 293.7f},
	2
};
