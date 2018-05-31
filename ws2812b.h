#ifndef WS2812B_H
#define WS2812B_H

/* C interface for WS2812B rgbled functions */

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} Pixel;

void rgbled_init(void);
void rgbled_vsync(void);
int rgbled_update(Pixel *scr, int len);

#endif
