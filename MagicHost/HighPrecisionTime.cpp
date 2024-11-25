#include "HighPrecisionTime.h"
#include <Windows.h>

LARGE_INTEGER frequency;
bool isFrequencyInitialized = false;

void initializeFrequency() {
	if (!isFrequencyInitialized) {
		QueryPerformanceFrequency(&frequency);
		isFrequencyInitialized = true;
	}
}

uint64 MicroClock() {
	initializeFrequency();

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	// 计算经过的微秒数
	return static_cast<uint64>(counter.QuadPart * 1e6 / frequency.QuadPart);
}

void MicroSleep(unsigned int microseconds) {
	LARGE_INTEGER frequency, start, end;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	double elapsed = 0;
	do {
		QueryPerformanceCounter(&end);
		elapsed = (end.QuadPart - start.QuadPart) * 1e6 / frequency.QuadPart;
	} while (elapsed < microseconds);
}
