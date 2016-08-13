#ifndef NEWUI_SCROLLBAR_H
#define NEWUI_SCROLLBAR_H

#include "enumeration.h"
#include "../gui/widget.h"

enum ScrollbarItemType
{
	SCROLLBAR_CATEGORY,
	SCROLLBAR_ITEM,
	SCROLLBAR_CHECKBOX,
	SCROLLBAR_BRAIN,
};

struct ScrollbarItem
{
	char text[64];
	ScrollbarItemType type;
	bool no_desc;

	union
	{
		uint32 offset;
		bool* checkbox;
		struct
		{
			Brain* brain;
			HouseType house;
		} br;
	} d;
};

void GUI_Widget_Scrollbar_Init(Widget* w, int16 scrollMax, int16 scrollPageSize, int16 scrollPosition);
void GUI_Widget_Free_WithScrollbar(Widget* w);

Widget* Scrollbar_Allocate(Widget* list, WindowID parentID, int listarea_dx, int scrollbar_dx, int dy, bool set_mentat_widgets);
ScrollbarItem* Scrollbar_AllocItem(Widget* w, ScrollbarItemType type);
void Scrollbar_FreeItems();
void Scrollbar_Sort(Widget* w);
ScrollbarItem* Scrollbar_GetItem(const Widget* w, int i);
ScrollbarItem* Scrollbar_GetSelectedItem(const Widget* w);
void Scrollbar_CycleUp(Widget* w);
void Scrollbar_CycleDown(Widget* w);
bool Scrollbar_ArrowUp_Click(Widget* w);
bool Scrollbar_ArrowDown_Click(Widget* w);
void Scrollbar_HandleEvent(Widget* w, int key);
bool Scrollbar_Click(Widget* w);

#endif
