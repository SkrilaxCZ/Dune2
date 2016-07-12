#ifndef INPUT_INPUT_H
#define INPUT_INPUT_H

#include <stdbool.h>
#include "scancode.h"

/* "Scancodes" for mouse events, stored from 0x7F backwards. */
enum MouseEvent
{
	MOUSE_ZAXIS = 0x7F,
	MOUSE_LMB = 0x7E,
	MOUSE_RMB = 0x7D
};

enum InputMouseMode
{
	INPUT_MOUSE_MODE_NORMAL = 0,
	INPUT_MOUSE_MODE_RECORD = 1,
	INPUT_MOUSE_MODE_PLAY = 2
};

void Input_Init();
void Input_History_Clear();
void Input_EventHandler(Scancode key);
bool Input_Test(Scancode key);
bool Input_IsInputAvailable();
Scancode Input_PeekNextKey();
Scancode Input_GetNextKey();

#include "input_a5.h"

#define Input_Tick  InputA5_Tick

#endif
