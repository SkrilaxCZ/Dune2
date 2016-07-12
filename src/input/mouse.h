#ifndef INPUT_MOUSE_H
#define INPUT_MOUSE_H

#include <stdbool.h>
#include "scancode.h"
#include "../gfx.h"

extern int g_mouseX;
extern int g_mouseY;
extern int g_mouseDX;
extern int g_mouseDY;
extern int g_mouseDZ;
extern int g_mouseClickX;
extern int g_mouseClickY;
extern bool g_mousePanning;
extern bool g_mouseHidden;

void Mouse_Init();
void Mouse_TransformToDiv(ScreenDivID div, int* mouseX, int* mouseY);
void Mouse_TransformFromDiv(ScreenDivID div, int* mouseX, int* mouseY);
void Mouse_EventHandler(bool apply_transform, int x, int y, int dz, Scancode state);
extern bool Mouse_InRegion(int x1, int y1, int x2, int y2);
extern bool Mouse_InRegion_Div(ScreenDivID div, int x1, int y1, int x2, int y2);

#endif
