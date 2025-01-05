#include "DebuggerLog.h"
#include <Windows.h>

void DebuggerLog(const char* str) {
	wchar_t wideBuffer[WIDE_BUFFER_LEN] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, str, -1, wideBuffer, WIDE_BUFFER_LEN);
	OutputDebugString(wideBuffer);
}

void DebuggerLogW(const wchar_t* wideStr) {
	OutputDebugString(wideStr);
}
