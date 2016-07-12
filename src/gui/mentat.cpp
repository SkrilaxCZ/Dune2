/** @file src/gui/mentat.c Mentat gui routines. */

#include <cstdio>
#include "multichar.h"
#include "types.h"
#include "../os/endian.h"
#include "../os/sleep.h"

#include "mentat.h"

#include "font.h"
#include "widget.h"
#include "../config.h"
#include "../enhancement.h"
#include "../file.h"
#include "../gfx.h"
#include "../house.h"
#include "../input/input.h"
#include "../input/mouse.h"
#include "../newui/mentatnewui.h"
#include "../newui/scrollbar.h"
#include "../opendune.h"
#include "../scenario.h"
#include "../sprites.h"
#include "../string.h"
#include "../timer/timer.h"
#include "../tools/random_lcg.h"
#include "../wsa.h"

static uint8* s_mentatSprites[3][5];

bool g_disableOtherMovement; /*!< Disable moving of the other object. */

char s_mentatFilename[13];

static void GUI_Mentat_SetSprites(MentatID mentatID)
{
	memset(s_mentatSprites, 0, sizeof(s_mentatSprites));

	for (int i = 0; i < 5; i++)
	{
		s_mentatSprites[0][i] = g_sprites[387 + mentatID * 15 + i];
	}

	for (int i = 0; i < 5; i++)
	{
		s_mentatSprites[1][i] = g_sprites[392 + mentatID * 15 + i];
	}

	for (int i = 0; i < 4; i++)
	{
		s_mentatSprites[2][i] = g_sprites[398 + mentatID * 15 + i];
	}
}

/**
 * Draw sprites and handle mouse in a mentat screen.
 * @param speakingMode If \c 1, the mentat is speaking.
 */
void GUI_Mentat_Animation(MentatID mentatID, uint16 speakingMode)
{
	static int64_t movingEyesTimer = 0; /* Timer when to change the eyes sprite. */
	static uint16 movingEyesNextSprite = 0; /* If not 0, it decides the movingEyesNextSprite */
	static int64_t movingMouthTimer = 0;
	static int64_t movingOtherTimer = 0;

	int s_eyesLeft, s_eyesTop, s_eyesRight, s_eyesBottom;
	int s_mouthLeft, s_mouthTop, s_mouthRight, s_mouthBottom;

	GUI_Mentat_SetSprites(mentatID);
	Mentat_GetEyePositions(mentatID, &s_eyesLeft, &s_eyesTop, &s_eyesRight, &s_eyesBottom);
	Mentat_GetMouthPositions(mentatID, &s_mouthLeft, &s_mouthTop, &s_mouthRight, &s_mouthBottom);

	uint16 i;

	if (movingOtherTimer < Timer_GetTicks() && !g_disableOtherMovement)
	{
		if (movingOtherTimer != 0)
		{
			if (s_mentatSprites[2][1 + abs(otherSprite)] == NULL)
			{
				otherSprite = 1 - otherSprite;
			}
			else
			{
				otherSprite++;
			}
		}

		switch (mentatID)
		{
		case MENTAT_RADNOR:
			break;
		case MENTAT_CYRIL:
			movingOtherTimer = Timer_GetTicks() + 60 * Tools_RandomLCG_Range(1, 3);
			break;
		case MENTAT_AMMON:
			if (otherSprite != 0)
				movingOtherTimer = Timer_GetTicks() + 6;
			else
				movingOtherTimer = Timer_GetTicks() + 60 * Tools_RandomLCG_Range(10, 19);
			break;
		case MENTAT_BENE_GESSERIT:
		case MENTAT_MAX:
			break;
		}
	}

	if (speakingMode == 1)
	{
		if (movingMouthTimer < Timer_GetTicks())
		{
			movingMouthSprite = Tools_RandomLCG_Range(0, 4);

			switch (movingMouthSprite)
			{
			case 0:
				movingMouthTimer = Timer_GetTicks() + Tools_RandomLCG_Range(7, 30);
				break;
			case 1:
			case 2:
			case 3:
				movingMouthTimer = Timer_GetTicks() + Tools_RandomLCG_Range(6, 10);
				break;
			case 4:
				movingMouthTimer = Timer_GetTicks() + Tools_RandomLCG_Range(5, 6);
				break;
			default:
				break;
			}
		}
	}
	else
	{
		if (Input_Test((Scancode)MOUSE_LMB) == 0 && Input_Test((Scancode)MOUSE_RMB) == 0)
		{
			if (movingMouthSprite != 0)
			{
				movingMouthSprite = 0;
				movingMouthTimer = 0;
			}
		}
		else if (Mouse_InRegion(s_mouthLeft, s_mouthTop, s_mouthRight, s_mouthBottom) != 0)
		{
			if (movingMouthTimer != 0xFFFFFFFF)
			{
				movingMouthTimer = 0xFFFFFFFF;
				movingMouthSprite = Tools_RandomLCG_Range(1, 4);
			}
		}
		else
		{
			if (movingMouthSprite != 0)
			{
				movingMouthSprite = 0;
				movingMouthTimer = 0;
			}
		}
	}

	if (Input_Test((Scancode)MOUSE_LMB) != 0 || Input_Test((Scancode)MOUSE_RMB) != 0)
	{
		if (Mouse_InRegion(s_eyesLeft, s_eyesTop, s_eyesRight, s_eyesBottom) != 0)
		{
			if (movingEyesSprite != 0x4)
			{
				movingEyesSprite = (movingEyesSprite == 3) ? 4 : 3;
				movingEyesNextSprite = 0;
				movingEyesTimer = 0;
			}

			return;
		}
	}

	if (Mouse_InRegion((int)s_eyesLeft - 16, (int)s_eyesTop - 8, s_eyesRight + 16, s_eyesBottom + 24) != 0)
	{
		if (Mouse_InRegion((int)s_eyesLeft - 8, s_eyesBottom, s_eyesRight + 8, SCREEN_HEIGHT - 1) != 0)
		{
			i = 3;
		}
		else
		{
			if (Mouse_InRegion(s_eyesRight, (int)s_eyesTop - 8, s_eyesRight + 16, s_eyesBottom + 8) != 0)
			{
				i = 2;
			}
			else
			{
				i = (Mouse_InRegion((int)s_eyesLeft - 16, (int)s_eyesTop - 8, s_eyesLeft, s_eyesBottom + 8) == 0) ? 0 : 1;
			}
		}

		if (i != movingEyesSprite)
		{
			movingEyesSprite = i;
			movingEyesNextSprite = 0;
			movingEyesTimer = Timer_GetTicks();
		}
	}
	else
	{
		if (movingEyesTimer >= Timer_GetTicks())
			return;

		if (movingEyesNextSprite != 0)
		{
			movingEyesSprite = movingEyesNextSprite;
			movingEyesNextSprite = 0;

			if (movingEyesSprite != 4)
			{
				movingEyesTimer = Timer_GetTicks() + Tools_RandomLCG_Range(20, 180);
			}
			else
			{
				movingEyesTimer = Timer_GetTicks() + Tools_RandomLCG_Range(12, 30);
			}
		}
		else
		{
			switch (speakingMode)
			{
			case 0:
				i = Tools_RandomLCG_Range(0, 7);
				if (i > 5)
					i = 1;
				else if (i == 5)
					i = 4;
				break;

			case 1:
				if (movingEyesSprite != 0)
					i = 0;
				else
				{
					i = Tools_RandomLCG_Range(0, 17);
					if (i > 9)
						i = 0;
					else if (i >= 5)
						i = 4;
				}
				break;

			default:
				i = Tools_RandomLCG_Range(0, 15);
				if (i > 10)
					i = 2;
				else if (i >= 5)
					i = 4;
				break;
			}

			if ((i == 2 && movingEyesSprite == 1) || (i == 1 && movingEyesSprite == 2))
			{
				movingEyesNextSprite = i;
				movingEyesSprite = 0;
				movingEyesTimer = Timer_GetTicks() + Tools_RandomLCG_Range(1, 5);
			}
			else
			{
				if (i != movingEyesSprite && (i == 4 || movingEyesSprite == 4))
				{
					movingEyesNextSprite = i;
					movingEyesSprite = 3;
					movingEyesTimer = Timer_GetTicks();
				}
				else
				{
					movingEyesSprite = i;
					if (i != 4)
						movingEyesTimer = Timer_GetTicks() + Tools_RandomLCG_Range(15, 180);
					else
						movingEyesTimer = Timer_GetTicks() + Tools_RandomLCG_Range(6, 60);
				}
			}
		}
	}
}

/** Create the widgets of the mentat help screen. */
void GUI_Mentat_Create_HelpScreen_Widgets()
{
	if (g_widgetMentatScrollbar != NULL)
	{
		GUI_Widget_Free_WithScrollbar(g_widgetMentatScrollbar);
		g_widgetMentatScrollbar = NULL;
	}

	free(g_widgetMentatScrollUp);
	g_widgetMentatScrollUp = NULL;
	free(g_widgetMentatScrollDown);
	g_widgetMentatScrollDown = NULL;

	g_widgetMentatTail = Scrollbar_Allocate(g_widgetMentatTail, WINDOWID_MENTAT_PICTURE, 0, 0, 0, true);
	g_widgetMentatTail = GUI_Widget_Link(g_widgetMentatTail, g_widgetMentatFirst);
}

void
GUI_Mentat_ShowHelp(Widget* scrollbar, SearchDirectory dir,
                    HouseType houseID, int campaignID)
{
	MentatState* mentat = &g_mentat_state;

	struct
	{
		uint8 notused[8];
		uint32 length;
	} info;

	bool noDesc;
	uint8 fileID;
	uint32 offset;
	char* compressedText;
	char* desc;
	char* picture;
	char* text;

	ScrollbarItem* si = Scrollbar_GetSelectedItem(scrollbar);
	if (si->type == SCROLLBAR_CATEGORY)
		return;

	noDesc = si->no_desc;
	offset = si->d.offset;

	memset(&info, 0, sizeof(info));
	fileID = ChunkFile_Open_Ex(dir, s_mentatFilename);
	uint32 bufread = ChunkFile_Read(fileID, HTOBE32(CC_INFO), &info, 12);

	if (bufread >= 12)
	{
		info.length = HTOBE32(info.length);
	}
	else
	{
		/* Note: some files are buggy and don't give a proper length. */
		info.length = File_GetSize(fileID);
	}

	ChunkFile_Close(fileID);

	text = (char*)g_readBuffer;
	compressedText = (char*)malloc(info.length);

	fileID = File_Open_Ex(dir, s_mentatFilename, FILE_MODE_READ);
	File_Seek(fileID, offset, 0);
	File_Read(fileID, compressedText, info.length);
	String_Decompress(compressedText, text);
	String_TranslateSpecial(text, text);
	File_Close(fileID);

	while (*text != '*' && *text != '?')
		text++;

	*text++ = '\0';

	if (noDesc)
	{
		picture = g_scenario.pictureBriefing;
		desc = NULL;
		text = (char *)g_readBuffer;

		const uint16 index = (campaignID * 4) + (*text - 44);

		strncpy((char*)g_readBuffer, String_GetMentatString(houseID, index), g_readBufferSize);
	}
	else
	{
		picture = (char *)g_readBuffer;
		desc = text;

		while (*text != '\0' && *text != 0xC)
			text++;
		if (*text != '\0')
			*text++ = '\0';
	}

	free(compressedText);
	mentat->state = MENTAT_PAUSE_DESCRIPTION;
	mentat->desc = desc;
	mentat->text = text;

	mentat->wsa = WSA_LoadFile(picture, GFX_Screen_Get_ByIndex(SCREEN_2), GFX_Screen_GetSize_ByIndex(SCREEN_2), false);
	mentat->wsa_timer = Timer_GetTicks();
	mentat->wsa_frame = 0;
}

uint16 GUI_Mentat_SplitText(char* str, uint16 maxWidth)
{
	uint16 lines = 0;
	uint16 height = 0;

	if (str == NULL)
		return 0;

	while (*str != '\0')
	{
		uint16 width = 0;

		while (width < maxWidth && *str != '.' && *str != '!' && *str != '?' && *str != '\0' && *str != '\r')
		{
			width += Font_GetCharWidth(*str++);
		}

		if (width >= maxWidth)
		{
			while (*str != ' ')
				width -= Font_GetCharWidth(*str--);
		}

		height++;

		if ((*str != '\0' && (*str == '.' || *str == '!' || *str == '?' || *str == '\r')) || height >= 3)
		{
			while (*str != '\0' && (*str == ' ' || *str == '.' || *str == '!' || *str == '?' || *str == '\r'))
				str++;

			if (*str != '\0')
				str[-1] = '\0';
			height = 0;
			lines++;
			continue;
		}

		if (*str == '\0')
		{
			lines++;
			height = 0;
			continue;
		}

		*str++ = '\r';
	}

	return lines;
}
