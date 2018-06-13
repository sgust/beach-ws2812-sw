/* C Implementation of SysTick timer functions */

#include "stm32f10x.h"
#include "common.h"
#include "systick.h"

const int HZ = 1000U; /* standard for SysTick is 1ms resolution */

volatile unsigned int delaytick = 0;

/* Initialize SysTick */
void systicktimer_init(void)
{
	delaytick = 0;
	SysTick->LOAD = SYSCLK / HZ - 1;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

/* sleep for ms milliseconds */
void systicktimer_sleepms(unsigned int ms)
{
	delaytick = ms;
	while (delaytick) {}
}

/* interface for interrupt handler */
void __attribute__ ((interrupt)) SysTick_Handler(void)
{
	if (delaytick) delaytick--;
}
