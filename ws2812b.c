/* control a string of WS2812B RGB LEDs */

#include <stdio.h>

#include "stm32f10x.h"
#include "common.h"
#include "ws2812b.h"

static Pixel *screen = NULL;
static volatile uint8_t screen_update = 0;
static unsigned int screen_size = 0;
static int8_t bit = 0;		// need signed for countdown to 0!
static uint8_t reset = 0;
static uint8_t pixel = 0;
static uint8_t color = 0;

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
	// prepare interrupt
	NVIC_SetPriority(TIM2_IRQn, 0);
	NVIC_EnableIRQ(TIM2_IRQn);
	// init data
	screen_update = 0;
	screen_size = 0;
	reset = 0;
	pixel = 0;
	color = 0;
	bit = 7;
}

// wait until all data is sent
void rgbled_vsync(void)
{
	while (screen_update) {}
}

int rgbled_update(Pixel *scr, int len)
{
	if (screen_update) return -1; // still sending data, refuse new data
	screen_size = len;
	screen_update = 1;
	// copy screen data reference
	screen = scr;
	// >50us needed, doing 100us
	reset = 80;
	TIM2->CCR2 = 0;
	// enable IRQ
	TIM2->DIER = TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM2_IRQn);
	return 0;
}

// interrupt handler, used to update the bits
//FIXME, NEW IDEA: use dma from buffer, refill buffer by irq
void __attribute__ ((interrupt)) TIM2_IRQHandler(void)
{
	unsigned int i;

	TIM2->SR &= ~(TIM_SR_UIF);
	// wait for end of reset signal
	if (reset) {
		reset--;
		return;
	}
	// we need to send a bit every 1.25µs, the irq has too much latency
	// to be called for every bit, so we send all bits in one call
	// disable IRQ
	TIM2->DIER &= ~TIM_DIER_UIE;
	NVIC_DisableIRQ(TIM2_IRQn);
	for (i = 0; i < screen_size; i++) {
		for(bit = 7; bit >= 0; bit--) {
			// wait for end of previous bit
			while (!(TIM2->SR & TIM_SR_UIF)) {}
			if (screen[i].green & (1 << bit))
				TIM2->CCR2 = 57;
			else
				TIM2->CCR2 = 28;
			// clear overflow
			TIM2->SR &= ~(TIM_SR_UIF);
		}
		for(bit = 7; bit >= 0; bit--) {
			// wait for end of previous bit
			while (!(TIM2->SR & TIM_SR_UIF)) {}
			if (screen[i].red & (1 << bit))
				TIM2->CCR2 = 57;
			else
				TIM2->CCR2 = 28;
			// clear overflow
			TIM2->SR &= ~(TIM_SR_UIF);
		}
		for(bit = 7; bit >= 0; bit--) {
			// wait for end of previous bit
			while (!(TIM2->SR & TIM_SR_UIF)) {}
			if (screen[i].blue & (1 << bit))
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
	screen_update = 0;
	return;
}
