/* handle the beach scene animation */

#include <stdio.h>

#include "stm32f10x.h"
#include "common.h"
#include "ws2812b.h"

/* set 1 pixel to color col */
void setpixel(Pixel *scr, Pixel *col)
{
	scr->red = col->red;
	scr->green = col->green;
	scr->blue = col->blue;
}

/* set all LEDs for wave w to color col */
void setwave(Pixel *scr, int w, Pixel *col)
{
	switch (w) {
		case 0:
			setpixel(&scr[0], col);
			setpixel(&scr[1], col);
			setpixel(&scr[2], col);
			break;
		case 1:
			setpixel(&scr[4], col);
			setpixel(&scr[5], col);
			setpixel(&scr[6], col);
			setpixel(&scr[11], col);
			setpixel(&scr[12], col);
			setpixel(&scr[13], col);
			break;
	}
}
