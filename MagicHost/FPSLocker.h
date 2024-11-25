#ifndef __FPSLocker_H__
#define __FPSLocker_H__

#include <Windows.h>
#include "HighPrecisionTime.h"

class FPSLocker {
public:
	float targetFPS;
	float targetDT;
	
	int sleepT;  // Can be positive or negative
	int correctionT;  // Can be positive or negative

	FPSLocker(float targetFPS_);
	void ModifyTargetFPS(float targetFPS_);
	void ModifyTargetFPSSafe(float targetFPS_);
	void Lock(int deltaTime);
};

#endif
