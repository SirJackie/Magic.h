#ifndef __Magic_H__
#define __Magic_H__

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>


/*
** Compiler Specific
*/


// MSVC Compiler WChar Feature Compatibility.
#if defined(_MSC_VER)
#define PIPE_NAME TEXT("MagicDotHBuffer")
#else
#define PIPE_NAME ("MagicDotHBuffer")
#endif


/*
** Fast Inline Functions
*/


#define clamp(minVal, value, maxVal) (((value) < (minVal))? (minVal) : (((value) > (maxVal-1))? (maxVal-1) : (value)))


/*
** Definitions: Buffer
*/


HANDLE  hMapFile;
LPCTSTR pBuf;
char* pixels;


/*
** Definitions: Buffer Swapping
*/


#define PAGE_LENGTH 1440000  // (800 x 600 x 3) bytes
#define SIGN_LENGTH 8192
#define PIPE_LENGTH (SIGN_LENGTH+2*PAGE_LENGTH)
int bufferDelta = SIGN_LENGTH;


/*
** Definitions: Pipe Signal Transfer
*/


#define exitSignal   (((char*)pBuf)[ 0])  // [ 0]
#define swapSignal   (((char*)pBuf)[ 1])  // [ 1]
#define gotitSignal  (((char*)pBuf)[ 2])  // [ 2]
#define fpsLockRate  (((int* )pBuf)[ 3])  // [ 3] [ 4] [ 5] [ 6]
#define isWinFocus   (((char*)pBuf)[ 7])  // [ 7]

#define mouseX       (((int* )pBuf)[ 8])  // [ 8] [ 9] [10] [11]
#define mouseY       (((int* )pBuf)[12])  // [12] [13] [14] [15]
#define isLeftClick  (((char*)pBuf)[16])  // [16]
#define isRightClick (((char*)pBuf)[17])  // [17]

#define keyboard     (((char*)pBuf) +18)  // [18] [19] ... [273]


/*
** Definitions: Bitmap Loading
*/


// Disable Memory Alignment for Struct
#pragma pack(push, 1)

typedef struct {
	char     type[2];
	unsigned int  size;
	unsigned short int  reserved1, reserved2;
	unsigned int  offset;
} BMPHeader;

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


/*
** Functions: Bitmap Loading
*/


void Magic(int fps = 60){

	/*
	** --------------------------------------------------
	** Initialize Shared Memory
	** --------------------------------------------------
	*/

	// Create Shared Memory
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,   // Use the system paging file
		NULL,                   // Default Security
		PAGE_READWRITE,         // Read and write permissions
		0,                      // Max Obj Size's HIGHER 32 Bits
		PIPE_LENGTH,            // Max Obj Size's LOWER 32 Bits, aka PIPE SIZE, (SIGN_LENGTH+2*PAGE_LENGTH)
		PIPE_NAME               // NAME of Shared Memory
	);
	if (hMapFile == NULL) {
		GetLastError();  // Error Code.
	}

	// Map Up the Shared Memory
	pBuf = (LPTSTR) MapViewOfFile(
		hMapFile,               // HANDLE of Shared Memory
		FILE_MAP_ALL_ACCESS,    // Write Permission
		0,                      // Mapping Offset
		0,                      // Mapping Offset
		PIPE_LENGTH             // Mapping SIZE, (SIGN_LENGTH+2*PAGE_LENGTH)
	);

	if (pBuf == NULL) {
		GetLastError();  // Error Code.
		CloseHandle(hMapFile);
	}

	// Initialize Pixels Buffer Pointer
	pixels = ((char*)pBuf + bufferDelta);

	/*
	** --------------------------------------------------
	** Initialize Pipe Signals
	** --------------------------------------------------
	*/

	exitSignal  = (unsigned char) 0;
	swapSignal  = (unsigned char) 0;
	gotitSignal = (unsigned char) 0;
	fpsLockRate = (int) fps;

	/*
	** --------------------------------------------------
	** Initialize MagicHost.exe
	** --------------------------------------------------
	*/

	system("start MagicHost.exe");
}


void Quit(){
	/*
	** --------------------------------------------------
	** Finalize Shared Memory
	** --------------------------------------------------
	*/

	// Sending Exit Signal
	exitSignal = (unsigned char) 1;
	while(gotitSignal != 1);  // Wait Until Exiting Finished.
	gotitSignal = (unsigned char) 0;

	// Delete Shared Memory
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}

// DISABLE MSVC OPEIMIZATION: START
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif

void Show(){
	// Swapping Buffers
	bufferDelta = bufferDelta == SIGN_LENGTH ? PAGE_LENGTH + SIGN_LENGTH : SIGN_LENGTH;
	swapSignal = (unsigned char) 1;

	// You MUST DISABLE MSVC OPEIMIZATION in Order to Make the Following Line Work.
	while(gotitSignal != 1);  // Wait Until Pushing Finished.
	
	gotitSignal = (unsigned char) 0;

	// Update Pixels Buffer Pointer
	pixels = ((char*)pBuf + bufferDelta);
}

// DISABLE MSVC OPEIMIZATION: END
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif


/*
** Functions: Bitmap Loading
*/


void LoadBMP(const char* filename, int* width, int* height, unsigned char** image_data_ptr){
	FILE *file;
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
		return;
	}

	// Read File Header
	fread(&header, sizeof(header), 1, file);

	// Read Info Header
	fread(&info_header, sizeof(info_header), 1, file);

	// Allocate Memory for Image Storage
	(*image_data_ptr) = (unsigned char *)malloc(info_header.image_size);
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

	*width = info_header.width;
	*height = info_header.height;
}


/*
** Class: Picture Class
*/


class Picture{
public:
	int width, height;
	unsigned char* pixels;

	void Load(const char* filename){
		LoadBMP(filename, &width, &height, &pixels);
	}
		
	void Draw(int x_, int y_){

		// Avoid User to Draw Outside the Border
		x_ = clamp(0, x_, 800 - this->width + 1);
		y_ = clamp(0, y_, 600 - this->height + 1);

		// Fast Fillings
		for (int y = 0; y < this->height; y++) {
			for (int x = 0; x < this->width; x++) {
				int picture_y = this->height - y - 1;  // Picture is Y-Axis Reversed.
				::pixels[(( (y + y_) * 800) + (x + x_) ) * 3 + 0] = this->pixels[((picture_y * this->width) + x) * 3 + 0];
				::pixels[(( (y + y_) * 800) + (x + x_) ) * 3 + 1] = this->pixels[((picture_y * this->width) + x) * 3 + 1];
				::pixels[(( (y + y_) * 800) + (x + x_) ) * 3 + 2] = this->pixels[((picture_y * this->width) + x) * 3 + 2];
			}
		}
	}

	inline void SetPixel(int x, int y, char r, char g, char b) {
		x = clamp(0, x, this->width);
		y = clamp(0, y, this->height);
		y = this->height - y - 1;  // Picture is Y-Axis Reversed.

		this->pixels[((y * this->width) + x) * 3 + 0] = b;  // B
		this->pixels[((y * this->width) + x) * 3 + 1] = g;  // G
		this->pixels[((y * this->width) + x) * 3 + 2] = r;  // R
	}


	inline void GetPixel(int x, int y, char* r, char* g, char* b) {
		x = clamp(0, x, this->width);
		y = clamp(0, y, this->height);
		y = this->height - y - 1;  // Picture is Y-Axis Reversed.

		*b = this->pixels[((y * this->width) + x) * 3 + 0];  // B
		*g = this->pixels[((y * this->width) + x) * 3 + 1];  // G
		*r = this->pixels[((y * this->width) + x) * 3 + 2];  // R
	}
};


/*
** Function: Drawings
*/


void Fill(int x0, int y0, int x1, int y1, char r, char g, char b){
	for (int y = y0; y < y1; y++){
		for (int x = x0; x < x1; x++){
			pixels[((y * 800) + x) * 3 + 0] = b;  // B
			pixels[((y * 800) + x) * 3 + 1] = g;  // G
			pixels[((y * 800) + x) * 3 + 2] = r;  // R
		}
	}
}

void Clean(char r, char g, char b){
	Fill(0, 0, 800, 600, r, g, b);
}


inline void MagicSetPixel(int x, int y, char r, char g, char b) {
	x = clamp(0, x, 800);
	y = clamp(0, y, 600);

	pixels[((y * 800) + x) * 3 + 0] = b;  // B
	pixels[((y * 800) + x) * 3 + 1] = g;  // G
	pixels[((y * 800) + x) * 3 + 2] = r;  // R
}


inline void MagicGetPixel(int x, int y, char* r, char* g, char* b) {
	x = clamp(0, x, 800);
	y = clamp(0, y, 600);

	*b = pixels[((y * 800) + x) * 3 + 0];  // B
	*g = pixels[((y * 800) + x) * 3 + 1];  // G
	*r = pixels[((y * 800) + x) * 3 + 2];  // R
}


#endif

