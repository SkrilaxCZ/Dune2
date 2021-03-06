/** @file src/opendune.h Gameloop and other main definitions. */

#ifndef OPENDUNE_H
#define OPENDUNE_H

#include <inttypes.h>
#include "types.h"

enum GameMode
{
	GM_MENU = 0,
	GM_NORMAL = 1,
	GM_RESTART = 2,
	GM_PICKHOUSE = 3,
	GM_WIN = 4,
	GM_LOSE = 5,
	GM_QUITGAME = 6
};

enum GameOverlay
{
	GAMEOVERLAY_NONE,
	GAMEOVERLAY_MENTAT,
	GAMEOVERLAY_OPTIONS,
	GAMEOVERLAY_LOAD_GAME,
	GAMEOVERLAY_SAVE_GAME,
	GAMEOVERLAY_SAVE_ENTRY,
	GAMEOVERLAY_GAME_CONTROLS,
	GAMEOVERLAY_CONFIRM_RESTART,
	GAMEOVERLAY_CONFIRM_PICK_HOUSE,
	GAMEOVERLAY_CONFIRM_QUIT,
};

extern uint32 g_hintsShown1;
extern uint32 g_hintsShown2;
extern GameMode g_gameMode;
extern GameOverlay g_gameOverlay;
extern uint16 g_techLevel;
extern uint16 g_campaignID;
extern uint16 g_scenarioID;
extern uint16 g_activeAction;
extern int64_t g_tickScenarioStart;
extern bool g_debugGame;
extern bool g_debugScenario;
extern bool g_debugSkipDialogs;

extern uint16 g_validateStrictIfZero;
extern bool g_generatingMap;
extern uint16 g_selectionType;
extern uint16 g_selectionTypeNew;
extern bool g_viewport_forceRedraw;
extern bool g_viewport_fadein;

extern int16 g_musicInBattle;

extern void* g_readBuffer;
extern uint32 g_readBufferSize;

void GameLoop_TweakWidgetDimensions();
void GameLoop_Main(bool new_game, const char* scenario);
void Game_Prepare();
void Game_Init();
void Game_LoadScenario(uint8 houseID, uint16 scenarioID);
bool Game_LoadScenario(const char* scenario);
void GameLoop_Uninit();
void PrepareEnd();

#endif /* OPENDUNE_H */
