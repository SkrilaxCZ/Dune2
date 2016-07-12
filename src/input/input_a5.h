#ifndef INPUT_INPUTA5_H
#define INPUT_INPUTA5_H

union ALLEGRO_EVENT;

extern bool InputA5_Init();
void InputA5_Uninit();
extern bool InputA5_ProcessEvent(union ALLEGRO_EVENT* event, bool apply_mouse_transform);
extern bool InputA5_Tick(bool apply_mouse_transform);

#endif
