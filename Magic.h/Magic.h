#ifndef __Magic_H__
#define __Magic_H__

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>


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
		"MagicDotHBuffer"       // NAME of Shared Memory
	);
	if (hMapFile == NULL) {
		GetLastError();  // Error Code.
	}
	
	// Map Up the Shared Memory
	pBuf = (LPTSTR) MapViewOfFile(
		hMapFile,               // HANDLE of Shared Memory
		FILE_MAP_WRITE,         // Write Permission
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


void Show(){
	// Swapping Buffers
	bufferDelta = bufferDelta == SIGN_LENGTH ? PAGE_LENGTH + SIGN_LENGTH : SIGN_LENGTH;
	swapSignal = (unsigned char) 1;
	while(gotitSignal != 1);  // Wait Until Pushing Finished.
	gotitSignal = (unsigned char) 0;
	
	// Update Pixels Buffer Pointer
	pixels = ((char*)pBuf + bufferDelta);
}


/*
** Functions: Bitmap Loading
*/


void LoadBMP(const char* filename, int* width, int* height, unsigned char** image_data_ptr){
	FILE *file;
	BMPHeader header;
	BMPInfoHeader info_header;

	// Load Bitmap File
	file = fopen(filename, "rb");
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
};


#endif

