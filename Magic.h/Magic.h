/*
 * The Magic Graphical Interface
 * Author: SirJackie
 * Project Repository: https://github.com/SirJackie/Magic.h
 * Project Repository: https://gitee.com/SirJackie/Magic.h
 *
 * Description:
 * A simple and elegant graphics programming framework that allows you
 * to create a window with just one line of code.
 *
 * License:
 * This software is licensed under the Mulan PSL v2 License.
 * You may use, copy, modify, and distribute this software
 * for any purpose with or without fee, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#ifndef __Magic_H__
#define __Magic_H__

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <wchar.h>


/**
 * @section
 * Struct Definitions
 */


// Disable Memory Alignment for Struct
#pragma pack(push, 1)

/**
 * @brief: struct BMPHeader for internal bitmap loading purposes.
 * @SHOULD NOT BE CALLED BY USERS!
 */

typedef struct {
	char     type[2];
	unsigned int  size;
	unsigned short int  reserved1, reserved2;
	unsigned int  offset;
} BMPHeader;

/**
 * @brief: struct BMPInfoHeader for internal bitmap loading purposes.
 * @SHOULD NOT BE CALLED BY USERS!
 */

typedef struct {
	unsigned int  size;
	unsigned int  width, height;
	unsigned short int  planes;
	unsigned short int  bit_count;
	unsigned int  compression;
	unsigned int  image_size;
	unsigned int  x_pixels_per_meter;
	unsigned int  y_pixels_per_meter;
	unsigned int  colors_used;
	unsigned int  colors_important;
} BMPInfoHeader;

#pragma pack(pop)


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
int G_bufferDelta = SIGN_LENGTH;  // Start from the first buffer.


/**
 * @section
 * Internal Functions, SHOULD NOT BE CALLED BY USERS!
 */


/**
 * @brief: Make sure min <= value < max. (including left but not including right)
 * @param minVal: Minimum value for clampping.
 * @param value: The value to clamp.
 * @param maxVal: Maximum value for clampping.
 * @return _T: The result of clampped value.
 */

#define clamp(minVal, value, maxVal) (((value) < (minVal))? (minVal) : (((value) > (maxVal-1))? (maxVal-1) : (value)))

/**
 * @brief: Make sure min <= value <= max. (including left AND right)
 * @param minVal: Minimum value for clampping.
 * @param value: The value to clamp.
 * @param maxVal: Maximum value for clampping.
 * @return _T: The result of clampped value.
 */

#define clampF(minVal, value, maxVal) (((value) < (minVal))? (minVal) : (((value) > (maxVal))? (maxVal) : (value)))

/**
 * @brief: Internal bitmap loading function, not encapsulated by the Picture class.
 * @SHOULD NOT BE CALLED BY USERS!
 * @param *filename: a C-String describing the filename of the bitmap.
 * @param *width: Output parameter, outputting the width of the loaded bitmap.
 * @param *height: Output parameter, outputting the height of the loaded bitmap.
 * @param *pitch: Output parameter, outputting the pitch of the loaded bitmap.
 *                Pitch refers to the offset when advecting through each row.
 * @param **image_data_ptr: Output parameter, outputting the loaded pixel results.
 * @return void
 */

void LoadBMP(const char* filename, int* width, int* height, int* pitch, unsigned char** image_data_ptr) {
	FILE* file;
	BMPHeader header;
	BMPInfoHeader info_header;

	// Load Bitmap File
#if defined(_MSC_VER)
	fopen_s(&file, filename, "rb");
#else
	file = fopen(filename, "rb");
#endif

	if (!file) {
		perror("Error opening file");
		printf("%s\n", filename);
		return;
	}

	// Read File Header
	fread(&header, sizeof(header), 1, file);

	// Read Info Header
	fread(&info_header, sizeof(info_header), 1, file);

	// Check If BMP is a 24Bit Bitmap.
	if (info_header.bit_count != 24) {
		printf("BMP Loader Error: Unsupported Bit Count: %d\n", info_header.bit_count);
		printf("BMP Loader Only Supports 24Bit Bitmaps.\n");
	}

	// Sometimes BMP Encoder Will Save a info_header.image_size = 0, which sucks.
	if (info_header.image_size == 0) {
		// Manual Correction Required.
		info_header.image_size = header.size - header.offset;
	}

	// Allocate Memory for Image Storage
	(*image_data_ptr) = (unsigned char*)malloc(info_header.image_size);
	if (!(*image_data_ptr)) {
		perror("Error allocating memory");
		fclose(file);
		return;
	}

	// Move File Ptr to the Start of the Image
	fseek(file, header.offset, SEEK_SET);

	// Read Image Pixels
	fread((*image_data_ptr), info_header.image_size, 1, file);

	// Close File
	fclose(file);

	// Get Width & Height
	*width = info_header.width;
	*height = info_header.height;

	// Get Pitch (Pitch >= Width * 3)
	*pitch = info_header.image_size / info_header.height;
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

/**
 * @brief: Internal string sending function.
 * @SHOULD NOT BE CALLED BY USERS!
 * @param *str: The string to send.
 * @return void
 */

void Internal_SendString(const char* str) {

	// Save the length of the string to pipe.
	int length = strlen(str) + 1;
	stringLen = length;

	// Invoke String Transfer
	while (invokeTransfer != 0 || invokeReceived != 0);
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
		while (invokeSendBtch != 0 || invokeReceived != 0);
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

/**
 * @brief: Internal wide string sending function.
 * @SHOULD NOT BE CALLED BY USERS!
 * @param *str: The wide string to send.
 * @return void
 */

void Internal_SendStringW(const wchar_t* wideStr) {

	// Save the length of the string to pipe.
	int length = wcslen(wideStr) + 1;
	stringLen = length;

	// Invoke String Transfer
	while (invokeTransfer != 0 || invokeReceived != 0);
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
		while (invokeSendBtch != 0 || invokeReceived != 0);
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

/**
 * @brief: Internal string receiving function.
 * @SHOULD NOT BE CALLED BY USERS!
 * @param *str: The string to receive.
 * @return void
 */

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

/**
 * @brief: Internal wide string receiving function.
 * @SHOULD NOT BE CALLED BY USERS!
 * @param *str: The wide string to receive.
 * @return void
 */

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


// --------------------------------------------------
// --------------------------------------------------
// --------------------------------------------------
// --------------------------------------------------
// --------------------------------------------------


/**
 * @section
 * User API Functions
 */


/**
 * @brief: Initialize the Magic Framework, which creates a window to draw.
 * @param fps=60: The locked framerate of FPS, defaultly set to 60, can be modified.
 * @return void
 */

void Magic(int fps = 60){

	//
	// Initialize Shared Memory
	//

	// Create Shared Memory
	G_hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,   // Use the system paging file
		NULL,                   // Default Security
		PAGE_READWRITE,         // Read and write permissions
		0,                      // Max Obj Size's HIGHER 32 Bits
		PIPE_LENGTH,            // Max Obj Size's LOWER 32 Bits, aka PIPE SIZE, (SIGN_LENGTH+2*PAGE_LENGTH)
		G_PIPE_NAME             // NAME of Shared Memory
	);
	if (G_hMapFile == NULL) {
		GetLastError();  // Error Code.
	}

	// Map Up the Shared Memory
	G_pBuf = (LPTSTR) MapViewOfFile(
		G_hMapFile,             // HANDLE of Shared Memory
		FILE_MAP_ALL_ACCESS,    // Write Permission
		0,                      // Mapping Offset
		0,                      // Mapping Offset
		PIPE_LENGTH             // Mapping SIZE, (SIGN_LENGTH+2*PAGE_LENGTH)
	);

	if (G_pBuf == NULL) {
		GetLastError();  // Error Code.
		CloseHandle(G_hMapFile);
	}

	// Initialize Pixels Buffer Pointer
	G_pixels = ((unsigned char*)G_pBuf + G_bufferDelta);

	//
	// Initialize Pipe Signals
	//

	// Initialize Process Pipe to ALL 0.
	for (char* ptr = (char*)G_pBuf; ptr < ((char*)G_pBuf) + PIPE_LENGTH; ptr++) {
		*ptr = 0;
	}

	// Initialize Pipe Signals
	exitSignal  = (unsigned char) 0;
	invokeBufSwap  = (unsigned char) 0;
	invokeReceived = (unsigned char) 0;
	fpsLockRate = (int) fps;

	//
	// Initialize MagicHost.exe
	//

	system("start MagicHost.exe");
}

/**
 * @brief: Finalize the Magic Framework, which turns off the window.
 * @return void
 */

void Quit(){

	//
	// Finalize Shared Memory
	//

	// Sending Exit Signal
	exitSignal = (unsigned char) 1;
	while(invokeReceived != 1);  // Wait Until Exiting Finished.
	invokeReceived = (unsigned char) 0;

	// Delete Shared Memory
	UnmapViewOfFile(G_pBuf);
	CloseHandle(G_hMapFile);
}

// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

/**
 * @brief: Push the Back Buffer to the Front Buffer, which shows newly drawn pixels.
 * @return void
 */

void Show(){
	// Swapping Buffers: Client Side
	G_bufferDelta = G_bufferDelta == SIGN_LENGTH ? PAGE_LENGTH + SIGN_LENGTH : SIGN_LENGTH;
	G_pixels = ((unsigned char*)G_pBuf + G_bufferDelta);

	// Invoke Buffer Swap: Host Side
	while (invokeBufSwap != 0 || invokeReceived != 0);
	invokeReceived = 0;
	invokeBufSwap = 1;
	while (invokeReceived == 0);  // Wait for Response
	invokeReceived = 0;
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif

/**
 * @brief: Fill a specific rectangle with a color.
 * @param x0: The x coordinate of the up-left corner of the rectangle.
 * @param y0: The y coordinate of the up-left corner of the rectangle.
 * @param x1: The x coordinate of the down-right corner of the rectangle.
 * @param y1: The y coordinate of the down-right corner of the rectangle.
 * @param r: The red channel of the filling color.
 * @param g: The green channel of the filling color.
 * @param b: The blue channel of the filling color.
 * @return void
 */

void Fill(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b){
	for (int y = y0; y < y1; y++){
		for (int x = x0; x < x1; x++){
			G_pixels[((y * 800) + x) * 3 + 0] = b;  // B
			G_pixels[((y * 800) + x) * 3 + 1] = g;  // G
			G_pixels[((y * 800) + x) * 3 + 2] = r;  // R
		}
	}
}

/**
 * @brief: Clean the whole screen with a specific color.
 * @param r: The red channel of the filling color.
 * @param g: The green channel of the filling color.
 * @param b: The blue channel of the filling color.
 * @return void
 */

void Clean(unsigned char r, unsigned char g, unsigned char b){
	Fill(0, 0, 800, 600, r, g, b);
}

/**
 * @brief: Set the color of a specific pixel.
 * @param x: The x coordinate of the specific pixel.
 * @param y: The y coordinate of the specific pixel.
 * @param r: The red channel of the pixel color.
 * @param g: The green channel of the pixel color.
 * @param b: The blue channel of the pixel color.
 * @return void
 */

inline void MagicSetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	if (x < 0) return;
	if (y < 0) return;
	if (x >= 800) return;
	if (y >= 600) return;

	G_pixels[((y * 800) + x) * 3 + 0] = b;  // B
	G_pixels[((y * 800) + x) * 3 + 1] = g;  // G
	G_pixels[((y * 800) + x) * 3 + 2] = r;  // R
}

/**
 * @brief: Get the color of a specific pixel.
 * @param x: The x coordinate of the specific pixel.
 * @param y: The y coordinate of the specific pixel.
 * @param *r: Output parameter, returns the red channel of the pixel color.
 * @param *g: Output parameter, returns the green channel of the pixel color.
 * @param *b: Output parameter, returns the blue channel of the pixel color.
 * @return void
 */

inline void MagicGetPixel(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b) {
	x = clamp(0, x, 800);
	y = clamp(0, y, 600);

	*b = G_pixels[((y * 800) + x) * 3 + 0];  // B
	*g = G_pixels[((y * 800) + x) * 3 + 1];  // G
	*r = G_pixels[((y * 800) + x) * 3 + 2];  // R
}

/**
 * @brief: Get the red channel of the color of a specific pixel.
 * @param x: The x coordinate of the specific pixel.
 * @param y: The y coordinate of the specific pixel.
 * @return unsigned char: The red channel of the pixel color.
 */

inline unsigned char MagicGetR(int x, int y) {
	x = clamp(0, x, 800);
	y = clamp(0, y, 600);

	return G_pixels[((y * 800) + x) * 3 + 2];  // R
}

/**
 * @brief: Get the green channel of the color of a specific pixel.
 * @param x: The x coordinate of the specific pixel.
 * @param y: The y coordinate of the specific pixel.
 * @return unsigned char: The green channel of the pixel color.
 */

inline unsigned char MagicGetG(int x, int y) {
	x = clamp(0, x, 800);
	y = clamp(0, y, 600);

	return G_pixels[((y * 800) + x) * 3 + 1];  // G
}

/**
 * @brief: Get the blue channel of the color of a specific pixel.
 * @param x: The x coordinate of the specific pixel.
 * @param y: The y coordinate of the specific pixel.
 * @return unsigned char: The blue channel of the pixel color.
 */

inline unsigned char MagicGetB(int x, int y) {
	x = clamp(0, x, 800);
	y = clamp(0, y, 600);

	return G_pixels[((y * 800) + x) * 3 + 0];  // B
}

// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

/**
 * @brief: The Audio Interface provided by Magic.h
 * @param command: The command to send.
 *                 For details, please view the API Docs.
 * @return void
 */

void MagicMusic(const char* command) {

	// Invoke Music Interface
	while (invokeMusic != 0 || invokeReceived != 0);
	invokeReceived = 0;
	invokeMusic = 1;
	while (invokeReceived == 0);  // Wait for Response
	invokeReceived = 0;

	// Transfer the Commands
	Internal_SendString(command);

	// Receive Feedback
	char* feedback = Internal_ReceiveString();
	if (strcmp(feedback, "OK") != 0) {
		// Error
		printf("MagicMusic() Error: %s\n", feedback);
	}
	delete[] feedback;
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif

// DISABLE MSVC OPTIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

/**
 * @brief: The Text Interface provided by Magic.h
 * @param command: The command to send.
 *                 For details, please view the API Docs.
 * @return void
 */

void MagicText(const wchar_t* wideCommand) {

	// Invoke Music Interface
	while (invokeText != 0 || invokeReceived != 0);
	invokeReceived = 0;
	invokeText = 1;
	while (invokeReceived == 0);  // Wait for Response
	invokeReceived = 0;

	// Transfer the Commands
	Internal_SendStringW(wideCommand);

	// Receive Feedback
	char* feedback = Internal_ReceiveString();
	if (strcmp(feedback, "OK") != 0) {
		// Error
		printf("MagicText() Error: %s\n", feedback);
	}
	delete[] feedback;
}

// DISABLE MSVC OPTIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif


/**
 * @section
 * Picture Loading Class
 */


class Picture{
public:
	int width, height, pitch;
	unsigned char* pixels;

	~Picture() {
		free((void*)pixels);
		pixels = nullptr;
	}

	/**
	 * @brief: Load picture pixels from a file.
	 * @param filename: The filename of the file.
	 * @return void
	 */

	void Load(const char* filename){
		LoadBMP(filename, &width, &height, &pitch, &pixels);
	}

	/**
	 * @brief: Draw this picture in the position (x, y) on the screen.
	 *         Support boundary clipping, can safely drawn on any position.
	 *         Support transparent filtering (filter: (255, 0, 255) )
	 * @param x_: The x coordinate of the drawing position.
	 * @param y_: The y coordinate of the drawing position.
	 * @return void
	 */

	void Draw(int x_, int y_) {

		int screenStartX = x_;
		int screenStartY = y_;
		int screenEndX = x_ + this->width;
		int screenEndY = y_ + this->height;

		screenStartX = clamp(0, screenStartX, G_SCREEN_WIDTH);
		screenStartY = clamp(0, screenStartY, G_SCREEN_HEIGHT);
		screenEndX = clamp(0, screenEndX, G_SCREEN_WIDTH);
		screenEndY = clamp(0, screenEndY, G_SCREEN_HEIGHT);

		unsigned char ri, gi, bi;

		for (int y = screenStartY; y < screenEndY; y++) {
			for (int x = screenStartX; x < screenEndX; x++) {

				ri = this->GetR(x - x_, y - y_);
				gi = this->GetG(x - x_, y - y_);
				bi = this->GetB(x - x_, y - y_);

				if (ri == 255 && gi == 0 && bi == 255) {
					// Pink, Transparent, Do Not Draw.
					continue;
				}

				MagicSetPixel(x, y, ri, gi, bi);
			}
		}
	}

	/**
	 * @brief: Draw this picture in the position (x, y) on the screen,
	 *         With a zooming ratio in range [0.0f, 1.0f]
	 *         to control how big the image should be drawn.
	 * @note: The zooming center: Left-Up Corner of this picture.
	 * @param x_: The x coordinate of the drawing position.
	 * @param y_: The y coordinate of the drawing position.
	 * @param ratio: The zooming ratio, in range [0.0f, 1.0f].
	 * @return void
	 */

	void DrawZoom(int x_, int y_, float ratio) {

		int screenStartX = x_;
		int screenStartY = y_;
		int screenEndX = x_ + int(this->width * ratio);
		int screenEndY = y_ + int(this->height * ratio);

		screenStartX = clamp(0, screenStartX, G_SCREEN_WIDTH);
		screenStartY = clamp(0, screenStartY, G_SCREEN_HEIGHT);
		screenEndX = clamp(0, screenEndX, G_SCREEN_WIDTH);
		screenEndY = clamp(0, screenEndY, G_SCREEN_HEIGHT);

		unsigned char ri, gi, bi;

		for (int y = screenStartY; y < screenEndY; y++) {
			for (int x = screenStartX; x < screenEndX; x++) {

				ri = this->GetR(int((x - x_) / ratio), int((y - y_) / ratio));
				gi = this->GetG(int((x - x_) / ratio), int((y - y_) / ratio));
				bi = this->GetB(int((x - x_) / ratio), int((y - y_) / ratio));

				if (ri == 255 && gi == 0 && bi == 255) {
					// Pink, Transparent, Do Not Draw.
					continue;
				}

				MagicSetPixel(x, y, ri, gi, bi);
			}
		}
	}

	/**
	 * @brief: Draw this picture in the position (x, y) on the screen,
	 *         With a zooming ratio in range [0.0f, 1.0f]
	 *         to control how big the image should be drawn.
	 * @note: The zooming center: The CENTRIC of this picture.
	 * @param x_: The x coordinate of the drawing position.
	 * @param y_: The y coordinate of the drawing position.
	 * @param ratio: The zooming ratio, in range [0.0f, 1.0f].
	 * @return void
	 */

	void DrawZoomCentered(int x_, int y_, float ratio) {

		int leftPadding = int(this->width * (1.0f - ratio) * 0.5f);
		int topPadding = int(this->height * (1.0f - ratio) * 0.5f);

		this->DrawZoom(x_ + leftPadding, y_ + topPadding, ratio);
	}

	/**
	 * @brief: Draw this picture in the position (x, y) on the screen,
	 *         With a brightness value in range [0.0f, Infinity]
	 *         to control how bright the image should be drawn.
	 * @param x_: The x coordinate of the drawing position.
	 * @param y_: The y coordinate of the drawing position.
	 * @param brightness: The brightness value, in range [0.0f, Infinity].
	 * @return void
	 */

	void DrawBrightness(int x_, int y_, float brightness) {

		int screenStartX = x_;
		int screenStartY = y_;
		int screenEndX = x_ + this->width;
		int screenEndY = y_ + this->height;

		screenStartX = clamp(0, screenStartX, G_SCREEN_WIDTH);
		screenStartY = clamp(0, screenStartY, G_SCREEN_HEIGHT);
		screenEndX = clamp(0, screenEndX, G_SCREEN_WIDTH);
		screenEndY = clamp(0, screenEndY, G_SCREEN_HEIGHT);

		unsigned char ri, gi, bi;

		for (int y = screenStartY; y < screenEndY; y++) {
			for (int x = screenStartX; x < screenEndX; x++) {

				ri = this->GetR(x - x_, y - y_);
				gi = this->GetG(x - x_, y - y_);
				bi = this->GetB(x - x_, y - y_);

				if (ri == 255 && gi == 0 && bi == 255) {
					// Pink, Transparent, Do Not Draw.
					continue;
				}

				ri = int(ri * brightness);
				gi = int(gi * brightness);
				bi = int(bi * brightness);

				ri = clamp(0, ri, 256);
				gi = clamp(0, gi, 256);
				bi = clamp(0, bi, 256);

				MagicSetPixel(x, y, ri, gi, bi);
			}
		}
	}

	/**
	* @brief: Draw this picture in the position (x, y) on the screen,
	*         With 4 clipping values in range [0.0f, 100.0f]
	*         to control how to clip the image.
	* @param x_: The x coordinate of the drawing position.
	* @param y_: The y coordinate of the drawing position.
	* @param clipStartX : The Starting Percentage for X-Axis Clipping.
	* @param clipEndX   : The Ending   Percentage for X-Axis Clipping.
	* @param clipStartY : The Starting Percentage for Y-Axis Clipping.
	* @param clipEndY   : The Ending   Percentage for Y-Axis Clipping.
	* @return void
	*/

	void DrawClip(
		int x_, int y_,
		float clipStartX, float clipEndX,
		float clipStartY, float clipEndY
	) {

		clipStartX = clampF(0.0f, clipStartX, 100.0f);
		clipStartY = clampF(0.0f, clipStartY, 100.0f);
		clipEndX = clampF(0.0f, clipEndX, 100.0f);
		clipEndY = clampF(0.0f, clipEndY, 100.0f);

		int clipStartXPixel = int(clipStartX * this->width / 100.0f);
		int clipStartYPixel = int(clipStartY * this->height / 100.0f);
		int clipEndXPixel = int(clipEndX * this->width / 100.0f);
		int clipEndYPixel = int(clipEndY * this->height / 100.0f);

		int screenStartX = x_ + clipStartXPixel;
		int screenStartY = y_ + clipStartYPixel;
		int screenEndX = x_ + clipEndXPixel;
		int screenEndY = y_ + clipEndYPixel;

		screenStartX = clamp(0, screenStartX, G_SCREEN_WIDTH);
		screenStartY = clamp(0, screenStartY, G_SCREEN_HEIGHT);
		screenEndX = clamp(0, screenEndX, G_SCREEN_WIDTH);
		screenEndY = clamp(0, screenEndY, G_SCREEN_HEIGHT);

		unsigned char ri, gi, bi;

		for (int y = screenStartY; y < screenEndY; y++) {
			for (int x = screenStartX; x < screenEndX; x++) {

				ri = this->GetR(x - x_, y - y_);
				gi = this->GetG(x - x_, y - y_);
				bi = this->GetB(x - x_, y - y_);

				if (ri == 255 && gi == 0 && bi == 255) {
					// Pink, Transparent, Do Not Draw.
					continue;
				}

				MagicSetPixel(x, y, ri, gi, bi);
			}
		}
	}

	/**
	 * @brief: Draw this picture in the position (x, y) on the screen,
	 *         With 2 flipping boolean values either true or false
	 *         to control whether X/Y Axis should be flipped.
	 * @param x_: The x coordinate of the drawing position.
	 * @param y_: The y coordinate of the drawing position.
	 * @param flipX: Boolean value, whether to flip X-Axis or not.
	 * @param flipY: Boolean value, whether to flip Y-Axis or not.
	 * @return void
	 */

	void DrawFlip(int x_, int y_, bool flipX, bool flipY) {

		int screenStartX = x_;
		int screenStartY = y_;
		int screenEndX = x_ + this->width;
		int screenEndY = y_ + this->height;

		screenStartX = clamp(0, screenStartX, G_SCREEN_WIDTH);
		screenStartY = clamp(0, screenStartY, G_SCREEN_HEIGHT);
		screenEndX = clamp(0, screenEndX, G_SCREEN_WIDTH);
		screenEndY = clamp(0, screenEndY, G_SCREEN_HEIGHT);

		unsigned char ri, gi, bi;
		int sourceX, sourceY;

		for (int y = screenStartY; y < screenEndY; y++) {
			for (int x = screenStartX; x < screenEndX; x++) {

				sourceX = flipX ? (this->width - (x - x_) - 1) : (x - x_);
				sourceY = flipY ? (this->height - (y - y_) - 1) : (y - y_);

				ri = this->GetR(sourceX, sourceY);
				gi = this->GetG(sourceX, sourceY);
				bi = this->GetB(sourceX, sourceY);

				if (ri == 255 && gi == 0 && bi == 255) {
					// Pink, Transparent, Do Not Draw.
					continue;
				}

				MagicSetPixel(x, y, ri, gi, bi);
			}
		}
	}

	/**
	 * @brief: Set the color of a specific pixel IN THIS PICTURE.
	 * @param x: The x coordinate of the specific pixel.
	 * @param y: The y coordinate of the specific pixel.
	 * @param r: The red channel of the pixel color.
	 * @param g: The green channel of the pixel color.
	 * @param b: The blue channel of the pixel color.
	 * @return void
	 */

	inline void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
		if (x < 0) return;
		if (y < 0) return;
		if (x >= this->width) return;
		if (y >= this->height) return;
		y = this->height - y - 1;  // Picture is Y-Axis Reversed.

		this->pixels[(y * this->pitch) + (x * 3) + 0] = b;  // B
		this->pixels[(y * this->pitch) + (x * 3) + 1] = g;  // G
		this->pixels[(y * this->pitch) + (x * 3) + 2] = r;  // R
	}

	/**
	 * @brief: Get the color of a specific pixel IN THIS PICTURE.
	 * @param x: The x coordinate of the specific pixel.
	 * @param y: The y coordinate of the specific pixel.
	 * @param *r: Output parameter, returns the red channel of the pixel color.
	 * @param *g: Output parameter, returns the green channel of the pixel color.
	 * @param *b: Output parameter, returns the blue channel of the pixel color.
	 * @return void
	 */

	inline void GetPixel(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b) {
		x = clamp(0, x, this->width);
		y = clamp(0, y, this->height);
		y = this->height - y - 1;  // Picture is Y-Axis Reversed.

		*b = this->pixels[(y * this->pitch) + (x * 3) + 0];  // B
		*g = this->pixels[(y * this->pitch) + (x * 3) + 1];  // G
		*r = this->pixels[(y * this->pitch) + (x * 3) + 2];  // R
	}

	/**
	 * @brief: Get the red channel of the color of a specific pixel IN THIS PICTURE.
	 * @param x: The x coordinate of the specific pixel.
	 * @param y: The y coordinate of the specific pixel.
	 * @return unsigned char: The red channel of the pixel color.
	 */

	inline unsigned char GetR(int x, int y) {
		x = clamp(0, x, this->width);
		y = clamp(0, y, this->height);
		y = this->height - y - 1;  // Picture is Y-Axis Reversed.

		return this->pixels[(y * this->pitch) + (x * 3) + 2];  // R
	}

	/**
	 * @brief: Get the green channel of the color of a specific pixel IN THIS PICTURE.
	 * @param x: The x coordinate of the specific pixel.
	 * @param y: The y coordinate of the specific pixel.
	 * @return unsigned char: The green channel of the pixel color.
	 */

	inline unsigned char GetG(int x, int y) {
		x = clamp(0, x, this->width);
		y = clamp(0, y, this->height);
		y = this->height - y - 1;  // Picture is Y-Axis Reversed.

		return this->pixels[(y * this->pitch) + (x * 3) + 1];  // G
	}

	/**
	 * @brief: Get the blue channel of the color of a specific pixel IN THIS PICTURE.
	 * @param x: The x coordinate of the specific pixel.
	 * @param y: The y coordinate of the specific pixel.
	 * @return unsigned char: The blue channel of the pixel color.
	 */

	inline unsigned char GetB(int x, int y) {
		x = clamp(0, x, this->width);
		y = clamp(0, y, this->height);
		y = this->height - y - 1;  // Picture is Y-Axis Reversed.

		return this->pixels[(y * this->pitch) + (x * 3) + 0];  // B
	}
};

#endif
