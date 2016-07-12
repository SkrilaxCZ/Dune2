/** @file src/table/houseinfo.c HouseInfo file table. */

#include "sound.h"
#include "../house.h"

const DeathHandSpecialWeaponData g_HarkonnenSpecialWeapon = {NULL};
const FremenSpecialWeaponData g_AtreidesSpecialWeapon = {HOUSE_FREMEN, UNIT_TROOPERS, UNIT_TROOPER};
const SaboteurSpecialWeaponData g_OrdosSpecialWeapon = {HOUSE_ORDOS, UNIT_SABOTEUR};
const FremenSpecialWeaponData g_FremenSpecialWeapon = {HOUSE_FREMEN, UNIT_TROOPERS, UNIT_TROOPER};
const DeathHandSpecialWeaponData g_SardaukarSpecialWeapon = {NULL};
const SaboteurSpecialWeaponData g_MercenarySpecialWeapon = {HOUSE_MERCENARY, UNIT_SABOTEUR};

const HouseInfo g_table_base_houseInfo[HOUSE_MAX] =
{
	{/* 0 */
		/* name                 */ "Harkonnen",
		/* toughness            */ 200,
		/* degradingChance      */ 85,
		/* degradingAmount      */ 3,
		/* spriteColor          */ 0,
		/* specialCountDown     */ 600,
		/* starportDeliveryTime */ 10,
		/* prefixChar           */ 'H',
		/* specialWeapon        */ HOUSE_WEAPON_MISSILE,
		/* musicWin             */ MUSIC_WIN_HARKONNEN,
		/* musicLose            */ MUSIC_LOSE_HARKONNEN,
		/* musicBriefing        */ MUSIC_BRIEFING_HARKONNEN,
		/* voiceFilename        */ "nhark.voc",
		/* mentat               */ MENTAT_RADNOR,
		/* sampleSet            */ SAMPLESET_HARKONNEN,
		/* superWeapon          */ SpecialWeaponData(g_HarkonnenSpecialWeapon),
		/* fameCps              */ 0,
		/* mapmachCps           */ 0,
		/* miscCps              */ 0
	},

	{/* 1 */
		/* name                 */ "Atreides",
		/* toughness            */ 77,
		/* degradingChance      */ 0,
		/* degradingAmount      */ 1,
		/* spriteColor          */ 1,
		/* specialCountDown     */ 300,
		/* starportDeliveryTime */ 10,
		/* prefixChar           */ 'A',
		/* specialWeapon        */ HOUSE_WEAPON_FREMEN,
		/* musicWin             */ MUSIC_WIN_ATREIDES,
		/* musicLose            */ MUSIC_LOSE_ATREIDES,
		/* musicBriefing        */ MUSIC_BRIEFING_ATREIDES,
		/* voiceFilename        */ "nattr.voc",
		/* mentat               */ MENTAT_CYRIL,
		/* sampleSet            */ SAMPLESET_ATREIDES,
		/* superWeapon          */ SpecialWeaponData(g_AtreidesSpecialWeapon),
		/* fameCps              */ 1,
		/* mapmachCps           */ 1,
		/* miscCps              */ 1
	},

	{/* 2 */
		/* name                 */ "Ordos",
		/* toughness            */ 128,
		/* degradingChance      */ 10,
		/* degradingAmount      */ 2,
		/* spriteColour         */ 2,
		/* specialCountDown     */ 300,
		/* starportDeliveryTime */ 10,
		/* prefixChar           */ 'O',
		/* specialWeapon        */ HOUSE_WEAPON_SABOTEUR,
		/* musicWin             */ MUSIC_WIN_ORDOS,
		/* musicLose            */ MUSIC_LOSE_ORDOS,
		/* musicBriefing        */ MUSIC_BRIEFING_ORDOS,
		/* voiceFilename        */ "nordo.voc",
		/* mentat               */ MENTAT_AMMON,
		/* sampleSet            */ SAMPLESET_ORDOS,
		/* superWeapon          */ SpecialWeaponData(g_OrdosSpecialWeapon),
		/* fameCps              */ 2,
		/* mapmachCps           */ 2,
		/* miscCps              */ 2
	},

	{/* 3 */
		/* name                 */ "Fremen",
		/* toughness            */ 55,
		/* degradingChance      */ 0,
		/* degradingAmount      */ 1,
		/* spriteColour         */ 3,
		/* specialCountDown     */ 300,
		/* starportDeliveryTime */ 10, /* was 0. */
		/* prefixChar           */ 'A',
		/* specialWeapon        */ HOUSE_WEAPON_FREMEN,
		/* musicWin             */ MUSIC_WIN_ATREIDES,
		/* musicLose            */ MUSIC_LOSE_ATREIDES,
		/* musicBriefing        */ MUSIC_BRIEFING_ATREIDES,
		/* voiceFilename        */ "afremen.voc",
		/* mentat               */ MENTAT_CYRIL,
		/* sampleSet            */ SAMPLESET_ATREIDES,
		/* superWeapon          */ SpecialWeaponData(g_FremenSpecialWeapon),
		/* fameCps              */ 1,
		/* mapmachCps           */ 1,
		/* miscCps              */ 1
	},

	{/* 4 */
		/* name                 */ "Sardaukar",
		/* toughness            */ 234,
		/* degradingChance      */ 0,
		/* degradingAmount      */ 1,
		/* spriteColour         */ 4,
		/* specialCountDown     */ 600,
		/* starportDeliveryTime */ 10, /* was 0. */
		/* prefixChar           */ 'H',
		/* specialWeapon        */ HOUSE_WEAPON_MISSILE,
		/* musicWin             */ MUSIC_WIN_HARKONNEN,
		/* musicLose            */ MUSIC_LOSE_HARKONNEN,
		/* musicBriefing        */ MUSIC_BRIEFING_HARKONNEN,
		/* voiceFilename        */ "asard.voc",
		/* mentat               */ MENTAT_RADNOR,
		/* sampleSet            */ SAMPLESET_HARKONNEN,
		/* superWeapon          */ SpecialWeaponData(g_SardaukarSpecialWeapon),
		/* fameCps              */ 0,
		/* mapmachCps           */ 0,
		/* miscCps              */ 0
	},

	{/* 5 */
		/* name                 */ "Mercenary",
		/* toughness            */ 0,
		/* degradingChance      */ 10,
		/* degradingAmount      */ 2,
		/* spriteColour         */ 5,
		/* specialCountDown     */ 300,
		/* starportDeliveryTime */ 10, /* was 0. */
		/* prefixChar           */ 'O',
		/* specialWeapon        */ HOUSE_WEAPON_SABOTEUR,
		/* musicWin             */ MUSIC_WIN_ORDOS,
		/* musicLose            */ MUSIC_LOSE_ORDOS,
		/* musicBriefing        */ MUSIC_BRIEFING_ORDOS,
		/* voiceFilename        */ "amerc.voc",
		/* mentat               */ MENTAT_AMMON,
		/* sampleSet            */ SAMPLESET_ORDOS,
		/* superWeapon          */ SpecialWeaponData(g_MercenarySpecialWeapon),
		/* fameCps              */ 2,
		/* mapmachCps           */ 2,
		/* miscCps              */ 2
	}
};

const uint8 g_table_minimapColour[HOUSE_MAX] =
{
	/* Harkonnen */ 144,
	/* Atreides  */ 160,
	/* Ordos     */ 176,
	/* Fremen    */ 192,
	/* Sardaukar */ 208,
	/* Mercenary */ 224,
};

HouseInfo g_table_houseInfo[HOUSE_MAX];
HouseAlliance g_table_houseAlliance[HOUSE_MAX][HOUSE_MAX];

const HouseType g_table_houseRemap6to3[HOUSE_MAX] =
{
	HOUSE_HARKONNEN, HOUSE_ATREIDES, HOUSE_ORDOS,
	HOUSE_ATREIDES, HOUSE_HARKONNEN, HOUSE_ORDOS
};
