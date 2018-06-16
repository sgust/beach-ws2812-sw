/* handle the beach scene animation */

#include <stdio.h>

#include "stm32f10x.h"
#include "common.h"
#include "ws2812b.h"
#include "beach.h"

/* some colors */
const Pixel Pix_sand = { 0x80, 0x40, 0x00 };
const Pixel Pix_water = { 0x00, 0x00, 0x80 };
const Pixel Pix_water2 = { 0x20, 0x40, 0x80 };

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
