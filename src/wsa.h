/** @file src/wsa.h WSA definitions. */

#ifndef WSA_H
#define WSA_H

const int RADAR_ANIMATION_FRAME_COUNT = 21;
const int RADAR_ANIMATION_DELAY = 3;

uint16 WSA_GetFrameCount(void* wsa);
void* WSA_LoadFile(const char* filename, void* wsa, uint32 wsaSize, bool reserveDisplayFrame);
void WSA_Unload(void* wsa);
bool WSA_DisplayFrame(void* wsa, uint16 frameNext, uint16 posX, uint16 posY, Screen screenID);

#endif /* WSA_H */
