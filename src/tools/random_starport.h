/** @file src/tools/random_starport.h */

#ifndef TOOLS_RANDOM_STARPORT_H
#define TOOLS_RANDOM_STARPORT_H

#include <inttypes.h>
#include "enumeration.h"
#include "types.h"

extern int64_t Random_Starport_GetSeedTime();
extern uint16 Random_Starport_GetSeed(uint16 scenarioID, HouseType houseID);
void Random_Starport_Reseed();
void Random_Starport_Seed(uint16 seed);
extern uint16 Random_Starport_CalculatePrice(uint16 credits);
extern uint16 Random_Starport_CalculateUnitPrice(UnitType unitType);

#endif
