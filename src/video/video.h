#ifndef VIDEO_VIDEO_H
#define VIDEO_VIDEO_H

#include "../shape.h"
#include "types.h"
#include "prim.h"

enum
{
	RADIO_BUTTON_BACKGROUND_COLOUR = 20,
	CONQUEST_COLOUR = 146,
	STRATEGIC_MAP_ARROW_EDGE_COLOUR = 148,
	WINDTRAP_COLOUR = 223,
	STRATEGIC_MAP_ARROW_COLOUR = 251,
};

enum CPSID
{
	CPS_MENUBAR_LEFT,
	CPS_MENUBAR_MIDDLE,
	CPS_MENUBAR_RIGHT,
	CPS_STATUSBAR_LEFT,
	CPS_STATUSBAR_MIDDLE,
	CPS_STATUSBAR_RIGHT,
	CPS_SIDEBAR_TOP,
	CPS_SIDEBAR_MIDDLE,
	CPS_SIDEBAR_BOTTOM,
	CPS_CONQUEST_EN,
	CPS_CONQUEST_FR,
	CPS_CONQUEST_DE,

	CPS_SPECIAL_MAX
};

ENUM_MATH_OPERATORS(CPSID)

struct FadeInAux;

void Video_InitMentatSprites(bool use_benepal);

void Video_SetPalette(const uint8* palette, int from, int length);
void Video_SetClippingArea(int x, int y, int w, int h);
void Video_SetCursor(int cursor);
void Video_ShowCursor();
void Video_HideCursor();
void Video_WarpCursor(int x, int y);
void Video_ShadeScreen(int alpha);
void Video_HoldBitmapDrawing(bool hold);

void Video_DrawFadeIn(const struct FadeInAux* aux);
bool Video_TickFadeIn(struct FadeInAux* aux);
struct FadeInAux* Video_InitFadeInCPS(const char* filename, int x, int y, int w, int h, bool fade_in);
struct FadeInAux* Video_InitFadeInShape(ShapeID shapeID, HouseType houseID, int x, int y);

void Video_DrawMinimap(int left, int top, int map_scale, int mode);

#include "video_a5.h"

#define Video_Init()            true
#define Video_Uninit()
#define Video_Tick              VideoA5_Tick

#define Video_DrawCPS                VideoA5_DrawCPS
#define Video_DrawCPSRegion          VideoA5_DrawCPSRegion
#define Video_DrawCPSSpecial         VideoA5_DrawCPSSpecial
#define Video_DrawCPSSpecialScale    VideoA5_DrawCPSSpecialScale
#define Video_DrawIcon          VideoA5_DrawIcon
#define Video_DrawIconAlpha     VideoA5_DrawIconAlpha
#define Video_DrawChar          VideoA5_DrawChar
#define Video_DrawWSA           VideoA5_DrawWSA
#define Video_DrawWSAStatic     VideoA5_DrawWSAStatic

#define GUI_Mouse_Show()
#define GUI_Mouse_Hide()
#define GUI_Mouse_Show_Safe()
#define GUI_Mouse_Hide_Safe()
#define GUI_Mouse_Show_InRegion()
#define GUI_Mouse_Hide_InRegion(l,t,r,b)
#define GUI_Mouse_Show_InWidget()   \
	do {} while (false)
#define GUI_Mouse_Hide_InWidget(w)

#endif
