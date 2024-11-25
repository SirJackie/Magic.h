#include "FPSLocker.h"

FPSLocker::FPSLocker(float targetFPS_)
{
	ModifyTargetFPS(targetFPS_);
}

void FPSLocker::ModifyTargetFPS(float targetFPS_)
{
	targetFPS = targetFPS_;
	targetDT = 1000000.0f / targetFPS;  // In Microseconds
	sleepT = 0;
	correctionT = 0;
}

void FPSLocker::ModifyTargetFPSSafe(float targetFPS_)
{
	// Safe: Safe to Call Repeatedly in Every Frame.
	if (targetFPS_ != targetFPS) {
		ModifyTargetFPS(targetFPS_);
	}
}

void FPSLocker::Lock(int deltaTime)
{
	correctionT = int(targetDT) - int(deltaTime);  // Can be positive or negative
	sleepT += correctionT;
	sleepT = max(0, sleepT);

	MicroSleep(sleepT);
}
