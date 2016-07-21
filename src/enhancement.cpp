/** @file src/enhancement.c
 *
 * Enhancements to the original game.  In all cases, true indicates
 * deviation from the original game.
 */

#include "enhancement.h"

/**
 * Various AI changes to make the game tougher.  Includes double
 * production rate, half cost, flanking attacks, etc.
 */
bool enhancement_brutal_ai = false;

/**
 * In the original game, fog is drawn underneath units, at the same
 * time as other overlays, making units suddenly appear and disappear.
 */
bool enhancement_fog_covers_units = true;

/**
 * Add non-permanent scouting.
 */
bool enhancement_fog_of_war = false;
