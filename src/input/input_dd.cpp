/* input.c */

#include <assert.h>
#include <string.h>

#include "input.h"

static Scancode s_history[256];
static int s_historyHead = 0;
static int s_historyTail = 0;

/* s_activeInputMap stores the states of the keys (or mouse buttons).
 * To test if a key with scancode c is down, use something like:
 *
 *	if (s_activeInputMap[(c & 0x7F) >> 3] & (1 << (c & 0x7))) {}
 */
static unsigned char s_activeInputMap[0x80 / 8];

void Input_Init()
{
	memset(s_activeInputMap, 0, sizeof(s_activeInputMap));
}

void Input_History_Clear()
{
	s_historyTail = s_historyHead;
}

static bool Input_AddHistory(Scancode key)
{
	const int index = (s_historyTail + 1) & 0xFF;

	if (index == s_historyHead)
		return false;

	/* Don't worry about extended keys. */
	s_history[s_historyTail] = (Scancode)(key & ~SCANCODE_EXTENDED);
	s_historyTail = index;
	return true;
}

void Input_EventHandler(Scancode key)
{
	const int idx = ((key & 0x7F) >> 3);
	const int bit = (1 << (key & 0x7));

	if (key == 0)
		return;

	if (key & SCANCODE_RELEASE)
	{
		s_activeInputMap[idx] &= ~bit;
	}
	else
	{
		s_activeInputMap[idx] |= bit;
	}

	Input_AddHistory(key);
}

bool Input_Test(Scancode key)
{
	const int idx = ((key & 0x7F) >> 3);
	const int bit = (1 << (key & 0x7));

	return (s_activeInputMap[idx] & bit);
}

bool Input_IsInputAvailable()
{
	return s_historyHead ^ s_historyTail;
}

Scancode Input_PeekNextKey()
{
	if (Input_IsInputAvailable())
	{
		return s_history[s_historyHead];
	}
	else
	{
		return SCANCODE_NONE;
	}
}

Scancode Input_GetNextKey()
{
	const Scancode key = s_history[s_historyHead];
	assert(Input_IsInputAvailable());

	s_historyHead = (s_historyHead + 1) & 0xFF;
	return key;
}
