/** @file src/enhancement.c
 *
 * Enhancements to the original game.  In all cases, true indicates
 * deviation from the original game.
 */

#include "enhancement.h"

/**
 * In the original game, the AI is allowed to place structures
 * on top of units, and is not penalised for lack of concrete.
 */
bool enhancement_ai_respects_structure_placement = true;

/**
 * Various AI changes to make the game tougher.  Includes double
 * production rate, half cost, flanking attacks, etc.
 */
bool enhancement_brutal_ai = false;

/**
 * In Dune II, construction automatically goes on hold when you run
 * out of funds.  This behaviour does not work well with build queues.
 */
bool enhancement_construction_does_not_pause = true;

/**
 * In the original game, once a unit starts to wobble, it will never
 * stop wobbling.
 */
bool const enhancement_fix_everlasting_unit_wobble = true;


/**
 * In the original game, fog is drawn underneath units, at the same
 * time as other overlays, making units suddenly appear and disappear.
 */
bool enhancement_fog_covers_units = true;

/**
 * Add non-permanent scouting.
 */
bool enhancement_fog_of_war = false;

/**
 * In the Sega Mega Drive version of Dune II, units will not return to
 * guard after catching up to the leader.  They will therefore
 * continue to follow once the leader moves again.  However, they will
 * not attack any enemies that come into range.
 */
bool enhancement_permanent_follow_mode = false;
