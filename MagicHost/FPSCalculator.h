#ifndef __FPSCalculator_H__
#define __FPSCalculator_H__

class FPSCalculator {

private:
	float fps;
	int deltaTimeCount;
	int frameCount;

public:
	FPSCalculator();
	void Count(int deltaTime);  // In Microseconds
	float GetCurrentFPS();
};

#endif
