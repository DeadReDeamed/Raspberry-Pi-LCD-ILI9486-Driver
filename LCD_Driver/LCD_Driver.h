#include <stdio.h>
#include <wiringPi.h>
#include <stdint.h>
#include <stdlib.h>

#define WRPIN 16
#define RSPIN 20
#define CSPIN 21
#define RSTPIN 22

void LCD_Init();

void LCD_push_command(uint16_t cmd, uint8_t *block, int8_t N);

void LCD_set_addr_window(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

void writeCmd16(uint16_t cmd);

void writeData16(uint16_t data);

void writeData8(uint8_t data);

void writeCmd8(uint8_t cmd);

void LCD_write_addr(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t *colors);