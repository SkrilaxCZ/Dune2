/** @file src/gfx.h Graphics definitions. */

#ifndef GFX_H
#define GFX_H

#include "types.h"

enum
{
	SCREEN_WIDTH = 320, /*!< Width of Dune 2 screen in pixels. */
	SCREEN_HEIGHT = 200 /*!< Height of Dune 2 screen in pixels. */
};

enum Screen
{
	SCREEN_0 = 0,
	SCREEN_1 = 2,
	SCREEN_2 = 4,
	SCREEN_3 = 6
};

enum AspectRatioCorrection
{
	ASPECT_RATIO_CORRECTION_NONE, /* Square pixels. */
	ASPECT_RATIO_CORRECTION_PARTIAL, /* Non-square pixels for menus, square pixels in game. */
	ASPECT_RATIO_CORRECTION_FULL, /* Non-square pixels throughout the game. */
	ASPECT_RATIO_CORRECTION_AUTO
};

enum ScreenDivID
{
	SCREENDIV_MAIN = 0,
	SCREENDIV_MENU = 1,
	SCREENDIV_MENUBAR = 2,
	SCREENDIV_SIDEBAR = 3,
	SCREENDIV_VIEWPORT = 4,

	SCREENDIV_MAX
};

ENUM_MATH_OPERATORS(ScreenDivID)

struct ScreenDiv
{
	float scalex;
	float scaley;
	int x, y;
	int width, height;
};

extern int TRUE_DISPLAY_WIDTH;
extern int TRUE_DISPLAY_HEIGHT;
extern AspectRatioCorrection g_aspect_correction;
extern float g_pixel_aspect_ratio; /* pixel height to pixel width. */
extern Screen g_screenActiveID;
extern ScreenDiv g_screenDiv[SCREENDIV_MAX];

void GFX_InitDefaultViewportScales(bool adjust_viewport);
float GFX_AspectCorrection_GetRatio();

void GFX_Init();
void GFX_Uninit();
Screen GFX_Screen_SetActive(Screen screenID);
void* GFX_Screen_GetActive();
uint16 GFX_Screen_GetSize_ByIndex(Screen screenID);
void* GFX_Screen_Get_ByIndex(Screen screenID);

void GFX_DrawSprite(uint16 spriteID, uint16 x, uint16 y, uint8 houseID);
void GFX_Init_SpriteInfo(uint16 widthSize, uint16 heightSize);
void GFX_Screen_Copy2(int16 xSrc, int16 ySrc, int16 xDst, int16 yDst, int16 width, int16 height, Screen screenSrc, Screen screenDst, bool skipNull);
void GFX_Screen_Copy(int16 xSrc, int16 ySrc, int16 xDst, int16 yDst, int16 width, int16 height, Screen screenSrc, Screen screenDst);
void GFX_ClearScreen();
void GFX_ClearBlock(Screen index);
void GFX_SetPalette(uint8* palette);

void GFX_ScreenShake_Start(int num_ticks);
bool GFX_ScreenShake_Tick();
int GFX_ScreenShake_Offset();

#endif /* GFX_H */
