#ifndef TABLE_LOCALE_H
#define TABLE_LOCALE_H

struct LanguageInfo
{
	char name[4];
	char suffix[4];
};

extern const LanguageInfo g_table_languageInfo;

#endif
