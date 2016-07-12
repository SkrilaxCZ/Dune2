/** @file src/os/sleep.h OS-independent inclusion of the delay routine. */

#ifndef OS_SLEEP_H
#define OS_SLEEP_H

#include <windows.h>
#define sleep(x) Sleep(x * 1000)
#define msleep(x) Sleep(x)

#define sleepIdle() msleep(1)

#endif /* OS_SLEEP_H */
