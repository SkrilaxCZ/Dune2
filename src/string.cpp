/** @file src/string.c String routines. */

#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <cctype>
#include <cstring>
#include <cassert>
#include "enum_string.h"
#include "types.h"
#include "os/math.h"
#include "os/endian.h"

#include "string.h"

#include "file.h"
#include "house.h"
#include "table/locale.h"

static char** s_strings = NULL;
static uint16 s_stringsCount = 0;
static char* s_strings_mentat[HOUSE_MAX][40];

static const char* const s_stringDecompress = " etainosrlhcdupmtasio wb rnsdalmh ieorasnrtlc synstcloer dtgesionr ufmsw tep.icae oiadur laeiyodeia otruetoakhlr eiu,.oansrctlaileoiratpeaoip bm";

/**
 * Decompress a string.
 *
 * @param source The compressed string.
 * @param dest The decompressed string.
 * @return The length of decompressed string.
 */
uint16 String_Decompress(const char* source, char* dest)
{
	uint16 count;
	const char* s;

	count = 0;

	for (s = source; *s != '\0'; s++)
	{
		uint8 c = *s;
		if ((c & 0x80) != 0)
		{
			c &= 0x7F;
			dest[count++] = s_stringDecompress[c >> 3];
			c = s_stringDecompress[c + 16];
		}
		dest[count++] = c;
	}
	dest[count] = '\0';
	return count;
}

/**
 * Appends ".(ENG|FRE|...)" to the given string.
 *
 * @param name The string to append extension to.
 * @return The new string.
 */
const char* String_GenerateFilename(const char* name)
{
	static char filename[14];

	snprintf(filename, sizeof(filename), "%s.%s", name, g_table_languageInfo.suffix);
	return filename;
}

/**
 * Returns a pointer to the string at given index in current string file.
 *
 * @param stringID The index of the string.
 * @return The pointer to the string.
 */
char* String_Get_ByIndex(uint16 stringID)
{
	return s_strings[stringID];
}

const char* String_GetMentatString(HouseType houseID, int stringID)
{
	return s_strings_mentat[houseID][stringID];
}

/**
 * Translates 0x1B 0xXX occurences into extended ASCII values (0x7F + 0xXX).
 *
 * @param source The untranslated string.
 * @param dest The translated string.
 */
void String_TranslateSpecial(const char* source, char* dest)
{
	if (source == NULL || dest == NULL)
		return;

	while (*source != '\0')
	{
		char c = *source++;
		if (c == 0x1B)
		{
			c = 0x7F + (*source++);
		}
		*dest++ = c;
	}
	*dest = '\0';
}

static bool String_IsOverridable(int stringID)
{
	if (STR_CARRYALL <= stringID && stringID <= STR_SELECT_YOUR_NEXT_REGION)
		return true;

	if ((stringID == STR_PICK_ANOTHER_HOUSE) ||
		(stringID == STR_ARE_YOU_SURE_YOU_WISH_TO_PICK_A_NEW_HOUSE))
		return true;

	return false;
}

static void String_Load(SearchDirectory dir, const char* filename, bool compressed, int start, int end)
{
	uint8* buf;
	uint16 count;
	uint16 i, j;

	buf = (uint8*)File_ReadWholeFile_Ex(dir, String_GenerateFilename(filename));
	count = READ_LE_UINT16(buf) / 2;

	if (end < 0)
		end = start + count - 1;

	if (s_stringsCount < end + 1)
	{
		s_strings = (char **)realloc(s_strings, (end + 1) * sizeof(char *));
		assert(s_strings != NULL);

		for (i = s_stringsCount; i <= end; i++)
			s_strings[i] = NULL;

		if (s_strings[0] == NULL)
			s_strings[0] = strdup("");

		s_stringsCount = end + 1;
	}

	for (i = 0 , j = start; i < count && j <= end; i++)
	{
		if ((s_strings[j] != NULL) && !String_IsOverridable(j))
			continue;

		const char* src = (const char *)buf + READ_LE_UINT16(buf + i * 2);
		char* dst;
		if (compressed)
		{
			dst = (char *)calloc(strlen(src) * 2 + 1, sizeof(char));
			String_Decompress(src, dst);
			String_TranslateSpecial(dst, dst);
		}
		else
		{
			dst = strdup(src);
		}

		String_Trim(dst);
		if (strlen(dst) == 0)
		{
			free(dst);
			continue;
		}

		free(s_strings[j]);
		s_strings[j++] = dst;
	}

	free(buf);
}

static void String_LoadCampaignStrings()
{
	String_Load(SEARCHDIR_GLOBAL_DATA_DIR, "DUNE", false, 1, 339);

	for (HouseType houseID = HOUSE_HARKONNEN; houseID < HOUSE_MAX; houseID++)
	{
		for (unsigned int i = 0; i < 40; i++)
		{
			/* Default string. */
			const uint16 stringID = STR_ORDERS_HARKONNEN_0 + 40 * g_table_houseRemap6to3[houseID] + i;

			char* def = String_Get_ByIndex(stringID);

			if (s_strings_mentat[houseID][i] && s_strings_mentat[houseID][i] != def)
				free(s_strings_mentat[houseID][i]);

			s_strings_mentat[houseID][i] = def;
		}
	}
}

/**
 * Loads the language files in the memory, which is used after that with String_GetXXX_ByIndex().
 */
void String_Init()
{
	String_Load(SEARCHDIR_GLOBAL_DATA_DIR, "DUNE", false, 1, 339);
	String_Load(SEARCHDIR_GLOBAL_DATA_DIR, "MESSAGE", false, 340, 367);
	String_Load(SEARCHDIR_GLOBAL_DATA_DIR, "INTRO", false, 368, 404);
	String_Load(SEARCHDIR_GLOBAL_DATA_DIR, "TEXTH", true, 405, 444);
	String_Load(SEARCHDIR_GLOBAL_DATA_DIR, "TEXTA", true, 445, 484);
	String_Load(SEARCHDIR_GLOBAL_DATA_DIR, "TEXTO", true, 485, 524);
	String_Load(SEARCHDIR_GLOBAL_DATA_DIR, "PROTECT", true, 525, -1);
	String_LoadCampaignStrings();

	/* EU version has one more string in DUNE langfile. */
	if (s_strings[STR_LOAD_GAME] == NULL)
	{
		s_strings[STR_LOAD_GAME] = strdup(s_strings[STR_LOAD_A_GAME]);
	}
	else
	{
		const char* str = s_strings[STR_LOAD_GAME];
		while (*str == ' ')
			str++;

		if (*str == '\0')
		{
			free(s_strings[STR_LOAD_GAME]);
			s_strings[STR_LOAD_GAME] = strdup(s_strings[STR_LOAD_A_GAME]);
		}
	}
}

/**
 * Unloads the language files in the memory.
 */
void String_Uninit()
{
	uint16 i;

	for (i = 0; i < s_stringsCount; i++)
		free(s_strings[i]);
	free(s_strings);
	s_strings = NULL;
}

void String_Trim(char* string)
{
	char* s = string + strlen(string) - 1;
	while (s >= string && isspace((uint8)*s))
	{
		*s = '\0';
		s--;
	}
}

void String_GetBool(const char* str, bool* value)
{
	if (str == NULL)
		return;

	const char c = str[0];

	if (c == '1' || c == 't' || c == 'T' || c == 'y' || c == 'Y')
		*value = true;
	else if (c == '0' || c == 'f' || c == 'F' || c == 'n' || c == 'N')
		*value = false;
}
