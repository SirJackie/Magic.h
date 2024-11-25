#include <windows.h>
#include <time.h>
#include <math.h>
#include "Main.h"
#include "FPSCalculator.h"
#include "FPSLocker.h"
#include "HighPrecisionTime.h"

// Window Size
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

// Image Size
extern const int IMAGE_WIDTH;
extern const int IMAGE_HEIGHT;

// Image Buffer
extern const int IMAGE_SIZE;
extern BYTE* image;

// Shared Memory
HANDLE  hMapFile = NULL;       // Shared Memory Handle
LPCTSTR pBuf = NULL;           // Shared Memory Buffer

// ----------

#define PAGE_LENGTH 1440000  // (800 x 600 x 3) bytes
#define SIGN_LENGTH 8192
#define PIPE_LENGTH (SIGN_LENGTH+2*PAGE_LENGTH)

// ----------

extern char keyboardBuffer[256];

#define exitSignal   (((char*)pBuf)[0])  // [0]
#define swapSignal   (((char*)pBuf)[1])  // [1]
#define gotitSignal  (((char*)pBuf)[2])  // [2]
#define fpsLockRate  (((int* )pBuf)[3])  // [3] [4] [5] [6]
#define isWinFocus   (((char*)pBuf)[7])  // [7]

#define mouseX       (((int* )pBuf)[ 8])  // [ 8] [ 9] [10] [11]
#define mouseY       (((int* )pBuf)[12])  // [12] [13] [14] [15]
#define isLeftClick  (((char*)pBuf)[16])  // [16]
#define isRightClick (((char*)pBuf)[17])  // [17]

#define keyboard     (((char*)pBuf) +18)  // [18] [19] ... [273]

int bufferDelta = PAGE_LENGTH + SIGN_LENGTH;  // Start from the second buffer.

// ----------

// FPS Counter & Locker
uint64 thisTime, lastTime;
extern FPSCalculator fpsCalculator;
FPSLocker fpsLocker(60);

bool IsWindowFocused(HWND hwnd) {
	// 获取当前拥有焦点的窗口
	HWND focusedWindow = GetForegroundWindow();

	// 比较句柄是否相同
	return (focusedWindow == hwnd);
}

// For Loading & Landing Animation
int squareSize = 50;
int squareX = (800 - squareSize) / 2;
int squareY = (600 - squareSize) / 2;
unsigned char colorCounter = 50;
float sinCurver = 0.0f;
bool countReversely = false;

void LandingAnimation() {
	// Clear Screen with Black: (0, 0, 0)
	for (int y = 0; y < 600; y++) {
		for (int x = 0; x < 800; x++) {
			image[(y * 800 + x) * 4 + 0] = 0;
			image[(y * 800 + x) * 4 + 1] = 0;
			image[(y * 800 + x) * 4 + 2] = 0;
		}
	}

	// Loading & Landing Animation
	for (int y = 0; y < squareSize; y++) {
		int currentY = squareY - 50 + y + sin(sinCurver) * 60;
		for (int x = 0; x < squareSize; x++) {
			image[(currentY * 800 + squareX + x) * 4 + 0] = colorCounter;
			image[(currentY * 800 + squareX + x) * 4 + 1] = colorCounter;
			image[(currentY * 800 + squareX + x) * 4 + 2] = colorCounter;
		}
	}
	sinCurver += 0.08f;

	if (colorCounter == 255) {
		countReversely = true;
	}
	else if (colorCounter == 50) {
		countReversely = false;
	}

	if (countReversely) {
		colorCounter -= 5;  // MUST be factor of 255
	}
	else {
		colorCounter += 5;  // MUST be factor of 255
	}
}

void Setup(HWND& hwnd, bool* wannaUpdate) {

	// Initialize the time counters
	thisTime = lastTime = MicroClock();

	// Loading & Landing Animation
	LandingAnimation();

	/*
	** --------------------------------------------------
	** Initialize Shared Memory
	** --------------------------------------------------
	*/

	// @@@ Open Shared Memory
	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,    // READ & WRITE Permission
		FALSE,                  // DO NOT Inherit
		TEXT("MagicDotHBuffer") // NAME of Shared Memory
	);
	OutputDebugString(L"Open Shared Memory Failed, Retry...\n");
	Sleep(10);

	if (hMapFile != NULL) {
		// @@@ Map Up the Shared Memory
		pBuf = (LPTSTR)MapViewOfFile(
			hMapFile,               // HANDLE of Shared Memory
			FILE_MAP_ALL_ACCESS,    // READ & WRITE Permission
			0,                      // Mapping Offset
			0,                      // Mapping Offset
			PIPE_LENGTH             // Mapping SIZE, (SIGN_LENGTH+2*PAGE_LENGTH)
		);
		OutputDebugString(L"Map Shared Memory Failed, Retry...\n");
		Sleep(10);

		if (pBuf != NULL) {
			// Enter the Main Game-Loop
			*wannaUpdate = true;
		}
	}

	// Count & Lock FPS
	thisTime = MicroClock();
	fpsCalculator.Count(thisTime - lastTime);
	fpsLocker.Lock(thisTime - lastTime);
	lastTime = thisTime;
}

bool everUpdated = false;

void Update(HWND& hwnd, bool* wannaExit) {

	/*
	** --------------------------------------------------
	** Utilize Shared Memory
	** --------------------------------------------------
	*/

	// Exit Signal Processing.
	if (exitSignal == (unsigned char)1) {
		*wannaExit = true;
		gotitSignal = (unsigned char)1;
	}

	// FPS Lock Rate Updating thru Pipe.
	fpsLocker.ModifyTargetFPSSafe(fpsLockRate);

	// Is Win Focus Detection
	isWinFocus = IsWindowFocused(hwnd) ? 1 : 0;

	// Keyboard Sending thru Pipe.
	for (int i = 0; i < 256; i++) {
		keyboard[i] = keyboardBuffer[i];
	}

	// Mouse Position Sending thru Pipe.
	if (isWinFocus == 1) {
		// Only Updating when Window is Focused.
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hwnd, &pt);
		mouseX = pt.x;
		mouseY = pt.y;

		// Key Pressed Or Not
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
			isLeftClick  = 1;
		}
		else {
			isLeftClick  = 0;
		}
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
			isRightClick = 1;
		}
		else {
			isRightClick = 0;
		}
	}

	// RAPID IMAGE COPY Using Pointer Technique.
	int loopTimes = 800 * 600;
	char* src = ((char*)pBuf) + bufferDelta;  // BGR  BGR  BGR  ; 800x600
	char* dst = (char*)image;                 // BGR0 BGR0 BGR0 ; 800x600

	for (int i = 0; i < loopTimes; i++) {
		*(dst++) = *(src++);  // B
		*(dst++) = *(src++);  // G
		*(dst++) = *(src++);  // R
		*(dst++) = 0;
	}

	if (everUpdated == false) {
		// Prevent Black Screen Appear when User Only Called Magic()
		// And Pushed NO FRAMES. A User-Friendly Feature.
		LandingAnimation();

		// Count & Lock FPS
		thisTime = MicroClock();
		fpsCalculator.Count(thisTime - lastTime);
		fpsLocker.Lock(thisTime - lastTime);
		lastTime = thisTime;
	}

	if (swapSignal == (unsigned char)1) {
		everUpdated = true;

		swapSignal = (unsigned char)0;
		bufferDelta = bufferDelta == SIGN_LENGTH ? PAGE_LENGTH + SIGN_LENGTH : SIGN_LENGTH;
		gotitSignal = (unsigned char)1;

		// Count & Lock FPS
		thisTime = MicroClock();
		fpsCalculator.Count(thisTime - lastTime);
		fpsLocker.Lock(thisTime - lastTime);
		lastTime = thisTime;
	}
}

void Exit() {

	/*
	** --------------------------------------------------
	** Finalize Shared Memory
	** --------------------------------------------------
	*/

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	OutputDebugString(L"---------- Exited Cleanly. ----------\n");
}
