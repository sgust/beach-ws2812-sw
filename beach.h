#ifndef BEACH_H
#define BEACH_H

/* C interface for beach animation routines */

/* some colors */

const Pixel pix_sand = { 0x80, 0x80, 0x00 };
const Pixel pix_water = { 0x00, 0x00, 0x80 };

void setpixel(Pixel *scr, Pixel *col);
void setwave(Pixel *scr, int w, Pixel *col);
#endif
