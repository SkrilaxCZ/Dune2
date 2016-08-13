#ifndef NEWUI_SAVEMENU_H
#define NEWUI_SAVEMENU_H

#include "types.h"

extern char g_savegameDesc[5][51];

int SaveMenu_Savegame_Click(uint16 key);
void SaveMenu_InitSaveLoad(bool save);
int SaveMenu_SaveLoad_Click(bool save);

#endif
