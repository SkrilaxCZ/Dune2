/** @file src/gui/mentat.h Mentat gui definitions. */

#ifndef GUI_MENTAT_H
#define GUI_MENTAT_H

#include "enumeration.h"
#include "../file.h"

struct Widget;

extern bool g_disableOtherMovement;
extern char s_mentatFilename[13];

void GUI_Mentat_Draw(bool force);
void GUI_Mentat_Animation(MentatID mentatID, uint16 speakingMode);
void GUI_Mentat_Create_HelpScreen_Widgets();
void GUI_Mentat_ShowHelp(struct Widget* scrollbar, SearchDirectory dir, HouseType houseID, int campaignID);
extern uint16 GUI_Mentat_SplitText(char* str, uint16 maxWidth);

#endif /* GUI_MENTAT_H */
