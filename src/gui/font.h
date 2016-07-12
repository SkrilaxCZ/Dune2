/** @file src/gui/font.h Font definitions. */

#ifndef GUI_FONT_H
#define GUI_FONT_H

struct FontChar
{
	uint8 width;
	uint8 unusedLines;
	uint8 usedLines;
	uint8* data;
};

struct Font
{
	uint8 height;
	uint8 maxWidth;
	uint8 count;
	FontChar* chars;
};

extern Font* g_fontIntro;
extern Font* g_fontNew6p;
extern Font* g_fontNew8p;

extern int8 g_fontCharOffset;

extern Font* g_fontCurrent;

bool Font_Init();
void Font_Uninit();
uint16 Font_GetCharWidth(unsigned char c);
uint16 Font_GetStringWidth(const char* string);
void Font_Select(Font* f);

#endif /* GUI_FONT_H */
