#ifndef _MAIN_H_
#define _MAIN_H_

#define MAX_MUSIC_CHANNELS 128
#define MAX_TEXT_CHANNELS 128

enum TextLayout {
	WORDBREAK = 0,   // DT_WORDBREAK
	ELLIPSIS = 1,    // DT_END_ELLIPSIS
	CENTER = 2,      // DT_CENTER
	SINGLE_LINE = 3  // DT_CENTER | DT_VCENTER | DT_SINGLELINE
};

void Setup(HWND& hwnd, bool* wannaUpdate);
void Update(HWND& hwnd, bool* wannaExit);
void Exit();

// For Text Interface, Sharing Variable to MagicHost.cpp
extern bool textEnabled[];
extern wchar_t* textPointers[];

extern unsigned char textColorR[];
extern unsigned char textColorG[];
extern unsigned char textColorB[];

extern unsigned char bgColorR[];
extern unsigned char bgColorG[];
extern unsigned char bgColorB[];
extern bool bgTransparent[];

extern int textX[];
extern int textY[];
extern int textWidth[];
extern int textHeight[];
extern TextLayout textLayout[];

#endif
