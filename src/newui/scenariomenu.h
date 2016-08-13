#ifndef NEWUI_SCENARIOMENU_H
#define NEWUI_SCENARIOMENU_H

#include "types.h"

extern char g_scenarioDesc[5][51];
extern char g_selectedScenario[51];

void ScenarioMenu_Init();
int ScenarioMenu_Click();

#endif
