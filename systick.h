#ifndef SYSTICK_H
#define SYSTICK_H

#include <ctype.h>

/* C Implementation of SysTick timer functions */

extern const int HZ;

void systicktimer_init(void);
void systicktimer_sleepms(unsigned int ms);
uint64_t systicktimer_time(void);

#endif
