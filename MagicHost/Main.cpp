#include <windows.h>
#include <time.h>
#include <math.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "Main.h"
#include "FPSCalculator.h"
#include "FPSLocker.h"
#include "HighPrecisionTime.h"
#include "DebuggerLog.h"


/**
 * @section
 * MagicHost Only Definitions
 */


// Window Size
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

// Image Size
extern const int IMAGE_WIDTH;
extern const int IMAGE_HEIGHT;

// Image Buffer
extern const int IMAGE_SIZE;
extern BYTE* image;

// Keyboard Buffer
extern char keyboardBuffer[256];

// FPS Counter & Locker
uint64 thisTime, lastTime;
extern FPSCalculator fpsCalculator;
FPSLocker fpsLocker(60);

// For Loading & Landing Animation
int squareSize = 50;
int squareX = (800 - squareSize) / 2;
int squareY = (600 - squareSize) / 2;
unsigned char colorCounter = 50;
float sinCurver = 0.0f;
bool countReversely = false;


/**
 * @section
 * Global Definitions
 */


// MSVC Compiler WChar Feature Compatibility.
#if defined(_MSC_VER)
#define G_PIPE_NAME TEXT("MagicDotHBuffer")
#else
#define G_PIPE_NAME ("MagicDotHBuffer")
#endif

// Screen Size
#define G_SCREEN_WIDTH 800
#define G_SCREEN_HEIGHT 600

// Process Pipe Offsets
#define PAGE_LENGTH 1440000  // (800 x 600 x 3) bytes
#define SIGN_LENGTH 8192
#define PIPE_LENGTH (SIGN_LENGTH+2*PAGE_LENGTH)


/**
 * @section
 * Pipe Definitions
 */

// Internal Definitions
#define exitSignal     (((char*)G_pBuf)[ 0])   // [ 0]
#define swapSignal     (((char*)G_pBuf)[ 1])   // [ 1]
#define gotitSignal    (((char*)G_pBuf)[ 2])   // [ 2]

// User API Definitions
#define fpsLockRate    (((int* )G_pBuf)[ 3])   // [ 3] [ 4] [ 5] [ 6]
#define isWinFocus     (((char*)G_pBuf)[ 7])   // [ 7]

#define mouseX         (((int* )G_pBuf)[ 8])   // [ 8] [ 9] [10] [11]
#define mouseY         (((int* )G_pBuf)[12])   // [12] [13] [14] [15]
#define isLeftClick    (((char*)G_pBuf)[16])   // [16]
#define isRightClick   (((char*)G_pBuf)[17])   // [17]

#define keyboard       (((char*)G_pBuf) +18)   // [18] [19] ... [273] (len: 256)

// String Transfer Definition
#define stringBuf      (((char*)G_pBuf) +274)  // [274] [275] ... [289] (len: 16)
#define stringLen      (((int* )G_pBuf)[290])  // [290] [291] [292] [293]
#define invokeTransfer (((char*)G_pBuf)[294])  // [294]
#define invokeSendBtch (((char*)G_pBuf)[295])  // [295]
#define invokeReceived (((char*)G_pBuf)[296])  // [296]


/**
 * @section
 * Global Variables
 */


// Win32 API Related
HANDLE  G_hMapFile = NULL;  // Shared Memory Handle
LPCTSTR G_pBuf = NULL;      // Shared Memory Buffer
unsigned char* G_pixels;

// Process Pipe Offset
int G_bufferDelta = PAGE_LENGTH + SIGN_LENGTH;  // Start from the second buffer.


/**
 * @section
 * Internal Functions, SHOULD NOT BE CALLED BY USERS!
 */


bool IsWindowFocused(HWND hwnd) {
	// Get the currently focused window
	HWND focusedWindow = GetForegroundWindow();

	// Compare whether the handles are the same
	return (focusedWindow == hwnd);
}

void LandingAnimation(bool r, bool g, bool b) {
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
			image[(currentY * 800 + squareX + x) * 4 + 0] = b ? colorCounter : 0;
			image[(currentY * 800 + squareX + x) * 4 + 1] = g ? colorCounter : 0;
			image[(currentY * 800 + squareX + x) * 4 + 2] = r ? colorCounter : 0;
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

// DISABLE MSVC OPEIMIZATION for Internal_ReceiveString() Function: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

char* Internal_ReceiveString() {
	// If there's no invokation requests, just return.
	if (!invokeTransfer) {
		return nullptr;
	}

	// Process Invokation, receiving the length of the string.
	int length = stringLen;
	invokeTransfer = 0;
	invokeReceived = 1;

	// Allocate Memory
	char* dest = new char[length + 1];

	// Receiving the long string batch by batch.
	int howManyBatch = length / 16 + (length % 16 == 0 ? 0 : 1);
	for (int batch = 0; batch < howManyBatch; batch++) {

		while (!invokeSendBtch);  // Wait Until the "Send Batch" Signal Comes.

		// Send a single batch of string.
		char* ptr = dest + batch * 16;  // Destination: Starting Position

		// Manual String Copy, Because '\0' ONLY APPEARED IN THE LAST BATCH.
		for (int i = 0; i < 16; i++) {
			ptr[i] = stringBuf[i];
			if (stringBuf[i] == '\0') {
				break;
			}
		}

		// Send the "Got it" Signal
		invokeSendBtch = 0;
		invokeReceived = 1;
	}

	return dest;
}

// DISABLE MSVC OPEIMIZATION for Internal_ReceiveString() Function: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif


/**
 * @section
 * Main Functions, Setup() & Update()
 */


bool firstTimeSetup = true;
Mix_Music* bgMusic;
int count = 0;

void Setup(HWND& hwnd, bool* wannaUpdate) {

	if (firstTimeSetup) {
		// Play Music
		Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
		bgMusic = Mix_LoadMUS(".\\Music\\bg.wav");
	}

	if (count == 0) {
		// Play Music
		Mix_PlayMusic(bgMusic, -1);
	}

	/*if (count % 100 == 50) {
		Mix_PauseMusic();
	}

	if (count % 100 == 0) {
		Mix_ResumeMusic();
	}*/

	/*if (count % 100 == 50) {
		Mix_HaltMusic();
	}

	if (count % 100 == 0) {
		Mix_PlayMusic(bgMusic, -1);
	}*/

	count++;

	// ----------

	if (firstTimeSetup) {
		// Initialize the time counters
		thisTime = lastTime = MicroClock();

		// Update the flag
		firstTimeSetup = false;
	}

	// Loading & Landing Animation
	LandingAnimation(true, true, false);

	/*
	** --------------------------------------------------
	** Initialize Shared Memory
	** --------------------------------------------------
	*/

	// @@@ Open Shared Memory
	G_hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,    // READ & WRITE Permission
		FALSE,                  // DO NOT Inherit
		G_PIPE_NAME             // NAME of Shared Memory
	);
	OutputDebugString(L"Open Shared Memory Failed, Retry...\n");

	if (G_hMapFile != NULL) {
		// @@@ Map Up the Shared Memory
		G_pBuf = (LPTSTR)MapViewOfFile(
			G_hMapFile,               // HANDLE of Shared Memory
			FILE_MAP_ALL_ACCESS,    // READ & WRITE Permission
			0,                      // Mapping Offset
			0,                      // Mapping Offset
			PIPE_LENGTH             // Mapping SIZE, (SIGN_LENGTH+2*PAGE_LENGTH)
		);
		OutputDebugString(L"Map Shared Memory Failed, Retry...\n");

		if (G_pBuf != NULL) {
			// Enter the Main Game-Loop
			*wannaUpdate = true;
		}
	}

	if (*wannaUpdate == true) {
		// Initialize Process Pipe to ALL 0.
		memset((void*)G_pBuf, 0, PIPE_LENGTH);

		// Ensure this value is initialized.
		fpsLockRate = 60;
	}

	// Count & Lock FPS
	thisTime = MicroClock();
	fpsCalculator.Count(thisTime - lastTime);
	fpsLocker.Lock(thisTime - lastTime);
	lastTime = thisTime;
}

bool firstTimeUpdate = true;
bool isShowEverCalled = false;

void Update(HWND& hwnd, bool* wannaExit) {

	if (firstTimeUpdate) {
		// Do Something
		;

		// Update the flag
		firstTimeUpdate = false;
	}

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
			isLeftClick = 1;
		}
		else {
			isLeftClick = 0;
		}
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
			isRightClick = 1;
		}
		else {
			isRightClick = 0;
		}
	}

	// String Transfer thru Pipe.
	char* result = Internal_ReceiveString();
	if (result != nullptr) {
		DebuggerLog(result);
	}

	/*
	** Ever Called Show() in the Client Side?
	** No : Front Buffer is currently empty, no need to do copying.
	**      Just show the LandingAnimation(), and wait for the Swap Signal.
	** Yes: Need to do Frame Buffer copying.
	**
	** No Matter Ever Called Show() or not, the Swap Signal should ALWAYS be processed.
	*/

	if (isShowEverCalled == false) {
		// Prevent Black Screen Appear when User Only Called Magic()
		// And Pushed NO FRAMES. A User-Friendly Feature.
		LandingAnimation(true, true, true);
	}
	else {
		// RAPID Frame Buffer Copying Using Pointer Technique.
		int loopTimes = 800 * 600;
		char* src = ((char*)G_pBuf) + G_bufferDelta;  // BGR  BGR  BGR  ; 800x600
		char* dst = (char*)image;                 // BGR0 BGR0 BGR0 ; 800x600

		for (int i = 0; i < loopTimes; i++) {
			*(dst++) = *(src++);  // B
			*(dst++) = *(src++);  // G
			*(dst++) = *(src++);  // R
			*(dst++) = 0;
		}
	}

	// Process Swap Signal
	if (swapSignal == (unsigned char)1) {
		isShowEverCalled = true;

		swapSignal = (unsigned char)0;
		G_bufferDelta = G_bufferDelta == SIGN_LENGTH ? PAGE_LENGTH + SIGN_LENGTH : SIGN_LENGTH;
		gotitSignal = (unsigned char)1;
	}

	// Count & Lock FPS
	thisTime = MicroClock();
	fpsCalculator.Count(thisTime - lastTime);
	fpsLocker.Lock(thisTime - lastTime);
	lastTime = thisTime;
}

void Exit() {

	/*
	** --------------------------------------------------
	** Finalize Shared Memory
	** --------------------------------------------------
	*/

	UnmapViewOfFile(G_pBuf);
	CloseHandle(G_hMapFile);
	OutputDebugString(L"---------- Exited Cleanly. ----------\n");
}
