/*
 * Created on Sat Apr 23 2022
 * Author: Luca Brugel
 * File: LCD_Driver.h
 * 
 */

#include "LCD_Driver.h"

#define TFTLCD_DELAY8 0x7F

/* Defines the datapins for sending data
 * This is only used for sending commands, data is defined in a different manner via datapointer
 * Change when needed
 */
int dataPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

/* Points to the first pin 
 * See pins above for data
 * [BUG] when you send commands or send half data it doesn't work
 */
volatile unsigned int *dataPointer;

static void LCD_init_table8(const void *table, int16_t size);

/*
 * Sends a write strobe to the display for sending data
 */
static void wr_strobe(){
    digitalWrite(WRPIN, 0);
    digitalWrite(WRPIN, 1);
}

/*
 * Will reset the LCD
 */
static void LCD_Reset(){
    digitalWrite(RSTPIN, 0);
    delay(2);
    digitalWrite(RSTPIN, 1);

    digitalWrite(CSPIN, 0);
    digitalWrite(RSPIN, 0);
    wr_strobe();
    wr_strobe();
    wr_strobe();

    digitalWrite(CSPIN, 1);
    digitalWrite(RSPIN, 1);
}

/**
 * This function will start the LCD and write the right setting to the LCD 
 */
static void start(){
    static const uint8_t ILI9486_regValues[] =  
    { 
            0x01, 0,            //Soft Reset
            TFTLCD_DELAY8, 150,  // .kbv will power up with ONLY reset, sleep out, display on
            0x28, 0,            //Display Off
            0x3A, 1, 0x55,      //Pixel read=565, write=565.
            0xB1, 2, 0b10100000, 0b00010000,
            0xC0, 2, 0x0E, 0x0E,        //Power Control 1 [0E 0E]
            0xC1, 2, 0x43, 0x00,        //Power Control 2 [43 00]
            0xC2, 1, 0x33,      //Power Control 3 [33]
            0xC5, 4, 0x00, 0x40, 0x00, 0x40,    //VCOM  Control 1 [00 40 00 40]
            0xB4, 1, 0x00,      //Inversion Control [00]
            0xB6, 3, 0x02, 0x02, 0x3B,  // Display Function Control [02 02 3B]
            0x36, 1, 0x28,
            0xE0, 15,0x0F, 0x24, 0x1C, 0x0A, 0x0F, 0x08, 0x43, 0x88, 0x32, 0x0F, 0x10, 0x06, 0x0F, 0x07, 0x00,
            0xE1, 15,0x0F, 0x38, 0x30, 0x09, 0x0F, 0x0F, 0x4E, 0x77, 0x3C, 0x07, 0x10, 0x05, 0x23, 0x1B, 0x00,
            0x11, 0,            //Sleep Out
            TFTLCD_DELAY8, 150,
            0x29, 0,         //Display On
            0x13, 0
    }; 
    LCD_init_table8(ILI9486_regValues, sizeof(ILI9486_regValues));
}

/**
 * This function initializes the LCD. This function needs to be called before sending pixel data 
 */
void LCD_Init(){
    wiringPiSetupGpio();
    pinMode(CSPIN, OUTPUT);
    pinMode(RSPIN, OUTPUT);
    pinMode(RSTPIN, OUTPUT);
    pinMode(WRPIN, OUTPUT);

    for(int i = 0; i < 16; i++){
        pinMode(dataPins[i], OUTPUT);
    }
    dataPointer = _wiringPiGpio + 7;
    LCD_Reset();
    LCD_Reset();
    delay(200);
    start();
    delay(100);
    for(int i = 1; i <= 16; i++){
        for(int z = 1; z <= 16; z++){

            LCD_set_addr_window(0 + (30 *(z-1)), 0 + (20 * (i -1)), 30 * z, 20 * i);
            digitalWrite(CSPIN, 0);
                LCD_writeCmd8(0x2C);
                for(int x = 0; x < 120; x++){
                    for(int y = 0; y < 80; y++){
                            LCD_writeData16(0x0000);
                    }
                }
            digitalWrite(CSPIN, 1);
        }
    }
}

/**
 * This function writes 8 bit data to the LCD screen
 * @param data This is de data that needs to be written
 */ 
void LCD_writeData8(uint8_t data){
    for(int i = 0; i < 8; i++){
        digitalWrite(dataPins[i], (data & (1<<i)));
    }

    for(int i = 8; i < 16; i++){
        digitalWrite(dataPins[i], 0);
    }
    wr_strobe();
}

/**
 * This function writes 8 bit command to the LCD screen
 * @param cmd This is the command data that needs to be written
 */
void LCD_writeCmd8(uint8_t cmd){
    digitalWrite(RSPIN, 0);
    for(int i = 0; i < 8; i++){
        digitalWrite(dataPins[i], (cmd & (1<<i)));
    }

    for(int i = 8; i < 16; i++){
        digitalWrite(dataPins[i], 0);
    }
    wr_strobe();
    digitalWrite(RSPIN, 1);
}

/**
 * This function writes 16 bit data to the LCD screen
 * @param data This is de data that needs to be written
 */ 
void LCD_writeData16(uint16_t data){
    *(dataPointer + 3) = 0xFFFF;
    *dataPointer &= 0x0000;
    *dataPointer = data;
    wr_strobe();
}


/**
 * This function writes 16 bit command to the LCD screen
 * @param cmd This is the command data that needs to be written
 */
void writeCmd16(uint16_t cmd){
    digitalWrite(RSPIN, 0);
    for(int i = 0; i < 16; i++){
        digitalWrite(dataPins[i], (cmd & (1<<i)));
    }
    wr_strobe();
    digitalWrite(RSPIN, 1);
}

/**
 * This function will push a command block with n size params to the LCD
 * @param cmd command that needs to be pushed
 * @param block pointer to the first param for this command
 * @param N the amount of params that needs to be pushed
 */ 
void LCD_push_command(uint16_t cmd, uint8_t *block, int8_t N){
    digitalWrite(CSPIN, 0);
    writeCmd16(cmd);
    while(N-- > 0){
        uint8_t u8 = *block++;
        LCD_writeData8(u8);
    }
    digitalWrite(CSPIN, 1);
}

/**
 * This function will initialize the lcd screen with the table that is given
 * @param table this is the table that needs to be written to the LCD screen
 * @param size the size of the table
 */ 
static void LCD_init_table8(const void *table, int16_t size){
    uint8_t i;
    uint8_t *p = (uint8_t *) table, dat[24];
    while(size > 0){
        uint8_t cmd = (*(const unsigned char *)(p++));
        uint8_t len = (*(const unsigned char *)(p++));
        if(cmd == TFTLCD_DELAY8){
            delay(len);
            len = 0;
        } else {
            for(i = 0; i < len; i++){
                dat[i] = (*(const unsigned char *)(p++));
            }
            LCD_push_command(cmd, dat, len);
        }
        size -= len + 2;
    }
}

/**
 * Sets a block to write to on the LCD
 * @param x1 begin coördinate of the block
 * @param x2 end coördinate of the blcok
 * @param y1 begin coördinate of the block
 * @param y2 end coördinate of the block
 */ 
void LCD_set_addr_window(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
    uint8_t x_buffer[] = {x1>>8, x1&0xFF, x2>>8, x2&0xFF};
    uint8_t y_buffer[] = {y2>>8, y1&0xFF, y2>>8, y2&0xFF};
    LCD_push_command(0x2A, x_buffer, 4);
    LCD_push_command(0x2B, y_buffer, 4);
}

/**
 * Sets a block to write to on the LCD and fills it with an array of colors
 * @param x1 begin coördinate of the block
 * @param x2 end coördinate of the blcok
 * @param y1 begin coördinate of the block
 * @param y2 end coördinate of the block
 * @param colors pointer to a list of colors to write to the specified block
 */ 
void LCD_write_addr(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t *colors){
    LCD_set_addr_window(x1, y1, x2, y2);
        digitalWrite(CSPIN, 0);
        int width = (x2 - x1) + 1;
        int height = (y2 - y1) + 1;
        LCD_writeCmd8(0x2C);
        for(int i = 0; i < width; i++){
            for(int x = 0; x < height; x++){
                LCD_writeData16(*(colors + (i * height) + x));
            }
        }
        digitalWrite(CSPIN, 1);
}