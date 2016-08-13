#ifndef NEWUI_MENTAT_H
#define NEWUI_MENTAT_H

#include <inttypes.h>
#include "enumeration.h"
#include "../file.h"
#include "../house.h"

enum BriefingState
{
	MENTAT_SHOW_CONTENTS,
	MENTAT_PAUSE_DESCRIPTION,
	MENTAT_SHOW_DESCRIPTION,
	MENTAT_SHOW_TEXT,
	MENTAT_IDLE
};

enum BriefingEntry
{
	MENTAT_BRIEFING_ORDERS = 0,
	MENTAT_BRIEFING_WIN = 1,
	MENTAT_BRIEFING_LOSE = 2,
	MENTAT_BRIEFING_ADVICE = 3
};

typedef struct MentatState
{
	BriefingState state;

	char buf[1024];

	char* desc;
	int desc_lines;
	int64_t desc_timer;

	char* text;
	int lines0;
	int lines;

	int speaking_mode;
	int64_t speaking_timer;

	void* wsa;
	int wsa_frame;
	int64_t wsa_timer;
} MentatState;

struct Widget;

extern int movingEyesSprite;
extern int movingMouthSprite;
extern int otherSprite;
extern MentatState g_mentat_state;

void Mentat_LoadHelpSubjects(struct Widget* scrollbar, bool init, SearchDirectory dir, HouseType houseID, int campaignID, bool skip_advice);

MentatID Mentat_InitFromString(const char* str, HouseType houseID);

void Mentat_GetEyePositions(MentatID mentatID, int* left, int* top, int* right, int* bottom);
void Mentat_GetMouthPositions(MentatID mentatID, int* left, int* top, int* right, int* bottom);
void Mentat_DrawBackground(MentatID mentatID);
void Mentat_Draw(MentatID mentatID);

void MentatBriefing_SplitText(MentatState* mentat);
void MentatBriefing_InitText(HouseType houseID, uint16 campaignID, BriefingEntry entry, MentatState* mentat);
void MentatBriefing_DrawText(const MentatState* mentat);
void MentatBriefing_AdvanceText(MentatState* mentat);

void MentatBriefing_InitWSA(HouseType houseID, uint16 scenarioID, BriefingEntry entry, MentatState* mentat);
void MentatBriefing_DrawWSA(MentatState* mentat);

void MentatHelp_Draw(MentatID mentatID, MentatState* mentat);
void MentatHelp_TickPauseDescription(MentatState* mentat);
void MentatHelp_TickShowDescription(MentatState* mentat);
bool MentatHelp_Tick(MentatState* mentat);

#endif
