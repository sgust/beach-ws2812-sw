/* control a string of WS2812B RGB LEDs */

#include <stdio.h>

#include "stm32f10x.h"
#include "common.h"
#include "ws2812b.h"

// initialise SPI
void rgbled_init(void) {
	// TIMER PWM on PA1
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->CRL &= ~(0x0f << 4);
	GPIOA->CRL |= 0xb << 4; // alt function push-pull on PA1
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	// the APB1 clock is 36MHz, but since the prescaler is 2 the timer clock is 72MHz
	TIM2->PSC = 0; // prescaler 0, run timer with full 72MHz
	TIM2->ARR = 89; // 1.25µs bitlength
	TIM2->CCR2 = 0; // set output low
	// PWM mode 1, see p. 308 of Reference Manual Rev 15
	TIM2->CCMR1 = (TIM2->CCMR1 & ~(TIM_CCMR1_OC2M)) | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
	TIM2->CCMR1 |= TIM_CCMR1_OC2PE;
	TIM2->CCER |= TIM_CCER_CC2E;
	TIM2->CR1 |= TIM_CR1_ARPE;
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
}

void rgbled_update(Pixel *scr, int len)
{
	int i;
	int8_t bit = 0;		// need signed for countdown to 0!

	__disable_irq();
	// >50us needed, doing 100us
	TIM2->CCR2 = 0;
	for (i = 0; i < 80; i++) {
		while (!(TIM2->SR & TIM_SR_UIF)) {}
		TIM2->SR &= ~(TIM_SR_UIF);
		TIM2->CCR2 = 0;
	}
	// we need to send a bit every 1.25µs, the irq has too much latency
	// to be called for every bit, so we send all bits in one call
	for (i = 0; i < len; i++) {
		for(bit = 7; bit >= 0; bit--) {
			// wait for end of previous bit
			while (!(TIM2->SR & TIM_SR_UIF)) {}
			if (scr[i].green & (1 << bit))
				TIM2->CCR2 = 57;
			else
				TIM2->CCR2 = 28;
			// clear overflow
			TIM2->SR &= ~(TIM_SR_UIF);
		}
		for(bit = 7; bit >= 0; bit--) {
			// wait for end of previous bit
			while (!(TIM2->SR & TIM_SR_UIF)) {}
			if (scr[i].red & (1 << bit))
				TIM2->CCR2 = 57;
			else
				TIM2->CCR2 = 28;
			// clear overflow
			TIM2->SR &= ~(TIM_SR_UIF);
		}
		for(bit = 7; bit >= 0; bit--) {
			// wait for end of previous bit
			while (!(TIM2->SR & TIM_SR_UIF)) {}
			if (scr[i].blue & (1 << bit))
				TIM2->CCR2 = 57;
			else
				TIM2->CCR2 = 28;
			// clear overflow
			TIM2->SR &= ~(TIM_SR_UIF);
		}
	}
	// wait for end of last bit
	while (!(TIM2->SR & TIM_SR_UIF)) {}
	// finished: set data low
	TIM2->CCR2 = 0;
	__enable_irq();
}
