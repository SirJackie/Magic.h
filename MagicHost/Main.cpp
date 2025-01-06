#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>
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

// For Music Interface

Mix_Music* sdlMusicPtr = nullptr;
Mix_Chunk* sdlChunkPtrs[MAX_MUSIC_CHANNELS] = { nullptr };
bool isChannelPlaying[MAX_MUSIC_CHANNELS] = { false };

// For Text Interface
bool textEnabled[MAX_TEXT_CHANNELS] = { false };
wchar_t* textPointers[MAX_TEXT_CHANNELS] = { nullptr };

unsigned char textColorR[MAX_TEXT_CHANNELS] = { 0 };
unsigned char textColorG[MAX_TEXT_CHANNELS] = { 0 };
unsigned char textColorB[MAX_TEXT_CHANNELS] = { 0 };

unsigned char bgColorR[MAX_TEXT_CHANNELS] = { 0 };
unsigned char bgColorG[MAX_TEXT_CHANNELS] = { 0 };
unsigned char bgColorB[MAX_TEXT_CHANNELS] = { 0 };
bool bgTransparent[MAX_TEXT_CHANNELS] = { false };

int textX[MAX_TEXT_CHANNELS] = { 0 };
int textY[MAX_TEXT_CHANNELS] = { 0 };
int textWidth[MAX_TEXT_CHANNELS] = { 0 };
int textHeight[MAX_TEXT_CHANNELS] = { 0 };
TextLayout textLayout[MAX_TEXT_CHANNELS] = { WORDBREAK };

// For Font Interface
bool fontEnabled[MAX_TEXT_CHANNELS] = { false };
HFONT defaultFontPtr = NULL;
HFONT fontPtrs[MAX_TEXT_CHANNELS] = { NULL };

unsigned char invokeInternalFontCreator = 0;
unsigned char invokeInternalFontDeletor = 0;
int invokeFontChannel = 0;
int invokeFontSize = 0;
bool invokeFontItalic = false;
bool invokeFontBold = false;
bool invokeFontUnderline = false;
wchar_t* invokeFontFamily = nullptr;


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
#define invokeBufSwap  (((char*)G_pBuf)[ 1])   // [ 1]
#define invokeReceived (((char*)G_pBuf)[ 2])   // [ 2]

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

// Music & Text Interface
#define invokeMusic    (((char*)G_pBuf)[296])  // [296]
#define invokeText     (((char*)G_pBuf)[297])  // [297]


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


/**
 * @section
 * String Transfer
 */

#define WIDE_PIPE_SIZE (16 / sizeof(wchar_t))

// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

void Internal_SendString(const char* str) {

	// Save the length of the string to pipe.
	int length = strlen(str) + 1;
	stringLen = length;

	// Invoke String Transfer
	invokeReceived = 0;
	invokeTransfer = 1;
	while (invokeReceived == 0);  // Wait for Response
	invokeReceived = 0;

	// Sending the long string batch by batch.
	int howManyBatch = length / 16 + (length % 16 == 0 ? 0 : 1);
	for (int batch = 0; batch < howManyBatch; batch++) {

		// Send a single batch of string.
		const char* ptr = str + batch * 16;  // Source: Starting Position

		// Manual String Copy, Because '\0' ONLY APPEARED IN THE LAST BATCH.
		for (int i = 0; i < 16; i++) {
			stringBuf[i] = ptr[i];
			if (ptr[i] == '\0') {
				break;
			}
		}

		// Invoke "Send Batch" Signal
		invokeReceived = 0;
		invokeSendBtch = 1;
		while (invokeReceived == 0);  // Wait for Response
		invokeReceived = 0;
	}
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif

// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

void Internal_SendStringW(const wchar_t* wideStr) {

	// Save the length of the string to pipe.
	int length = wcslen(wideStr) + 1;
	stringLen = length;

	// Invoke String Transfer
	invokeReceived = 0;
	invokeTransfer = 1;
	while (invokeReceived == 0);  // Wait for Response
	invokeReceived = 0;

	// Sending the long string batch by batch.
	int howManyBatch = length / WIDE_PIPE_SIZE + (length % WIDE_PIPE_SIZE == 0 ? 0 : 1);
	for (int batch = 0; batch < howManyBatch; batch++) {

		// Send a single batch of string.
		const wchar_t* ptr = wideStr + batch * WIDE_PIPE_SIZE;  // Source: Starting Position

		// Manual String Copy, Because '\0' ONLY APPEARED IN THE LAST BATCH.
		for (int i = 0; i < WIDE_PIPE_SIZE; i++) {
			((wchar_t*)stringBuf)[i] = ptr[i];
			if (ptr[i] == L'\0') {
				break;
			}
		}

		// Invoke "Send Batch" Signal
		invokeReceived = 0;
		invokeSendBtch = 1;
		while (invokeReceived == 0);  // Wait for Response
		invokeReceived = 0;
	}
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif

// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

char* Internal_ReceiveString() {

	// Wait for Invoke Signal
	while (invokeTransfer == 0);

	// Receive the length of the string
	int length = stringLen;

	// Process Invoke Signal
	invokeTransfer = 0;
	invokeReceived = 1;

	// Allocate Memory
	char* dest = new char[length + 1];

	// Receiving the long string batch by batch.
	int howManyBatch = length / 16 + (length % 16 == 0 ? 0 : 1);
	for (int batch = 0; batch < howManyBatch; batch++) {

		// Wait for Invoke Signal
		while (invokeSendBtch == 0);

		// Manual String Copy, Because '\0' ONLY APPEARED IN THE LAST BATCH.
		char* ptr = dest + batch * 16;  // Destination: Starting Position
		for (int i = 0; i < 16; i++) {
			ptr[i] = stringBuf[i];
			if (stringBuf[i] == '\0') {
				break;
			}
		}

		// Process Invoke Signal
		invokeSendBtch = 0;
		invokeReceived = 1;
	}

	return dest;
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif

// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

wchar_t* Internal_ReceiveStringW() {

	// Wait for Invoke Signal
	while (invokeTransfer == 0);

	// Receive the length of the string
	int length = stringLen;

	// Process Invoke Signal
	invokeTransfer = 0;
	invokeReceived = 1;

	// Allocate Memory
	wchar_t* dest = new wchar_t[length + 1];

	// Receiving the long string batch by batch.
	int howManyBatch = length / WIDE_PIPE_SIZE + (length % WIDE_PIPE_SIZE == 0 ? 0 : 1);
	for (int batch = 0; batch < howManyBatch; batch++) {

		// Wait for Invoke Signal
		while (invokeSendBtch == 0);

		// Manual String Copy, Because '\0' ONLY APPEARED IN THE LAST BATCH.
		wchar_t* ptr = dest + batch * WIDE_PIPE_SIZE;  // Destination: Starting Position
		for (int i = 0; i < WIDE_PIPE_SIZE; i++) {
			ptr[i] = ((wchar_t*)stringBuf)[i];
			if (((wchar_t*)stringBuf)[i] == L'\0') {
				break;
			}
		}

		// Process Invoke Signal
		invokeSendBtch = 0;
		invokeReceived = 1;
	}

	return dest;
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif


/**
 * @section
 * Arg Parser
 */

int strFindSpaceOrZero(const char* str) {
	int i = 0;
	while (true) {
		if (str[i] == ' ') {
			return i;
		}
		if (str[i] == '\0') {
			return -1;
		}
		i++;
	}
}

int strFindSpaceOrZeroW(const wchar_t* str) {
	int i = 0;
	while (true) {
		if (str[i] == L' ') {
			return i;
		}
		if (str[i] == L'\0') {
			return -1;
		}
		i++;
	}
}

void strcpy_firstN(char* dst, const char* src, int n) {
	for (int i = 0; i < n; i++) {
		dst[i] = src[i];
	}
	dst[n] = '\0';
}

void wcsncpy_firstN(wchar_t* dst, const wchar_t* src, int n) {
	for (int i = 0; i < n; i++) {
		dst[i] = src[i];
	}
	dst[n] = L'\0';
}

int str2int(const char* str) {
	int result;
	sscanf_s(str, "%d", &result);
	return result;
}

int str2intW(const wchar_t* str) {
	int result;
	swscanf_s(str, L"%d", &result);
	return result;
}

#define MAX_ARG_PARSE_AMOUNT 100

void ArgParser(const char* command, int* argcPtr, char*** argvPtr) {
	// Allocate a String Pointer List for argv
	(*argvPtr) = new char* [MAX_ARG_PARSE_AMOUNT];
	for (int i = 0; i < MAX_ARG_PARSE_AMOUNT; i++) {
		(*argvPtr)[i] = nullptr;
	}

	const char* ptr = command;
	int spaceIdx = 0;
	bool inQuotes = false;
	char quoteChar = '\0'; // Tracks which quote type we're inside of

	while (*ptr != '\0') {
		while (isspace(*ptr)) {
			ptr++; // Skip leading spaces
		}

		if (*ptr == '\0') {
			break;
		}

		const char* start = ptr;
		if (*ptr == '"' || *ptr == '\'') {
			inQuotes = true;
			quoteChar = *ptr;
			start++; // Skip the opening quote
			ptr++;
			while (*ptr != '\0' && *ptr != quoteChar) {
				ptr++;
			}

			if (*ptr == quoteChar) {
				inQuotes = false;
				int length = ptr - start;
				(*argvPtr)[*argcPtr] = new char[length + 1];
				strncpy((*argvPtr)[*argcPtr], start, length);
				(*argvPtr)[*argcPtr][length] = '\0';
				(*argcPtr)++;
				ptr++; // Skip the closing quote
			}
		}
		else {
			spaceIdx = strFindSpaceOrZero(ptr);
			if (spaceIdx != -1) { // First N Args
				(*argvPtr)[*argcPtr] = new char[spaceIdx + 1];
				strcpy_firstN((*argvPtr)[*argcPtr], ptr, spaceIdx);
				(*argcPtr)++;
				ptr += spaceIdx;
			}
			else { // Last Arg
				int bufSize = strlen(ptr) + 1;
				(*argvPtr)[*argcPtr] = new char[bufSize];
				strcpy_s((*argvPtr)[*argcPtr], bufSize, ptr);
				(*argcPtr)++;
				break;
			}
		}

		while (isspace(*ptr)) {
			ptr++; // Skip trailing spaces
		}
	}
}

void ArgParserW(const wchar_t* command, int* argcPtr, wchar_t*** argvPtr) {
	// Allocate a String Pointer List for argv
	(*argvPtr) = new wchar_t* [MAX_ARG_PARSE_AMOUNT];
	for (int i = 0; i < MAX_ARG_PARSE_AMOUNT; i++) {
		(*argvPtr)[i] = nullptr;
	}

	const wchar_t* ptr = command;
	int spaceIdx = 0;
	bool inQuotes = false;
	wchar_t quoteChar = L'\0'; // Tracks which quote type we're inside of

	while (*ptr != L'\0') {
		while (iswspace(*ptr)) {
			ptr++; // Skip leading spaces
		}

		if (*ptr == L'\0') {
			break;
		}

		const wchar_t* start = ptr;
		if (*ptr == L'"' || *ptr == L'\'') {
			inQuotes = true;
			quoteChar = *ptr;
			start++; // Skip the opening quote
			ptr++;
			while (*ptr != L'\0' && *ptr != quoteChar) {
				ptr++;
			}

			if (*ptr == quoteChar) {
				inQuotes = false;
				int length = ptr - start;
				(*argvPtr)[*argcPtr] = new wchar_t[length + 1];
				wcsncpy((*argvPtr)[*argcPtr], start, length);
				(*argvPtr)[*argcPtr][length] = L'\0';
				(*argcPtr)++;
				ptr++; // Skip the closing quote
			}
		}
		else {
			spaceIdx = strFindSpaceOrZeroW(ptr);
			if (spaceIdx != -1) { // First N Args
				(*argvPtr)[*argcPtr] = new wchar_t[spaceIdx + 1];
				wcsncpy_firstN((*argvPtr)[*argcPtr], ptr, spaceIdx);
				(*argcPtr)++;
				ptr += spaceIdx;
			}
			else { // Last Arg
				int bufSize = wcslen(ptr) + 1;
				(*argvPtr)[*argcPtr] = new wchar_t[bufSize];
				wcscpy_s((*argvPtr)[*argcPtr], bufSize, ptr);
				(*argcPtr)++;
				break;
			}
		}

		while (iswspace(*ptr)) {
			ptr++; // Skip trailing spaces
		}
	}
}

void ArgParser_Freer(int* argcPtr, char*** argvPtr) {
	for (int i = 0; i < (*argcPtr); i++) {
		delete[](*argvPtr)[i];
		(*argvPtr)[i] = nullptr;
	}
	delete[](*argvPtr);
	(*argvPtr) = nullptr;
}

void ArgParserW_Freer(int* argcPtr, wchar_t*** argvPtr) {
	for (int i = 0; i < (*argcPtr); i++) {
		delete[] (*argvPtr)[i];
		(*argvPtr)[i] = nullptr;
	}
	delete[] (*argvPtr);
	(*argvPtr) = nullptr;
}

#ifndef OUT
#define OUT
#endif

class ArgFinder {
private:
	int argc;
	char** argv;
	bool* argFetched;

public:
	ArgFinder(int argc_, char** argv_) {
		this->argc = argc_;
		this->argv = argv_;
		this->argFetched = new bool[this->argc];
		for (int i = 0; i < this->argc; i++) {
			this->argFetched[i] = false;
		}
	}

	// return bool: true means found, false means cannot found.
	bool FetchArg(const char* targetStr, int fetchHowMany, char*** OUT resultArgv) {
		for (int i = 0; i < this->argc; i++) {
			if (strcmp(targetStr, this->argv[i]) == 0 && this->argFetched[i] == false) {
				*resultArgv = this->argv + i;
				for (int j = i; j < i + fetchHowMany; j++) {
					this->argFetched[j] = true;
				}
				return true;
			}
		}
		return false;
	}

	void SetFetchedFlag(int startIndex, int fetchHowMany) {
		for (int i = startIndex; i < startIndex + fetchHowMany; i++) {
			this->argFetched[i] = true;
		}
	}

	~ArgFinder() {
		delete[] this->argFetched;
		this->argFetched = nullptr;
	}
};

class ArgFinderW {
private:
	int argc;
	wchar_t** argv;
	bool* argFetched;

public:
	ArgFinderW(int argc_, wchar_t** argv_) {
		this->argc = argc_;
		this->argv = argv_;
		this->argFetched = new bool[this->argc];
		for (int i = 0; i < this->argc; i++) {
			this->argFetched[i] = false;
		}
	}

	// return bool: true means found, false means cannot found.
	bool FetchArg(const wchar_t* targetStr, int fetchHowMany, wchar_t*** OUT resultArgv) {
		for (int i = 0; i < this->argc; i++) {
			if (wcscmp(targetStr, this->argv[i]) == 0 && this->argFetched[i] == false) {
				*resultArgv = this->argv + i;
				for (int j = i; j < i + fetchHowMany; j++) {
					this->argFetched[j] = true;
				}
				return true;
			}
		}
		return false;
	}

	void SetFetchedFlag(int startIndex, int fetchHowMany) {
		for (int i = startIndex; i < startIndex + fetchHowMany; i++) {
			this->argFetched[i] = true;
		}
	}

	~ArgFinderW() {
		delete[] this->argFetched;
		this->argFetched = nullptr;
	}
};


/**
 * @section
 * Magic Interfaces
 */

// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

void MagicMusic_Receiver() {
	// Process Invoke Signal
	if (invokeMusic == 1) {

		// Process Invoke Signal
		invokeMusic = 0;
		invokeReceived = 1;

		// String Transfer thru Pipe.
		char* command = Internal_ReceiveString();

		// Parse arguments in the command.
		int argc = 0;
		char** argv = nullptr;
		ArgParser(command, &argc, &argv);

		//
		// Process Music Command: START
		//

		// Error Handing.
		const char* errorString = nullptr;

		// Open Command; Example:
		// "open .\\Music\\bg.wav on_channel 0"
		if (strcmp(argv[0], "open") == 0) {
			if (argc != 4) {
				errorString = "Invalid Args for 'open' command: Not equal to 4.";
			}
			else {
				const char* filename = argv[1];
				int channel = str2int(argv[3]);

				if (channel == 0) {
					sdlMusicPtr = Mix_LoadMUS(filename);
				}
				else {
					sdlChunkPtrs[channel] = Mix_LoadWAV(filename);
				}
			}
		}
		
		// Play Command; Example:
		// "play channel 0 times -1"
		else if (strcmp(argv[0], "play") == 0) {
			if (argc != 5) {
				errorString = "Invalid Args for 'play' command: Not equal to 5.";
			}
			else {
				int channel = str2int(argv[2]);
				int times = str2int(argv[4]);

				if (isChannelPlaying[channel] == false) {
					// If not playing, start playing.
					isChannelPlaying[channel] = true;
					if (channel == 0) {
						Mix_PlayMusic(sdlMusicPtr, times);
					}
					else {
						Mix_PlayChannel(channel, sdlChunkPtrs[channel], times);
					}
				}
				else {
					// If playing, DON'T PLAY REPEATEDLY.
					;
				}
			}
		}
		
		// Stop Command; Example:
		// "stop channel 0"
		else if (strcmp(argv[0], "stop") == 0) {
			if (argc != 3) {
				errorString = "Invalid Args for 'stop' command: Not equal to 3.";
			}
			else {
				int channel = str2int(argv[2]);

				if (isChannelPlaying[channel] == true) {
					// If playing, stop the music.
					isChannelPlaying[channel] = false;
					if (channel == 0) {
						Mix_HaltMusic();
					}
					else {
						Mix_HaltChannel(channel);
					}
				}
				else {
					// If not playing, DON'T stop in vain.
					;
				}
			}
		}

		// Pause Command; Example:
		// "pause channel 0"
		else if (strcmp(argv[0], "pause") == 0) {
			if (argc != 3) {
				errorString = "Invalid Args for 'pause' command: Not equal to 3.";
			}
			else {
				int channel = str2int(argv[2]);

				if (isChannelPlaying[channel] == true) {
					// If playing, pause the music.
					isChannelPlaying[channel] = false;
					if (channel == 0) {
						Mix_PauseMusic();
					}
					else {
						Mix_Pause(channel);
					}
				}
				else {
					// If not playing, DON'T pause in vain.
					;
				}
			}
		}

		// Resume Command; Example:
		// "resume channel 0"
		else if (strcmp(argv[0], "resume") == 0) {
			if (argc != 3) {
				errorString = "Invalid Args for 'resume' command: Not equal to 3.";
			}
			else {
				int channel = str2int(argv[2]);

				if (isChannelPlaying[channel] == false) {
					// If not playing, resume playing.
					isChannelPlaying[channel] = true;
					if (channel == 0) {
						Mix_ResumeMusic();
					}
					else {
						Mix_Resume(channel);
					}
				}
				else {
					// If playing, DON'T RESUME REPEATEDLY.
					;
				}
			}
		}
		
		// Close Command; Example:
		// "close channel 0"
		else if (strcmp(argv[0], "close") == 0) {
			if (argc != 3) {
				errorString = "Invalid Args for 'close' command: Not equal to 3.";
			}
			else {
				int channel = str2int(argv[2]);

				if (channel == 0) {
					Mix_FreeMusic(sdlMusicPtr);
					sdlMusicPtr = nullptr;
				}
				else {
					Mix_FreeChunk(sdlChunkPtrs[channel]);
					sdlChunkPtrs[channel] = nullptr;
				}
			}
		}

		// Command Not Found Senario
		else {
			errorString = "Command Not Found!";
		}

		//
		// Process Music Command: END
		//

		// Send Feedback
		if (errorString != nullptr) {
			Internal_SendString(errorString);
		}
		else {
			Internal_SendString("OK");
		}

		// Release Allocated Memory
		ArgParser_Freer(&argc, &argv);
		delete[] command;
	}
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif

// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

void MagicText_Receiver() {
	// Process Invoke Signal
	if (invokeText == 1) {

		// Process Invoke Signal
		invokeText = 0;
		invokeReceived = 1;

		// String Transfer thru Pipe.
		wchar_t* wideCommand = Internal_ReceiveStringW();

		// Parse arguments in the command.
		int argc = 0;
		wchar_t** argv = nullptr;
		ArgParserW(wideCommand, &argc, &argv);

		// Arg Finder.
		ArgFinderW argFinder = ArgFinderW(argc, argv);
		wchar_t** tmpArgv = nullptr;

		//
		// Process Text Command: START
		//

		// Error Handing.
		const char* errorString = nullptr;

		// Draw Text Command (26 Args); Example:
		// L"draw text channel 0 content "STRING" x 100 y 100 width 300 height 300 layout wordBreak textColor 255 0 0 bgColor 0 0 255 bgTransparent false"
		if (wcscmp(argv[0], L"draw") == 0 && wcscmp(argv[1], L"text") == 0) {

			// Fetch L"draw text"
			argFinder.SetFetchedFlag(0, 2);

			// Fetch L"channel 0"
			int channel;
			if (argFinder.FetchArg(L"channel", 2, &tmpArgv)) {
				channel = str2intW(tmpArgv[1]);
				textEnabled[channel] = true;
			}
			else {
				channel = 0;
				textEnabled[channel] = true;
			}

			// Fetch L"content "STRING""
			const wchar_t* content;
			if (argFinder.FetchArg(L"content", 2, &tmpArgv)) {
				content = tmpArgv[1];
			}
			else {
				content = L"";
				// We won't accept empty content string.
				errorString = "Error in 'draw text' command: Content Not Specified.";
			}
			// -----
			if (textPointers[channel] != nullptr) {
				delete[] textPointers[channel];
				textPointers[channel] = nullptr;
			}
			textPointers[channel] = new wchar_t[wcslen(content) + 1];
			wcscpy(textPointers[channel], content);

			// Fetch L"x 100"
			int x;
			if (argFinder.FetchArg(L"x", 2, &tmpArgv)) {
				x = str2intW(tmpArgv[1]);
				textX[channel] = x;
			}
			else {
				x = 0;
				textX[channel] = x;
			}

			// Fetch L"y 100"
			int y;
			if (argFinder.FetchArg(L"y", 2, &tmpArgv)) {
				y = str2intW(tmpArgv[1]);
				textY[channel] = y;
			}
			else {
				y = 0;
				textY[channel] = y;
			}

			// Fetch L"width 300"
			int width;
			if (argFinder.FetchArg(L"width", 2, &tmpArgv)) {
				width = str2intW(tmpArgv[1]);
				textWidth[channel] = width;
			}
			else {
				width = G_SCREEN_WIDTH;
				textWidth[channel] = width;
			}

			// Fetch L"height 300"
			int height;
			if (argFinder.FetchArg(L"height", 2, &tmpArgv)) {
				height = str2intW(tmpArgv[1]);
				textHeight[channel] = height;
			}
			else {
				height = G_SCREEN_HEIGHT;
				textHeight[channel] = height;
			}

			// Fetch L"layout wordbreak"
			const wchar_t* layout;
			if (argFinder.FetchArg(L"layout", 2, &tmpArgv)) {
				layout = tmpArgv[1];
			}
			else {
				layout = L"clip";
			}
			// -----
			if (wcscmp(layout, L"clip") == 0) {
				textLayout[channel] = CLIP;
			}
			else if (wcscmp(layout, L"wordBreak") == 0) {
				textLayout[channel] = WORDBREAK;
			}
			else if (wcscmp(layout, L"ellipsis") == 0) {
				textLayout[channel] = ELLIPSIS;
			}
			else if (wcscmp(layout, L"center") == 0) {
				textLayout[channel] = CENTER;
			}
			else if (wcscmp(layout, L"singleLine") == 0) {
				textLayout[channel] = SINGLE_LINE;
			}

			// Fetch L"textColor 255 0 0"
			if (argFinder.FetchArg(L"textColor", 4, &tmpArgv)) {
				int textR = str2intW(tmpArgv[1]);
				int textG = str2intW(tmpArgv[2]);
				int textB = str2intW(tmpArgv[3]);

				textColorR[channel] = textR;
				textColorG[channel] = textG;
				textColorB[channel] = textB;
			}
			else {
				textColorR[channel] = 255;
				textColorG[channel] = 255;
				textColorB[channel] = 255;
			}

			// Fetch L"bgColor 0 0 255"
			if (argFinder.FetchArg(L"bgColor", 4, &tmpArgv)) {
				int bgR = str2intW(tmpArgv[1]);
				int bgG = str2intW(tmpArgv[2]);
				int bgB = str2intW(tmpArgv[3]);

				bgColorR[channel] = bgR;
				bgColorG[channel] = bgG;
				bgColorB[channel] = bgB;
			}
			else {
				bgColorR[channel] = 0;
				bgColorG[channel] = 0;
				bgColorB[channel] = 0;
			}

			// L"bgTransparent false"
			const wchar_t* bgTransparentWideStr;
			if (argFinder.FetchArg(L"bgTransparent", 2, &tmpArgv)) {
				bgTransparentWideStr = tmpArgv[1];
			}
			else {
				bgTransparentWideStr = L"false";
			}
			// -----
			if (wcscmp(bgTransparentWideStr, L"false") == 0) {
				bgTransparent[channel] = false;
			}
			else {
				bgTransparent[channel] = true;
			}
		}

		// Delete Text Command (4 Args); Example:
		// L"delete text channel 0"
		else if (wcscmp(argv[0], L"delete") == 0 && wcscmp(argv[1], L"text") == 0) {
			
			// Fetch L"delete text"
			argFinder.SetFetchedFlag(0, 2);

			// Fetch L"channel 0"
			int channel;
			if (argFinder.FetchArg(L"channel", 2, &tmpArgv)) {
				channel = str2intW(argv[3]);
				textEnabled[channel] = false;
			}
			else {
				channel = 0;
				textEnabled[channel] = false;
			}
		}

		// Set Font Command (14 Args); Example:
		// L"set font channel 0 fontSize 20 fontFamily "ºÚÌå" fontItalic false fontBold false fontUnderline false"
		else if (wcscmp(argv[0], L"set") == 0 && wcscmp(argv[1], L"font") == 0) {

			// Fetch L"set font"
			argFinder.SetFetchedFlag(0, 2);

			// Fetch L"channel 0"
			if (argFinder.FetchArg(L"channel", 2, &tmpArgv)) {
				invokeFontChannel = str2intW(tmpArgv[1]);
			}
			else {
				invokeFontChannel = 0;
			}

			// Fetch L"fontSize 20"
			if (argFinder.FetchArg(L"fontSize", 2, &tmpArgv)) {
				invokeFontSize = str2intW(tmpArgv[1]);
			}
			else {
				invokeFontSize = 20;
			}

			// Fetch L"fontFamily \"ºÚÌå\""
			const wchar_t* fontFamily;
			if (argFinder.FetchArg(L"fontFamily", 2, &tmpArgv)) {
				if (invokeFontFamily != nullptr) {
					delete[] invokeFontFamily;
					invokeFontFamily = nullptr;
				}
				fontFamily = tmpArgv[1];
				invokeFontFamily = new wchar_t[wcslen(fontFamily) + 1];
				wcscpy(invokeFontFamily, fontFamily);
			}
			else {
				if (invokeFontFamily != nullptr) {
					delete[] invokeFontFamily;
					invokeFontFamily = nullptr;
				}
				fontFamily = L"ºÚÌå";
				invokeFontFamily = new wchar_t[wcslen(fontFamily) + 1];
				wcscpy(invokeFontFamily, fontFamily);
			}

			// Fetch L"fontItalic false"
			const wchar_t* fontItalic;
			if (argFinder.FetchArg(L"fontItalic", 2, &tmpArgv)) {
				fontItalic = tmpArgv[1];
				if (wcscmp(fontItalic, L"false") == 0) {
					invokeFontItalic = false;
				}
				else {
					invokeFontItalic = true;
				}
			}
			else {
				invokeFontItalic = false;
			}

			// Fetch L"fontBold false"
			const wchar_t* fontBold;
			if (argFinder.FetchArg(L"fontBold", 2, &tmpArgv)) {
				fontBold = tmpArgv[1];
				if (wcscmp(fontBold, L"false") == 0) {
					invokeFontBold = false;
				}
				else {
					invokeFontBold = true;
				}
			}
			else {
				invokeFontBold = false;
			}

			// Fetch L"fontUnderline false"
			const wchar_t* fontUnderline;
			if (argFinder.FetchArg(L"fontUnderline", 2, &tmpArgv)) {
				fontUnderline = tmpArgv[1];
				if (wcscmp(fontUnderline, L"false") == 0) {
					invokeFontUnderline = false;
				}
				else {
					invokeFontUnderline = true;
				}
			}
			else {
				invokeFontUnderline = false;
			}

			// Send Invokation Signal
			invokeInternalFontCreator = 1;
		}

		// Restore Font Command (4 Args); Example:
		// L"restore font channel 0"
		else if (wcscmp(argv[0], L"restore") == 0 && wcscmp(argv[1], L"font") == 0) {
			
			// Fetch L"restore font"
			argFinder.SetFetchedFlag(0, 2);

			// Fetch L"channel 0"
			if (argFinder.FetchArg(L"channel", 2, &tmpArgv)) {
				invokeFontChannel = str2intW(tmpArgv[1]);
			}
			else {
				invokeFontChannel = 0;
			}

			// Send Invokation Signal
			invokeInternalFontDeletor = 1;
		}

		// Command Not Found Senario
		else {
			errorString = "Command Not Found!";
		}

		//
		// Process Text Command: END
		//

		// Send Feedback
		if (errorString != nullptr) {
			Internal_SendString(errorString);
		}
		else {
			Internal_SendString("OK");
		}

		// Release Allocated Memory
		ArgParserW_Freer(&argc, &argv);
		delete[] wideCommand;
	}
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif


/**
 * @section
 * Main Functions, Setup() & Update()
 */


bool firstTimeSetup = true;

void Setup(HWND& hwnd, bool* wannaUpdate) {

	if (firstTimeSetup) {
		// Initialize the time counters
		thisTime = lastTime = MicroClock();

		// Initialize SDL Audio Interface
		Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);

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
	DebuggerLog("Open Shared Memory Failed, Retry...\n");

	if (G_hMapFile != NULL) {
		// @@@ Map Up the Shared Memory
		G_pBuf = (LPTSTR)MapViewOfFile(
			G_hMapFile,               // HANDLE of Shared Memory
			FILE_MAP_ALL_ACCESS,    // READ & WRITE Permission
			0,                      // Mapping Offset
			0,                      // Mapping Offset
			PIPE_LENGTH             // Mapping SIZE, (SIGN_LENGTH+2*PAGE_LENGTH)
		);
		DebuggerLog("Map Shared Memory Failed, Retry...\n");

		if (G_pBuf != NULL) {
			// Connection Success, Output Message.
			DebuggerLog("Connection Success!\n");

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

bool firstTimeUpdate = true;
bool isShowEverCalled = false;

// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

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
		invokeReceived = (unsigned char)1;
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

	// String Interface thru Pipe.
	MagicMusic_Receiver();
	MagicText_Receiver();

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

	// Process Invoke Signal (Buffer Swap)
	if (invokeBufSwap == 1) {
		// Swap Buffers
		isShowEverCalled = true;
		G_bufferDelta = G_bufferDelta == SIGN_LENGTH ? PAGE_LENGTH + SIGN_LENGTH : SIGN_LENGTH;

		// Process Invoke Signal
		invokeBufSwap = 0;
		invokeReceived = 1;
	}

	// Count & Lock FPS
	thisTime = MicroClock();
	fpsCalculator.Count(thisTime - lastTime);
	fpsLocker.Lock(thisTime - lastTime);
	lastTime = thisTime;
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif

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
