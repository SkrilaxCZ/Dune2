#ifndef COMMON_A5_H
#define COMMON_A5_H

#include <stdbool.h>
#include "gfx.h"

extern struct ALLEGRO_EVENT_QUEUE* g_a5_input_queue;

void A5_InitTransform(bool screen_size_changed);
extern bool A5_InitOptions();
extern bool A5_Init();
void A5_Uninit();
void A5_UseTransform(ScreenDivID div);
extern ScreenDivID A5_SaveTransform();

#endif
