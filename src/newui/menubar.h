#ifndef NEWUI_MENUBAR_H
#define NEWUI_MENUBAR_H

#include "../gui/widget.h"
#include "../house.h"

void MenuBar_DrawCredits(int credits_new, int credits_old, int offset, int x0);
void MenuBar_DrawStatusBar(const char* line1, const char* line2, bool scrollInProgress, int x, int y, int offset);
void MenuBar_Draw(HouseType houseID);
void MenuBar_StartRadarAnimation(bool activate);

extern bool MenuBar_ClickMentat(Widget* w);
void MenuBar_TickMentatOverlay();
void MenuBar_DrawMentatOverlay();

extern bool MenuBar_ClickOptions(Widget* w);
void MenuBar_TickOptionsOverlay();
void MenuBar_DrawOptionsOverlay();

extern uint16 GUI_DisplayModalMessage(const char* str, uint16 shapeID, ...);

#endif
