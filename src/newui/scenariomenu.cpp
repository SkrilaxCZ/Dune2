/* savemenu.c */

#include <allegro5/allegro.h>
#include <cassert>
#include <cctype>
#include <cstring>
#include "enum_string.h"
#include "multichar.h"
#include "scrollbar.h"

#include "../os/endian.h"
#include "../os/math.h"
#include "../audio/audio.h"
#include "../file.h"
#include "../gui/gui.h"
#include "../gui/widget.h"
#include "../input/input.h"
#include "../input/mouse.h"
#include "../load.h"
#include "../shape.h"
#include "../string.h"
#include "../scenario.h"

static Widget* s_scrollbar;
char g_scenarioDesc[5][51]; /*!< Array of scenario descriptions for the ScenarioLoad window. */
char g_selectedScenario[51];

static bool ScenarioMenu_IsValidFilename(const ALLEGRO_PATH* path)
{
	const char* extension = al_get_path_extension(path);
	if (stricmp(extension, ".INI") != 0)
		return false;

	return true;
}

static void ScenarioMenu_FindScenarios(Widget* scrollbar)
{
	char dirname[1024];
	File_MakeCompleteFilename(dirname, sizeof(dirname), SEARCHDIR_SCENARIO_DIR, "", false);

	WidgetScrollbar* ws = (WidgetScrollbar*)scrollbar->data;
	ws->scrollMax = 0;

	ALLEGRO_FS_ENTRY* e = al_create_fs_entry(dirname);
	if (e != NULL)
	{
		if (al_open_directory(e))
		{
			ALLEGRO_FS_ENTRY* f = al_read_directory(e);
			while (f != NULL)
			{
				ALLEGRO_PATH* path = al_create_path(al_get_fs_entry_name(f));

				if (ScenarioMenu_IsValidFilename(path))
				{
					const char* filename = al_get_path_filename(path);
					ScrollbarItem* si = Scrollbar_AllocItem(scrollbar, SCROLLBAR_ITEM);
					strncpy(si->text, filename, sizeof(si->text));
				}

				al_destroy_path(path);
				al_destroy_fs_entry(f);
				f = al_read_directory(e);
			}

			al_close_directory(e);
		}

		al_destroy_fs_entry(e);
	}

	Scrollbar_Sort(scrollbar);
	GUI_Widget_Scrollbar_Init(scrollbar, ws->scrollMax, 5, 0);
}

static void ScenarioMenu_FillDesc(Widget* scrollbar)
{
	WidgetScrollbar* ws = (WidgetScrollbar*)scrollbar->data;

	for (int i = 0; i < 5; i++)
	{
		const int entry = ws->scrollPosition + i;
		g_scenarioDesc[i][0] = '\0';

		ScrollbarItem* si = Scrollbar_GetItem(scrollbar, entry);
		if (si == NULL)
			continue;

		const char* filename = si->text;
		if (!File_Exists_Ex(SEARCHDIR_SCENARIO_DIR, filename))
			continue;

		strncpy(g_scenarioDesc[i], filename, sizeof(g_scenarioDesc[i]) / sizeof(char));
	}
}

static void ScenarioMenu_FreeScrollbar()
{
	Widget* w = s_scrollbar;

	while (w != NULL)
	{
		Widget* next = w->next;

		if (w == s_scrollbar)
			GUI_Widget_Free_WithScrollbar(w);
		else
			free(w);

		w = next;
	}

	s_scrollbar = NULL;
}

static void ScenarioMenu_UpdateArrows(Widget* scrollbar)
{
	WidgetScrollbar* ws = (WidgetScrollbar*)scrollbar->data;
	Widget* w;

	w = &g_table_windowWidgets[7];
	if (ws->scrollPosition > 0)
		GUI_Widget_MakeVisible(w);
	else
		GUI_Widget_MakeInvisible(w);

	w = &g_table_windowWidgets[8];
	if (ws->scrollPosition + ws->scrollPageSize < ws->scrollMax)
		GUI_Widget_MakeVisible(w);
	else
		GUI_Widget_MakeInvisible(w);
}

void ScenarioMenu_Init()
{
	WindowDesc* desc = &g_scenarioLoadWindowDesc;

	s_scrollbar = Scrollbar_Allocate(NULL, (WindowID)0, 0, 0, 0, false);

	ScenarioMenu_FindScenarios(s_scrollbar);
	ScenarioMenu_FillDesc(s_scrollbar);

	GUI_Window_Create(desc);

	WidgetScrollbar* ws = (WidgetScrollbar*)s_scrollbar->data;
	if (ws->scrollMax >= 5 && desc->addArrows)
	{
		Widget* w = &g_table_windowWidgets[7];

		w->drawParameterNormal.sprite = SHAPE_SCROLL_UP;
		w->drawParameterSelected.sprite = SHAPE_SCROLL_UP_PRESSED;
		w->drawParameterDown.sprite = SHAPE_SCROLL_UP_PRESSED;
		w->next = NULL;
		w->parentID = desc->index;

		GUI_Widget_MakeNormal(w, false);
		GUI_Widget_MakeInvisible(w);

		g_widgetLinkedListTail = GUI_Widget_Link(g_widgetLinkedListTail, w);

		w = &g_table_windowWidgets[8];

		w->drawParameterNormal.sprite = SHAPE_SCROLL_DOWN;
		w->drawParameterSelected.sprite = SHAPE_SCROLL_DOWN_PRESSED;
		w->drawParameterDown.sprite = SHAPE_SCROLL_DOWN_PRESSED;
		w->next = NULL;
		w->parentID = desc->index;

		GUI_Widget_MakeNormal(w, false);
		GUI_Widget_MakeInvisible(w);

		g_widgetLinkedListTail = GUI_Widget_Link(g_widgetLinkedListTail, w);
	}

	ScenarioMenu_UpdateArrows(s_scrollbar);
}

/* return values:
* -3: scroll button pressed.
* -2: scenario was loaded.
* -1: cancel clicked.
*  0: stay in scenario load game loop.
*/
int ScenarioMenu_Click()
{
	Widget* scrollbar = s_scrollbar;
	Widget* w = g_widgetLinkedListTail;
	uint16 key = GUI_Widget_HandleEvents(w);
	int ret = 0;

	if (key == (0x80 | MOUSE_ZAXIS))
	{
		if ((g_mouseDZ > 0) && (!g_table_windowWidgets[7].flags.invisible))
			key = 0x8025;
		else if ((g_mouseDZ < 0) && (!g_table_windowWidgets[8].flags.invisible))
			key = 0x8026;
	}

	if (key & 0x8000)
	{
		Widget* w2;

		key &= 0x7FFF;
		w2 = GUI_Widget_Get_ByIndex(w, key);

		switch (key)
		{
		case 0x25: /* Up */
		case 0x26: /* Down */
			if (key == 0x25)
				Scrollbar_ArrowUp_Click(scrollbar);
			else
				Scrollbar_ArrowDown_Click(scrollbar);

			ScenarioMenu_UpdateArrows(scrollbar);
			ScenarioMenu_FillDesc(scrollbar);
			GUI_Widget_MakeNormal(w2, false);
			return -3;

		case 0x23: /* Cancel */
			ScenarioMenu_FreeScrollbar();
			return -1;

		default:
			const WidgetScrollbar* ws = (const WidgetScrollbar*)scrollbar->data;
			const int entry = ws->scrollPosition + (key - 0x1E);
			const ScrollbarItem* si = Scrollbar_GetItem(scrollbar, entry);
			strncpy(g_selectedScenario, si->text, sizeof(g_selectedScenario));
			ScenarioMenu_FreeScrollbar();
			Audio_LoadSampleSet(g_table_houseInfo[g_playerHouseID].sampleSet);
			return -2;
		}
	}

	return ret;
}
