#ifndef VIDEO_VIDEOA5_H
#define VIDEO_VIDEOA5_H

#include "video.h"
#include "../file.h"

enum GraphicsDriver
{
	GRAPHICS_DRIVER_OPENGL,
	GRAPHICS_DRIVER_DIRECT3D,
};

extern GraphicsDriver g_graphics_driver;

bool VideoA5_Init();
void VideoA5_Uninit();
void VideoA5_ToggleFullscreen();
void VideoA5_ToggleFPS();
void VideoA5_CaptureScreenshot();
void VideoA5_Tick();

void VideoA5_InitSprites();
void VideoA5_DisplayFound();
void VideoA5_DrawCPS(SearchDirectory dir, const char* filename);
void VideoA5_DrawCPSRegion(SearchDirectory dir, const char* filename, int sx, int sy, int dx, int dy, int w, int h);
void VideoA5_DrawCPSSpecial(CPSID cpsID, HouseType houseID, int x, int y);
void VideoA5_DrawCPSSpecialScale(CPSID cpsID, HouseType houseID, int x, int y, float scale);
void VideoA5_DrawIcon(uint16 iconID, HouseType houseID, int x, int y);
void VideoA5_DrawIconAlpha(uint16 iconID, int x, int y, unsigned char alpha);
void VideoA5_DrawRectCross(int x1, int y1, int w, int h, unsigned char c);
void VideoA5_DrawShape(ShapeID shapeID, HouseType houseID, int x, int y, int flags);
void VideoA5_DrawShapeRotate(ShapeID shapeID, HouseType houseID, int x, int y, int orient256, int flags);
void VideoA5_DrawShapeScale(ShapeID shapeID, int x, int y, int w, int h, int flags);
void VideoA5_DrawShapeGrey(ShapeID shapeID, int x, int y, int flags);
void VideoA5_DrawShapeGreyScale(ShapeID shapeID, int x, int y, int w, int h, int flags);
void VideoA5_DrawShapeTint(ShapeID shapeID, int x, int y, unsigned char c, int flags);
void VideoA5_DrawChar(unsigned char c, const uint8* pal, int x, int y);
bool VideoA5_DrawWSA(void* wsa, int frame, int sx, int sy, int dx, int dy, int w, int h);
void VideoA5_DrawWSAStatic(int frame, int x, int y);

#endif
