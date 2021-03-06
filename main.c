/* Control software for beach scene done with 173 WS2812 LEDs */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

#define TIME_WAVE (HZ/2)
#define TIME_FRISBEE_COLOR (HZ/20)
#define TIME_FRISBEE_MOVE (HZ/3)

#define TIME_NEVER 0xffffffffffffffff

uint64_t timer_wave; /* timer for wave animation */
uint64_t timer_color; /* timer for frisbee color animation */
uint64_t timer_fbee; /* timer for frisbee movement animation */

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
	uart->CR1 |= USART_CR1_RE;
	uart->BRR = SYSCLK / baud;
}

int uart_getc(USART_TypeDef *uart)
{
	int sr;
	uint8_t data;

	sr = uart->SR;
	if (sr & USART_SR_RXNE) {
		data = (uint8_t) uart->DR;
		if ((sr & USART_SR_FE) && (0 == data)) return -2; /* BREAK */
		return (uint8_t) data;
	} else return -1; /* no data */
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

/* inserts a \0 at the first whitespace
 *   buf  pointer to string, must be \0 terminated!
 *   size size of buf
 * Return
 *   pointer to the next non-whitespace or NULL if error
 */
char *parse_word(char *buf)
{
	char *p;

	for(p = buf; *p; p++) {
		if (isspace((int) *p)) {
			*p = 0;
			for(p++; *p; p++) {
				if (!isspace((int) *p)) break;
			}
			break;
		}
	}
	return p;
}

void cmd_led(char *s);
void cmd_wave(char *s);
void cmd_waveanim(char *s);
void cmd_person(char *s);
void cmd_newfrisbee(char *s);
void cmd_frisbeeanim(char *s);
void cmd_startanim(char *s);
void cmd_stopanim(char *s);
void cmd_test(char *s);
void cmd_state(char *s);

/* handle debug console commands */
void debugcommand(char *s)
{
	char *para;

	para = parse_word(s);
	printf("\r\n");
	if (!strcmp(s, "led")) cmd_led(para);
	else if (!strcmp(s, "wave")) cmd_wave(para);
	else if (!strcmp(s, "waveanim")) cmd_waveanim(para);
	else if (!strcmp(s, "person")) cmd_person(para);
	else if (!strcmp(s, "newfrisbee")) cmd_newfrisbee(para);
	else if (!strcmp(s, "frisbeeanim")) cmd_frisbeeanim(para);
	else if (!strcmp(s, "startanim")) cmd_startanim(para);
	else if (!strcmp(s, "stopanim")) cmd_stopanim(para);
	else if (!strcmp(s, "test")) cmd_test(para);
	else if (!strcmp(s, "state")) cmd_state(para);
	else printf("ERROR: unknown command\r\n");
}

/* led <num> <r> <g> <b> */
void cmd_led(char *s)
{
	char *r, *g, *b;
	int led;

	r = parse_word(s);
	g = parse_word(r);
	b = parse_word(g);

	led = strtol(s, NULL, 0);
	if ((led < 0) || (led >= NUMLEDS)) {
		printf("ERROR: LED out of range\r\n");
		return;
	}
	screen[led].red = strtol(r, NULL, 0);
	screen[led].green = strtol(g, NULL, 0);
	screen[led].blue = strtol(b, NULL, 0);
	rgbled_update(screen, NUMLEDS);
}

/* wave <num> <r> <g> <b> */
void cmd_wave(char *s)
{
	char *r, *g, *b;
	int wave;
	Pixel pix;

	r = parse_word(s);
	g = parse_word(r);
	b = parse_word(g);

	wave = strtol(s, NULL, 0);
	if ((wave < 0) || (wave > 6)) {
		printf("ERROR: wave out of range\r\n");
		return;
	}
	pix.red = strtol(r, NULL, 0);
	pix.green = strtol(g, NULL, 0);
	pix.blue = strtol(b, NULL, 0);
	setwave(screen, wave, &pix);
	rgbled_update(screen, NUMLEDS);
}

/* waveanim <step> */
void cmd_waveanim(char *s)
{
	int i;

	i = strtol(s, NULL, 0);
	if ((i >= 0) && (i <= 14)) anim_wave_state = i;
	animate_wave(screen);
	rgbled_update(screen, NUMLEDS);
}

/* person <p> <shirt> <pants> <left> <right> */
void cmd_person(char *s)
{
	char *r1, *r2, *g1, *g2, *b1, *b2, *lh, *rh;
	Pixel shirt, pants;
	int person, left, right;

	r1 = parse_word(s);
	g1 = parse_word(r1);
	b1 = parse_word(g1);
	r2 = parse_word(b1);
	g2 = parse_word(r2);
	b2 = parse_word(g2);
	lh = parse_word(b2);
	rh = parse_word(lh);
	person = strtol(s, NULL, 0);
	shirt.red = strtol(r1, NULL, 0);
	shirt.green = strtol(g1, NULL, 0);
	shirt.blue = strtol(b1, NULL, 0);
	pants.red = strtol(r2, NULL, 0);
	pants.green = strtol(g2, NULL, 0);
	pants.blue = strtol(b2, NULL, 0);
	left = strtol(lh, NULL, 0);
	right = strtol(rh, NULL, 0);
	setperson(screen, person, &shirt, &pants, left, right);
	rgbled_update(screen, NUMLEDS);
}

/* newfrisbee <p> */
void cmd_newfrisbee(char *s)
{
	uint8_t person;

	person = strtol(s, NULL, 0);
	anim_fbeepers = animate_newfrisbee(screen, person);
	printf("%d will catch from %d\r\n", anim_fbeepers, person);
	rgbled_update(screen, NUMLEDS);
}

/* frisbeeanim */
void cmd_frisbeeanim(char *s)
{
	uint8_t oldpos;

	oldpos = anim_fbeepos;
	if (animate_frisbee(screen)) {
		rgbled_update(screen, NUMLEDS);
	}
	if (oldpos == anim_fbeepos) printf("END OF ANIMATION\r\n");
}

/* startanim <anim> */
void cmd_startanim(char *s)
{
	int anim;

	anim = strtol(s, NULL, 0);
	if (1 == anim) timer_wave = systicktimer_time();
	if (2 == anim) timer_color = systicktimer_time();
	if (3 == anim) timer_fbee = systicktimer_time();
}

/* stopanim <anim> */
void cmd_stopanim(char *s)
{
	int anim;

	anim = strtol(s, NULL, 0);
	if (1 == anim) timer_wave = TIME_NEVER;
	if (2 == anim) timer_color = TIME_NEVER;
	if (3 == anim) timer_fbee = TIME_NEVER;
}

/* test <r> <g> <b> */
void cmd_test(char *r)
{
	char *g, *b;
	int i;

	g = parse_word(r);
	b = parse_word(g);

	for(i = 0; i < NUMLEDS; i++) {
		screen[i].red = strtol(r, NULL, 0);
		screen[i].green = strtol(g, NULL, 0);
		screen[i].blue = strtol(b, NULL, 0);
	}
	rgbled_update(screen, NUMLEDS);
}

/* state */
void cmd_state(char *s)
{
	printf("anim_fbeepers %d\r\n", anim_fbeepers);
	printf("anim_fbeepath %08x\r\n", anim_fbeepath);
	printf("anim_fbeepos %d\r\n", anim_fbeepos);
	printf("anim_wave_state %d\r\n", anim_wave_state);
}

int main(void)
{
	int i, j, c, p;
	char inputbuf[80];
	int jumper = 0;

	fullspeed();

	__disable_irq();
	rgbled_init();
	systicktimer_init();
	__enable_irq();

	/* USART1 on PA9/PA10 */
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_USART1EN;
	/* check for jumper on RX to GND */
	GPIOA->BSRR = (1 << 10); /* pull-up */
	GPIOA->CRH &= ~(0xf << 8);
	GPIOA->CRH |= 0x8 << 8; /* PA10 RX, input pull */
	systicktimer_sleepms(10); /* let input settle */
	if (0 == (GPIOA->IDR & (1 << 10))) jumper = 1;
	/* switch I/O to USART */
	GPIOA->BSRR = (1 << 26); /* pull-up off */
	GPIOA->CRH &= ~((0xf << 4) | (0xf << 8));
	GPIOA->CRH |= 0xb << 4; /* PA9 TX, alt function push-pull */
	GPIOA->CRH |= 0x4 << 8; /* PA10 RX, input */
	init_uart(USART1, 115200);

	printf("\r\nBeach scene with WS2812 RGB LEDs V1.0\r\n");

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRL=0x44444333;

	/* clear screen */
	for(i = 0; i < NUMLEDS; i++) {
		screen[i].red = screen[i].green = screen[i].blue = 0;
	}
	rgbled_update(screen, NUMLEDS);

	systicktimer_sleepms(1000);

	/* initial animation */
	setwave(screen, 0, &Pix_sand);
	setwave(screen, 1, &Pix_sand);
	setwave(screen, 2, &Pix_sand);
	setwave(screen, 3, &Pix_sand);
	setwave(screen, 4, &Pix_sand);
	setwave(screen, 5, &Pix_sand);
	setwave(screen, 6, &Pix_sand);

	/* person 0 holding frisbee in left hand up */
	anim_fbeepos = L404;
	anim_fbeepers = 0;

	for(i = 0; i < 4; i++) {
		Pixel *pix1, *pix2;

		pix1 = pix2 = &Clothes[rand()%(sizeof(Clothes)/sizeof(Pixel))];
		while (pix1 == pix2) {
			pix2 = &Clothes[rand()%(sizeof(Clothes)/sizeof(Pixel))];
		}
		/* person 0 left hand up (holds initial frisbee) */
		if (0 == i) {
			setperson(screen, i, pix1, pix2, 1, rand()%2);
		} else {
			setperson(screen, i, pix1, pix2, rand()%2, rand()%2);
		}
	}

	rgbled_update(screen, NUMLEDS);

	p = 0;
	printf("LED> ");
	fflush(stdout);
	timer_wave = systicktimer_time() + TIME_WAVE;
	timer_color = systicktimer_time() + TIME_FRISBEE_COLOR;
	timer_fbee = systicktimer_time() + TIME_FRISBEE_MOVE;
	if (jumper) {
		/* jumper on RX and GND */
		char s[10];
		cmd_stopanim("1");
		cmd_stopanim("2");
		cmd_stopanim("3");
		strcpy(s, "10 10 10");
		cmd_test(s);
	}
	while (1) {
		c = uart_getc(USART1);
		if (c > 0) {
			if ((0x0a == c) || (0x0d == c)) {
				inputbuf[p] = 0;
				debugcommand(inputbuf);
				p = 0;
				printf("LED> ");
			} else {
				if ((8 == c) || (127 == c)) {
					if (p > 0) {
						p--;
						printf("\010 \010");
					} else {
						printf("\007");
					}
				} else {
					if ((c >= 32) && (c <= 126)) {
						inputbuf[p++] = c;
						printf("%c", c);
					}
				}
			}
		}
		fflush(stdout);
		/* time for wave animation */
		if (systicktimer_time() > timer_wave) {
			timer_wave = systicktimer_time() + TIME_WAVE;
			animate_wave(screen);
			rgbled_update(screen, NUMLEDS);
			/* trigger return of missed frisbees */
			if ((PERSON_LOST == anim_fbeepers) && (NULL == anim_fbeepath)) {
				if ((L204 == anim_fbeepos) && (3 == anim_wave_state)) {
					anim_fbeepers = animate_newfrisbee(screen, PERSON_BACK_T);
				}
				if ((L211 == anim_fbeepos) && (4 == anim_wave_state)) {
					anim_fbeepers = animate_newfrisbee(screen, PERSON_BACK_M);
				}
				if ((L224 == anim_fbeepos) && (5 == anim_wave_state)) {
					anim_fbeepers = animate_newfrisbee(screen, PERSON_BACK_B);
				}
			}
			/* start new frisbee after return finished */
			if ((PERSON_BACK_T == anim_fbeepers) && (NULL == anim_fbeepath)) {
				anim_fbeepers = animate_newfrisbee(screen, 0);
			}
			if ((PERSON_BACK_M == anim_fbeepers) && (NULL == anim_fbeepath)) {
				anim_fbeepers = animate_newfrisbee(screen, 2);
			}
			if ((PERSON_BACK_B == anim_fbeepers) && (NULL == anim_fbeepath)) {
				anim_fbeepers = animate_newfrisbee(screen, 2);
			}
			/* return frisbee animation time-locked to wave */
			if ((PERSON_BACK_T == anim_fbeepers) || (PERSON_BACK_M == anim_fbeepers) || (PERSON_BACK_B == anim_fbeepers)) {
				if (animate_frisbee(screen)) {
					rgbled_update(screen, NUMLEDS);
				}
			}
		}
		/* time for frisbee color animation */
		if (systicktimer_time() > timer_color) {
			Pixel pix;

			timer_color = systicktimer_time() + TIME_FRISBEE_COLOR;
			pix.red = (rand() & 0x0f) << 4;
			pix.green = (rand() & 0x0f) << 4;
			pix.blue = (rand() & 0x0f) << 4;
			if (255 != anim_fbeepos) {
				setpixel(&screen[anim_fbeepos], &pix);
			}
			rgbled_update(screen, NUMLEDS);
		}
		/* time for frisbee movement animation */
		if ((systicktimer_time() > timer_fbee) && (anim_fbeepers <= 4)) {
			timer_fbee = systicktimer_time() + TIME_FRISBEE_MOVE;
			if (NULL == anim_fbeepath) {
				anim_fbeepers = animate_newfrisbee(screen, anim_fbeepers);
				rgbled_update(screen, NUMLEDS);
			}
			else if (animate_frisbee(screen)) {
				rgbled_update(screen, NUMLEDS);
			}
		}
	}

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
		systicktimer_sleepms(10);
	}

	return 0;
}
