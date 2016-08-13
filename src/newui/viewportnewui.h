#ifndef NEWUI_VIEWPORT_H
#define NEWUI_VIEWPORT_H

#include "../gui/widget.h"
#include "../unit.h"

void Viewport_Init();
void Viewport_Hotkey(SquadID squad);
void Viewport_Homekey();
void Viewport_DrawTiles();
void Viewport_DrawTileFog();
void Viewport_DrawSandworm(const Unit* u);
void Viewport_DrawUnit(const Unit* u, int windowX, int windowY, bool render_for_blur_effect);
void Viewport_DrawAirUnit(const Unit* u);
void Viewport_DrawRallyPoint();
void Viewport_DrawSelectionHealthBars();
void Viewport_DrawSelectionBox();
void Viewport_DrawPanCursor();
void Viewport_RenderBrush(int x, int y, int blurx);
bool Viewport_Click(Widget* w);

#endif
