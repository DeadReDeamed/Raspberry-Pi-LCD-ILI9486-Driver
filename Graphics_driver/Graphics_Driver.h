#ifndef Graphics_Driver
#define Graphics_Driver

#include <stdint.h>
#include "LCD_Driver.h"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>


void GD_init();

void GD_update_screen();

void GD_set_screen(uint16_t screenPixels[480][320]);

void GD_set_block(int x, int y, uint16_t *image_p, int h, int w);

void GD_clear_screen();

uint16_t convertRGBtoHex(int r, int g, int b);


#endif