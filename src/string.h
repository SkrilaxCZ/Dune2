/** @file src/string.h String definitions. */

#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include "enumeration.h"

extern uint16 String_Decompress(const char* source, char* dest);
extern const char* String_GenerateFilename(const char* name);
extern char* String_Get_ByIndex(uint16 stringID);
extern const char* String_GetMentatString(HouseType houseID, int stringID);
void String_TranslateSpecial(const char* source, char* dest);
void String_LoadCampaignStrings();
void String_Init();
void String_Uninit();
void String_Trim(char* string);
void String_GetBool(const char* str, bool* value);

#endif /* STRING_H */
