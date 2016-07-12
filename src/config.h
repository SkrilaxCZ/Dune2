/** @file src/config.h Configuration and options load and save definitions. */

#ifndef CONFIG_H
#define CONFIG_H

enum WindowMode
{
	WM_WINDOWED,
	WM_FULLSCREEN,
	WM_FULLSCREEN_WINDOW
};

struct GameCfg
{
	WindowMode windowMode;
	int gameSpeed;
	bool hints;
	bool autoScroll;
	bool scrollAlongScreenEdge;
	int scrollSpeed;

	/* "Right-click orders" control scheme:
	 * Left  -> select, selection box.
	 * Right -> order, pan.
	 *
	 * "Left-click orders" (Dune 2000) control scheme:
	 * Left  -> select/order, selection box.
	 * Right -> deselect, pan.
	 */
	bool leftClickOrders;
	bool holdControlToZoom;
	float panSensitivity;
};

extern GameCfg g_gameConfig;

void GameOptions_Load();
void GameOptions_Save();

#endif /* CONFIG_H */
