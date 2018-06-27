/* handle the beach scene animation */

#include <stdio.h>
#include <stdlib.h>

#include "stm32f10x.h"
#include "common.h"
#include "ws2812b.h"
#include "beach.h"

#define PROB_MISS 10 /* miss one out of 10 throws */

/* some colors */
const Pixel Pix_off = { 0, 0, 0 };
const Pixel Pix_sand = { 0x80, 0x40, 0x00 };
const Pixel Pix_water = { 0x00, 0x00, 0x80 };
const Pixel Pix_water2 = { 0x20, 0x40, 0x80 };
const Pixel Pix_skin = { 100, 57, 30 };
const Pixel Clothes[10] = { {255,0,0},{0,255,0},{0,0,255},{255,255,0},{255,0,255},{0,255,255},{255,20,147},{171,139,37},{154,205,50},{124,252,0} };

/* set 1 pixel to color col */
void setpixel(Pixel *scr, const Pixel *col)
{
	scr->red = col->red;
	scr->green = col->green;
	scr->blue = col->blue;
}

/* set all LEDs for wave w to color col */
void setwave(Pixel *scr, int w, const Pixel *col)
{
	switch (w) {
		case 0:
			setpixel(&scr[L201], col);
			setpixel(&scr[L202], col);
			setpixel(&scr[L203], col);
			break;
		case 1:
			setpixel(&scr[L205], col);
			setpixel(&scr[L206], col);
			setpixel(&scr[L207], col);
			setpixel(&scr[L212], col);
			setpixel(&scr[L213], col);
			setpixel(&scr[L214], col);
			break;
		case 2:
			setpixel(&scr[L208], col);
			setpixel(&scr[L209], col);
			setpixel(&scr[L210], col);
			setpixel(&scr[L222], col);
			setpixel(&scr[L221], col);
			setpixel(&scr[L220], col);
			setpixel(&scr[L216], col);
			setpixel(&scr[L217], col);
			setpixel(&scr[L219], col);
			break;
		case 3:
			setpixel(&scr[L225], col);
			setpixel(&scr[L226], col);
			setpixel(&scr[L227], col);
			setpixel(&scr[L228], col);
			setpixel(&scr[L230], col);
			setpixel(&scr[L231], col);
			setpixel(&scr[L233], col);
			setpixel(&scr[L234], col);
			setpixel(&scr[L235], col);
			setpixel(&scr[L236], col);
			setpixel(&scr[L238], col);
			setpixel(&scr[L239], col);
			break;
		case 4:
			setpixel(&scr[L320], col);
			setpixel(&scr[L319], col);
			setpixel(&scr[L318], col);
			setpixel(&scr[L317], col);
			setpixel(&scr[L316], col);
			setpixel(&scr[L315], col);
			setpixel(&scr[L314], col);
			setpixel(&scr[L313], col);
			setpixel(&scr[L311], col);
			setpixel(&scr[L310], col);
			setpixel(&scr[L309], col);
			setpixel(&scr[L307], col);
			setpixel(&scr[L306], col);
			setpixel(&scr[L305], col);
			setpixel(&scr[L304], col);
			setpixel(&scr[L303], col);
			setpixel(&scr[L301], col);
			setpixel(&scr[L240], col);
			break;
		case 5:
			setpixel(&scr[L321], col);
			setpixel(&scr[L322], col);
			setpixel(&scr[L323], col);
			setpixel(&scr[L324], col);
			setpixel(&scr[L325], col);
			setpixel(&scr[L326], col);
			setpixel(&scr[L328], col);
			setpixel(&scr[L329], col);
			setpixel(&scr[L330], col);
			setpixel(&scr[L331], col);
			setpixel(&scr[L332], col);
			setpixel(&scr[L333], col);
			setpixel(&scr[L334], col);
			setpixel(&scr[L335], col);
			setpixel(&scr[L336], col);
			setpixel(&scr[L338], col);
			setpixel(&scr[L339], col);
			break;
		case 6:
			setpixel(&scr[L419], col);
			setpixel(&scr[L418], col);
			setpixel(&scr[L417], col);
			setpixel(&scr[L416], col);
			setpixel(&scr[L412], col);
			setpixel(&scr[L411], col);
			setpixel(&scr[L409], col);
			setpixel(&scr[L408], col);
			setpixel(&scr[L407], col);
			setpixel(&scr[L406], col);
			setpixel(&scr[L405], col);
			setpixel(&scr[L403], col);
			setpixel(&scr[L402], col);
			setpixel(&scr[L340], col);
			break;
	}
}

int anim_wave_state = 0;

/* advance wave animate 1 step */
void animate_wave(Pixel *scr)
{
	int i;

	if (anim_wave_state <= 7) {
		for (i = 0; i <= 6; i++) {
			if (i < anim_wave_state) {
				setwave(scr, i, &Pix_water);
			} else if (i == anim_wave_state) {
				setwave(scr, i, &Pix_water2);
			} else {
				setwave(scr, i, &Pix_sand);
			}
		}
	} else {
		for (i = 6; i >= 0; i--) {
			if (i < (14-anim_wave_state)) {
				setwave(scr, i, &Pix_water);
			} else if (i == (14-anim_wave_state)) {
				setwave(scr, i, &Pix_water2);
			} else {
				setwave(scr, i, &Pix_sand);
			}
		}
	}
	if (++anim_wave_state > 15) anim_wave_state = 0;
}

/* person info */
typedef struct {
	Pixel shirt, pants;
	uint8_t left, right;
} person_t;

person_t persons[4];

/* set person p shirt and pants color plus control left and right hand (viewer left/right) */
void setperson(Pixel *scr, int p, Pixel *shirt, Pixel *pants, int left, int right)
{
	/* store info */
	if (p <= 3) {
		persons[p].shirt.red = shirt->red;
		persons[p].shirt.green = shirt->green;
		persons[p].shirt.blue = shirt->blue;
		persons[p].pants.red = pants->red;
		persons[p].pants.green = pants->green;
		persons[p].pants.blue = pants->blue;
		persons[p].left = left;
		persons[p].right = right;
	}
	switch(p) {
		case 0: /* top left */
			setpixel(&scr[L429], shirt);
			setpixel(&scr[L430], shirt);
			setpixel(&scr[L435], shirt);
			setpixel(&scr[L436], pants);
			setpixel(&scr[L437], pants);
			setpixel(&scr[L515], pants);
			setpixel(&scr[L431], &Pix_skin); /* head */
			setpixel(&scr[L438], &Pix_skin); /* left foot */
			setpixel(&scr[L512], &Pix_skin); /* right foot */
			if (left) {
				setpixel(&scr[L404], &Pix_skin);
				setpixel(&scr[L428], &Pix_off);
			} else {
				setpixel(&scr[L404], &Pix_off);
				setpixel(&scr[L428], &Pix_skin);
			}
			if (right) {
				setpixel(&scr[L517], &Pix_skin);
				setpixel(&scr[L516], &Pix_off);
			} else {
				setpixel(&scr[L517], &Pix_off);
				setpixel(&scr[L516], &Pix_skin);
			}
			break;
		case 1: /* top right */
			setpixel(&scr[L519], shirt);
			setpixel(&scr[L520], shirt);
			setpixel(&scr[L522], shirt);
			setpixel(&scr[L525], pants);
			setpixel(&scr[L528], pants);
			setpixel(&scr[L613], pants);
			setpixel(&scr[L521], &Pix_skin); /* head */
			setpixel(&scr[L530], &Pix_skin); /* left foot */
			setpixel(&scr[L611], &Pix_skin); /* right foot */
			if (left) {
				setpixel(&scr[L518], &Pix_skin);
				setpixel(&scr[L526], &Pix_off);
			} else {
				setpixel(&scr[L518], &Pix_off);
				setpixel(&scr[L526], &Pix_skin);
			}
			if (right) {
				setpixel(&scr[L523], &Pix_skin);
				setpixel(&scr[L524], &Pix_off);
			} else {
				setpixel(&scr[L523], &Pix_off);
				setpixel(&scr[L524], &Pix_skin);
			}
			break;
		case 2: /* bottom left */
			setpixel(&scr[L501], shirt);
			setpixel(&scr[L440], shirt);
			setpixel(&scr[L508], shirt);
			setpixel(&scr[L503], pants);
			setpixel(&scr[L421], pants);
			setpixel(&scr[L504], pants);
			setpixel(&scr[L439], &Pix_skin); /* head */
			setpixel(&scr[L420], &Pix_skin); /* left foot */
			setpixel(&scr[L505], &Pix_skin); /* right foot */
			if (left) {
				setpixel(&scr[L423], &Pix_skin);
				setpixel(&scr[L422], &Pix_off);
			} else {
				setpixel(&scr[L423], &Pix_off);
				setpixel(&scr[L422], &Pix_skin);
			}
			if (right) {
				setpixel(&scr[L510], &Pix_skin);
				setpixel(&scr[L506], &Pix_off);
			} else {
				setpixel(&scr[L510], &Pix_off);
				setpixel(&scr[L506], &Pix_skin);
			}
			break;
		case 3: /* bottom right */
			setpixel(&scr[L601], shirt);
			setpixel(&scr[L602], shirt);
			setpixel(&scr[L603], shirt);
			setpixel(&scr[L536], pants);
			setpixel(&scr[L540], pants);
			setpixel(&scr[L539], pants);
			setpixel(&scr[L608], &Pix_skin); /* head */
			setpixel(&scr[L537], &Pix_skin); /* left foot */
			setpixel(&scr[L538], &Pix_skin); /* right foot */
			if (left) {
				setpixel(&scr[L534], &Pix_skin);
				setpixel(&scr[L535], &Pix_off);
			} else {
				setpixel(&scr[L534], &Pix_off);
				setpixel(&scr[L535], &Pix_skin);
			}
			if (right) {
				setpixel(&scr[L605], &Pix_skin);
				setpixel(&scr[L604], &Pix_off);
			} else {
				setpixel(&scr[L605], &Pix_off);
				setpixel(&scr[L604], &Pix_skin);
			}
			break;
	}
}

/* control bits for person's hands in frisbee animation */
#define PERS_0 0x0000
#define PERS_1 0x0100
#define PERS_2 0x0200
#define PERS_3 0x0300
#define HAND_R 0x0400
#define HAND_L 0x0800
#define HAND_T 0x1000
#define HAND_B 0x0000 /* dummy, bottom position is really !HAND_T, but increases readability */
#define PERS_X 0x2000 /* redraw marker */

const uint16_t path_0_2_1[] = { PERS_0 | HAND_R | HAND_T | L517, PERS_0 | HAND_R | HAND_B | L516, PERS_X | L514, PERS_2 | HAND_R | HAND_T | L513, L511, L510, 255 };
const uint16_t path_0_2_2[] = { PERS_0 | HAND_L | HAND_T | L404, PERS_0 | HAND_L | HAND_B | L428, PERS_X | L427, PERS_2 | HAND_L | HAND_T | L426, L425, L424, L423, 255 };
const uint16_t path_0_mis[] = { PERS_X | L337, L302, L237, L218, L215, L204, 255 };
const uint16_t path_2_0_1[] = { PERS_2 | HAND_R | HAND_B | L506, PERS_2 | HAND_R | HAND_T | L510, PERS_X | L511, PERS_0 | HAND_R | HAND_T | L513, L514, L516, L517, 255 };
const uint16_t path_2_0_2[] = { PERS_2 | HAND_L | HAND_B | L422, PERS_2 | HAND_L | HAND_T | L423, PERS_X | L424, PERS_0 | HAND_L | HAND_B | L425, L426, L427, L428, 255 };
const uint16_t path_2_mis[] = { PERS_X | L410, L308, L232, L223, L211, 255 };

uint8_t anim_fbeepos = 255;
const uint16_t *anim_fbeepath;
uint8_t anim_fbeeidx = 0;

/* prepare to throw a frisbee from person p, return catching person */
uint8_t animate_newfrisbee(Pixel *scr, uint8_t p)
{
	uint8_t catcher;
	int r;

	anim_fbeepath = NULL;
	switch(p) {
		case 0:
			if (0 == (rand() % PROB_MISS)) {
				anim_fbeepath = path_0_mis;
				catcher = 4;
			} else {
				//FIXME: different catchers
				r = rand() % 2;
				if (r == 0) {
					anim_fbeepath = path_0_2_1;
					catcher = 2;
				} else if (r == 1) {
					anim_fbeepath = path_0_2_2;
					catcher = 2;
				}
			}
			break;
		case 2:
			if (0 == (rand() % PROB_MISS)) {
				anim_fbeepath = path_2_mis;
				catcher = 4;
			} else {
				//FIXME: different catchers
				r = rand() % 2;
				if (r == 0) {
					anim_fbeepath = path_2_0_1;
					catcher = 0;
				} else if (r == 1) {
					anim_fbeepath = path_2_0_2;
					catcher = 0;
				}
			}
			break;
	}
	if (anim_fbeepath) {
		anim_fbeeidx = 0;
		anim_fbeepos = 255; /* marker for fresh frisbee */
		(void) animate_frisbee(scr);
		return catcher;
	}
	else return 255;
}

/* animate frisbee 1 frame, return 1 when screen refresh needed */
int animate_frisbee(Pixel *scr)
{
	uint16_t newpos;
	int p;
	int need_update = 0;

	if (NULL == anim_fbeepath) return 0;
	if (255 != anim_fbeepos) {
		/* clear old frisbee position */
		setpixel(&scr[anim_fbeepos], &Pix_off);
		anim_fbeeidx++; /* next in path */
	}
	newpos = anim_fbeepath[anim_fbeeidx];
	if (newpos & (HAND_R | HAND_L)) {
		/* do hand movements */
		p = (newpos >> 8) & 3;
		if (newpos & HAND_R) {
			if (newpos & HAND_T) {
				setperson(scr, p, &persons[p].shirt, &persons[p].pants, persons[p].left, 1);
			} else {
				setperson(scr, p, &persons[p].shirt, &persons[p].pants, persons[p].left, 0);
			}
		}
		if (newpos & HAND_L) {
			if (newpos & HAND_T) {
				setperson(scr, p, &persons[p].shirt, &persons[p].pants, 1, persons[p].right);
			} else {
				setperson(scr, p, &persons[p].shirt, &persons[p].pants, 0, persons[p].right);
			}
		}
		need_update = 1;
	}
	if (newpos & PERS_X) {
		/* redraw all persons */
		for(p = 0; p < 4; p++) {
			setperson(scr, p, &persons[p].shirt, &persons[p].pants, persons[p].left, persons[p].right);
		}
	}
	if (255 != (newpos & 0xff)) {
		anim_fbeepos = newpos & 0xff;
	} else {
		anim_fbeepath = NULL;
	}
	return need_update;
}

/* FIXME
 *
 * move arms randomly (not in frisbee path)
 * brightness control
 */
