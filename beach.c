/* handle the beach scene animation */

#include <stdio.h>

#include "stm32f10x.h"
#include "common.h"
#include "ws2812b.h"
#include "beach.h"

/* some colors */
const Pixel pix_sand = { 0x80, 0x40, 0x00 };
const Pixel pix_water = { 0x00, 0x00, 0x80 };

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
	}
}
