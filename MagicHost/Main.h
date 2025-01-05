#ifndef _MAIN_H_
#define _MAIN_H_

#define MAX_MUSIC_CHANNELS 128
#define MAX_TEXT_CHANNELS 128

enum TextLayout {
	CLIP = 0,        // NULL
	WORDBREAK = 1,   // DT_WORDBREAK
	ELLIPSIS = 2,    // DT_END_ELLIPSIS
	CENTER = 3,      // DT_CENTER
	SINGLE_LINE = 4  // DT_CENTER | DT_VCENTER | DT_SINGLELINE
};

void Setup(HWND& hwnd, bool* wannaUpdate);
void Update(HWND& hwnd, bool* wannaExit);
void Exit();

// For Text Interface, Sharing Variable with MagicHost.cpp
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

// For Font Interface, Sharing Variable with MagicHost.cpp
extern bool fontEnabled[];
extern HFONT defaultFontPtr;
extern HFONT fontPtrs[];

extern unsigned char invokeInternalFontCreator;
extern int invokeFontChannel;
extern int invokeFontSize;
extern bool invokeFontItalic;
extern bool invokeFontBold;
extern bool invokeFontUnderline;
extern wchar_t* invokeFontFamily;

#endif
