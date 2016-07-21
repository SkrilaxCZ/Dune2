#ifndef BRUTAL_AI_H
#define BRUTAL_AI_H

#include <stdio.h>
#include "house.h"
#include "structure.h"
#include "unit.h"

bool AI_IsBrutalAI(HouseType houseID);

uint16 StructureAI_PickNextToBuild(const Structure* s);

bool UnitAI_CallCarryallToEvadeSandworm(const Unit* harvester);
uint16 UnitAI_GetAnyEnemyInRange(const Unit* unit);
bool UnitAI_ShouldDestructDevastator(const Unit* devastator);

void UnitAI_ClearSquads();
void UnitAI_DetachFromSquad(Unit* unit);
void UnitAI_AbortMission(Unit* unit, uint16 enemy);
uint16 UnitAI_GetSquadDestination(Unit* unit, uint16 destination);
void UnitAI_SquadLoop();

bool BrutalAI_Load(FILE* fp, uint32 length);
bool BrutalAI_Save(FILE* fp);

#endif
