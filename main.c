/* Control software for beach scene done with 173 WS2812 LEDs */

#include <stdio.h>

#include "stm32f10x.h"
#include "common.h"
#include "systick.h"
#include "ws2812b.h"
#include "beach.h"

#ifdef __cplusplus
extern "C" int _write (int fd, char *ptr, int len);
#endif 

#define NUMLEDS 173
Pixel screen[NUMLEDS];

int _write (int fd, char *ptr, int len)
{
	int i;

	for(i = 0; i < len; i++) {
		while (!(USART1->SR & USART_SR_TXE)) {}
		USART1->DR = ptr[i];
	}
	return len;
}

void init_uart(USART_TypeDef *uart, unsigned int baud)
{
	uart->CR1 |= USART_CR1_UE;
	uart->CR1 &= ~USART_CR1_M; /* 8 bit */
	uart->CR2 &= ~(USART_CR2_STOP_0 | USART_CR2_STOP_1); /* 1 stop bit */
	uart->CR1 |= USART_CR1_TE;
	uart->BRR = SYSCLK / baud;
}

/* 8 MHz HSE with PLL to 72MHz */
void fullspeed()
{
	/* enable prefetch, 2 wait states */
	FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_1 | FLASH_ACR_PRFTBE | FLASH_ACR_PRFTBS;
	/* start HSE */
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY)) {}
	/* setup PLL */
	RCC->CR &= ~RCC_CR_PLLON;
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLMULL) | RCC_CFGR_PLLMULL9; /* 72MHz */
	RCC->CFGR &= ~RCC_CFGR_PLLXTPRE; /* no pre divider */
	RCC->CFGR |= RCC_CFGR_PLLSRC; /* HSE to PLL */
	RCC->CFGR &= ~RCC_CFGR_USBPRE; /* div 1.5 for 48Mhz USB */
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | RCC_CFGR_PPRE2_DIV1; /* no div for 72MHz APB2 */
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | RCC_CFGR_PPRE1_DIV2; /* div 2 for 36MHz APB1 */
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | RCC_CFGR_HPRE_DIV1; /* no div for 72MHz AHB */
	/* start PLL */
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY)) {}
	/* switch to PLL clock */
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
}

int main(void)
{
	int i, j;

	fullspeed();

	/* USART1 on PA9/PA10 */
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_USART1EN;
	GPIOA->CRH &= ~((0xf << 4) | (0xf << 8));
	GPIOA->CRH |= 0xb << 4; /* PA9 TX, alt function push-pull */
	GPIOA->CRH |= 0x4 << 8; /* PA10 RX, input */
	init_uart(USART1, 115200);

	printf("\r\nHello\r\n");

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRL=0x44444333;

	__disable_irq();
	rgbled_init();
	systicktimer_init();
	__enable_irq();

	printf("IRQ on\r\n");

	/* clear screen */
	for(i = 0; i < NUMLEDS; i++) {
		screen[i].red = screen[i].green = screen[i].blue = 0;
	}
	if (rgbled_update(screen, NUMLEDS)) printf("CLS failed");
	rgbled_vsync();

	setwave(screen, 0, &pix_water);
	setwave(screen, 1, &pix_sand);

	if (rgbled_update(screen, NUMLEDS)) printf("Test1 failed");
	rgbled_vsync();

	systicktimer_sleepms(1000);

	if (rgbled_update(screen, NUMLEDS)) printf("Test2 failed");
	rgbled_vsync();

	systicktimer_sleepms(10000);

	j = 0;
	while (1) {
		for(i = 0; i < NUMLEDS; i++) {
			screen[i].red = 0x10;
			screen[i].green = 0x10;
			screen[i].blue = 0x10;
		}
		screen[j].red = 255;
		screen[j].green = 0;
		screen[j].blue = 0;
		screen[(j+1)%NUMLEDS].red = 0;
		screen[(j+1)%NUMLEDS].green = 255;
		screen[(j+1)%NUMLEDS].blue = 0;
		screen[(j+2)%NUMLEDS].red = 0;
		screen[(j+2)%NUMLEDS].green = 0;
		screen[(j+2)%NUMLEDS].blue = 255;
		screen[(j+3)%NUMLEDS].red = 255;
		screen[(j+3)%NUMLEDS].green = 255;
		screen[(j+3)%NUMLEDS].blue = 255;
		if (++j >= NUMLEDS) j = 0;
		rgbled_update(screen, NUMLEDS);
		rgbled_vsync();
		systicktimer_sleepms(10);
	}

	return 0;
}
