/*
 * Created on Sat Apr 23 2022
 * Author: Luca Brugel
 * File: LCD_Driver.h
 * 
 */


#ifndef LCD_DRIVER
#define LCD_DRIVER

#include <stdio.h>
#include <wiringPi.h>
#include <stdint.h>
#include <stdlib.h>


#define WRPIN 16
#define RSPIN 20
#define CSPIN 21
#define RSTPIN 22

/**
 * This function initializes the LCD. This function needs to be called before sending pixel data 
 */
void LCD_Init();

/**
 * This function will push a command block with n size params to the LCD
 * @param cmd command that needs to be pushed
 * @param block pointer to the first param for this command
 * @param N the amount of params that needs to be pushed
 */ 
void LCD_push_command(uint16_t cmd, uint8_t *block, int8_t N);

/**
 * Sets a block to write to on the LCD
 * @param x1 begin coördinate of the block
 * @param x2 end coördinate of the blcok
 * @param y1 begin coördinate of the block
 * @param y2 end coördinate of the block
 */ 
void LCD_set_addr_window(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

/**
 * This function writes 16 bit command to the LCD screen
 * @param cmd This is the command data that needs to be written
 */
void LCD_writeCmd16(uint16_t cmd);

/**
 * This function writes 16 bit data to the LCD screen
 * @param data This is de data that needs to be written
 */ 
void LCD_writeData16(uint16_t data);

/**
 * This function writes 8 bit command to the LCD screen
 * @param cmd This is the command data that needs to be written
 */
void LCD_writeData8(uint8_t data);

/**
 * Sets a block to write to on the LCD
 * @param x1 begin coördinate of the block
 * @param x2 end coördinate of the blcok
 * @param y1 begin coördinate of the block
 * @param y2 end coördinate of the block
 */ 
void LCD_writeCmd8(uint8_t cmd);

/**
 * Sets a block to write to on the LCD and fills it with an array of colors
 * @param x1 begin coördinate of the block
 * @param x2 end coördinate of the blcok
 * @param y1 begin coördinate of the block
 * @param y2 end coördinate of the block
 * @param colors pointer to a list of colors to write to the specified block
 */
void LCD_write_addr(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t *colors);

#endif