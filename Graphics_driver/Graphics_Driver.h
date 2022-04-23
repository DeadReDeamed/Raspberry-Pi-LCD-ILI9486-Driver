/*
 * Created on Sat Apr 23 2022
 * Author: Luca Brugel
 * File: Graphics_Driver.h
 * 
 */

#ifndef Graphics_Driver
#define Graphics_Driver

#include <stdint.h>
#include "LCD_Driver.h"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

/**
 * Initializes the screen and sets the screen arrays to 0x0000 for updating
 */ 
void GD_init();

/**
 * Checks for differences in the new screen pixels and if needed updates the pixels that are different.
 */ 
void GD_update_screen();

/**
 * Sets the whole screen to the pixels given
 * @param screenPixels Array for all the pixels on the screen
 */ 
void GD_set_screen(uint16_t screenPixels[480][320]);

/**
 * Sets a block of pixels to the given image
 * @param x Position to draw
 * @param y Position to draw
 * @param image_p Pointer to an array of pixels to draw
 * @param h Height of the image
 * @param w Width of the image
 */ 
void GD_set_block(int x, int y, uint16_t *image_p, int h, int w);

/**
 * Clears screen and sets all pixels to 0x0000
 */ 
void GD_clear_screen();

/**
 * Converts RGB to the hex number needed to send to the LCD
 * @param r Red value 0-255
 * @param g Green value 0-255
 * @param b Blue value 0-255
 */ 
uint16_t convertRGBtoHex(int r, int g, int b);


#endif