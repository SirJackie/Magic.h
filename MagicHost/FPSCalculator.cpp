#include "FPSCalculator.h"

FPSCalculator::FPSCalculator() {
	fps             = 0.0f;
	deltaTimeCount  = 0;
	frameCount      = 0;
}

void FPSCalculator::Count(int deltaTime)  // In Microseconds
{
	/* Calculate FPS */
	deltaTimeCount += deltaTime;
	frameCount += 1;

	if (deltaTimeCount > 1000000) {
		fps = (float)frameCount / ((float)deltaTimeCount / 1000000.0f);
		deltaTimeCount -= 1000000;
		frameCount = 0;
	}
}

float FPSCalculator::GetCurrentFPS()
{
	return fps;
}
