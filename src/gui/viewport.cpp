/** @file src/gui/viewport.c Viewport routines. */

#include <cstring>

#include "types.h"
#include "gui.h"
#include "widget.h"

#include "../common_a5.h"
#include "../enhancement.h"
#include "../explosion.h"
#include "../gfx.h"
#include "../house.h"
#include "../map.h"
#include "../newui/viewportnewui.h"
#include "../opendune.h"
#include "../pool/pool.h"
#include "../pool/unitpool.h"
#include "../scenario.h"
#include "../sprites.h"
#include "../structure.h"
#include "../table/widgetinfo.h"
#include "../tools/coord.h"
#include "../unit.h"
#include "../video/video.h"

/**
 * Redraw parts of the viewport that require redrawing.
 *
 * @param forceRedraw If true, dirty flags are ignored, and everything is drawn.
 * @param arg08 ??
 * @param drawToMainScreen True if and only if we are drawing to the main screen and not some buffer screen.
 */
void GUI_Widget_Viewport_Draw(bool forceRedraw, bool arg08, bool drawToMainScreen)
{
	uint16 x;
	uint16 y;
	uint16 i;
	bool updateDisplay;
	Screen oldScreenID;
	uint16 oldValue_07AE_0000;
	int16 minX[10];
	int16 maxX[10];

	PoolFindStruct find;

	updateDisplay = forceRedraw;

	memset(minX, 0xF, sizeof(minX));
	memset(maxX, 0, sizeof(minX));

	oldScreenID = GFX_Screen_SetActive(SCREEN_1);

	oldValue_07AE_0000 = Widget_SetCurrentWidget(2);

	Viewport_DrawTiles();

	{
		find.type = UNIT_SANDWORM;
		find.index = 0xFFFF;
		find.houseID = HOUSE_INVALID;

		Unit* u = Unit_Find(&find);
		while (u != NULL)
		{
			Viewport_DrawSandworm(u);
			u = Unit_Find(&find);
		}
	}

	/* Draw selected unit under units. */
	if ((g_selectionType != SELECTIONTYPE_PLACE) && !Unit_AnySelected() && (Structure_Get_ByPackedTile(g_selectionRectanglePosition) != NULL))
	{
		const int x1 = TILE_SIZE * (Tile_GetPackedX(g_selectionRectanglePosition) - Tile_GetPackedX(g_viewportPosition)) - g_viewport_scrollOffsetX;
		const int y1 = TILE_SIZE * (Tile_GetPackedY(g_selectionRectanglePosition) - Tile_GetPackedY(g_viewportPosition)) - g_viewport_scrollOffsetY;
		const int x2 = x1 + (TILE_SIZE * g_selectionWidth) - 1;
		const int y2 = y1 + (TILE_SIZE * g_selectionHeight) - 1;

		Prim_Rect_i(x1, y1, x2, y2, 0xFF);
	}

	find.type = 0xFFFF;
	find.index = 0xFFFF;
	find.houseID = HOUSE_INVALID;

	Unit* u = Unit_Find(&find);
	while (u != NULL)
	{
		if ((UNIT_INDEX_SABOTEUR_START <= u->o.index && u->o.index <= UNIT_INDEX_NORMAL_END))
			Viewport_DrawUnit(u, 0, 0, false);

		u = Unit_Find(&find);
	}

	Explosion_Draw();
	Viewport_DrawTileFog();

	Viewport_DrawRallyPoint();
	Viewport_DrawSelectionHealthBars();
	Viewport_DrawSelectionBox();
	Viewport_DrawPanCursor();

	/* Draw placement box over fog. */
	if (g_selectionType == SELECTIONTYPE_PLACE)
	{
		const int x1 = TILE_SIZE * (Tile_GetPackedX(g_selectionRectanglePosition) - Tile_GetPackedX(g_viewportPosition)) - g_viewport_scrollOffsetX;
		const int y1 = TILE_SIZE * (Tile_GetPackedY(g_selectionRectanglePosition) - Tile_GetPackedY(g_viewportPosition)) - g_viewport_scrollOffsetY;

		if (g_selectionState == 0 && g_selectionType == SELECTIONTYPE_PLACE)
		{
			VideoA5_DrawRectCross(x1, y1, g_selectionWidth, g_selectionHeight, 0xFF);
		}
		else
		{
			const int x2 = x1 + (TILE_SIZE * g_selectionWidth) - 1;
			const int y2 = y1 + (TILE_SIZE * g_selectionHeight) - 1;

			Prim_Rect_i(x1, y1, x2, y2, 0xFF);
		}
	}

	find.type = 0xFFFF;
	find.index = 0xFFFF;
	find.houseID = HOUSE_INVALID;

	u = Unit_Find(&find);
	while (u != NULL)
	{
		if (u->o.index <= UNIT_INDEX_PROJECTILE_END)
			Viewport_DrawAirUnit(u);

		u = Unit_Find(&find);
	}

	if ((g_viewportMessageCounter & 1) != 0 && g_viewportMessageText != NULL && (minX[6] <= 14 || maxX[6] >= 0 || arg08 || forceRedraw))
	{
		const ScreenDivID old_div = A5_SaveTransform();
		A5_UseTransform(SCREENDIV_MENU);

		const WidgetInfo* wi = &g_table_gameWidgetInfo[GAME_WIDGET_VIEWPORT];
		const int xcentre = (wi->width * g_screenDiv[SCREENDIV_VIEWPORT].scalex) / (2 * g_screenDiv[SCREENDIV_MENU].scalex);
		const int ymessage = SCREEN_HEIGHT - 61;

		GUI_DrawText_Wrapper(g_viewportMessageText, xcentre, ymessage, 15, 0, 0x132);
		minX[6] = -1;
		maxX[6] = 14;

		A5_UseTransform(old_div);
	}

	if (updateDisplay && !drawToMainScreen)
	{
		if (g_viewport_fadein)
		{
			GUI_Mouse_Hide_InWidget(g_curWidgetIndex);
			Screen oldScreenID2 = g_screenActiveID;

			GFX_Screen_SetActive(SCREEN_0);
			Prim_FillRect_i(g_curWidgetXBase, g_curWidgetYBase, g_curWidgetXBase + g_curWidgetWidth, g_curWidgetYBase + g_curWidgetHeight, 0);
			GFX_Screen_SetActive(oldScreenID2);
			GUI_Screen_FadeIn(g_curWidgetXBase / 8, g_curWidgetYBase, g_curWidgetXBase / 8, g_curWidgetYBase, g_curWidgetWidth / 8, g_curWidgetHeight, g_screenActiveID, SCREEN_0);
			GUI_Mouse_Show_InWidget();

			g_viewport_fadein = false;
		}
		else
		{
			bool init = false;

			for (i = 0; i < 10; i++)
			{
				uint16 width;
				uint16 height;

				if (arg08)
				{
					minX[i] = 0;
					maxX[i] = 14;
				}

				if (maxX[i] < minX[i])
					continue;

				x = minX[i] * 2;
				y = (i << 4) + 0x28;
				width = (maxX[i] - minX[i] + 1) * 2;
				height = 16;

				if (!init)
				{
					GUI_Mouse_Hide_InWidget(g_curWidgetIndex);

					init = true;
				}

				GUI_Screen_Copy(x, y, x, y, width, height, g_screenActiveID, SCREEN_0);
			}

			if (init)
			GUI_Mouse_Show_InWidget();
		}
	}

	GFX_Screen_SetActive(oldScreenID);

	Widget_SetCurrentWidget(oldValue_07AE_0000);
}

void GUI_Widget_Viewport_RedrawMap()
{
	const WidgetInfo* wi = &g_table_gameWidgetInfo[GAME_WIDGET_MINIMAP];
	Video_DrawMinimap(wi->offsetX, wi->offsetY, g_scenario.mapScale, 0);
	Map_UpdateMinimapPosition(g_viewportPosition, true);
}
