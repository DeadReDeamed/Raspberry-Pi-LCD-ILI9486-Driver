/*
 * Created on Sat Apr 23 2022
 * Author: Luca Brugel
 * File: Graphics_Driver.c
 * 
 */


#include "Graphics_Driver.h"

/** 
 * Defines the screen width and height 
 * [WARNING] dont change 
 */
#define SCRWIDTH 480
#define SCRHEIGHT 320

/**
 * Defines a struct for coordinates. Used for updating the screen. 
 */
typedef struct coordinates{
    int x1;
    int x2;
    int y1;
    int y2;
} coordinates_t;

/**
 * Current screen pixels
 */ 
uint16_t screenPixels[SCRWIDTH][SCRHEIGHT];

/**
 * Updated screen pixels for comparison
 */ 
uint16_t newFramePixels[SCRWIDTH][SCRHEIGHT];

static coordinates_t * GD_screen_changes(int *size);

/**
 * Initializes the screen and sets the screen arrays to 0x0000 for updating
 */ 
void GD_init(){
    for(int x = 0; x < SCRWIDTH; x++){
        for(int y = 0; y < SCRHEIGHT; y++){
            screenPixels[x][y] = 0x0000;
        }
    }

    for(int x = 0; x < SCRWIDTH; x++){
        for(int y = 0; y < SCRHEIGHT; y++){
            newFramePixels[x][y] = 0x0000;
        }
    }
}

/**
 * Checks for differences in the new screen pixels and if needed updates the pixels that are different.
 */ 
void GD_update_screen(){
    int size = 0;
    coordinates_t *coordinatesArray = GD_screen_changes(&size);


    if(coordinatesArray != NULL){
        int colorIndex = 0;
        for(int i = 0; i < size; i++){
            int sizeOfMalloc = ((((coordinatesArray + i)->x2 - (coordinatesArray + i)->x1) + 1) * (((coordinatesArray + i)->y2 - (coordinatesArray + i)->y1) + 1)) * sizeof(uint16_t);
            uint16_t *colors = (uint16_t *) malloc(sizeOfMalloc);
            for(int x = (coordinatesArray + i)->x1; x <= (coordinatesArray + i)->x2; x++){
                for(int y = (coordinatesArray + i)->y1; y <= (coordinatesArray + i)->y2; y++){
                    *(colors + colorIndex) = newFramePixels[x][y];
                    screenPixels[x][y] = newFramePixels[x][y];
                    colorIndex++;
                }
            }
            LCD_write_addr(coordinatesArray[i].x1, coordinatesArray[i].y1, coordinatesArray[i].x2, coordinatesArray[i].y2, colors);
            colorIndex = 0;
            free(colors);
        }
        free(coordinatesArray);
    }
}

/**
 * Sets the whole screen to the pixels given
 * @param screenPixels Array for all the pixels on the screen
 */ 
void GD_set_screen(uint16_t screenPixels[480][320]){
    for(int x = 0; x < SCRWIDTH; x++){
        for(int y = 0; y < SCRHEIGHT; y++){
            newFramePixels[x][y] = screenPixels[x][y];
        }
    }
}

/**
 * Clears screen and sets all pixels to 0x0000
 */ 
void GD_clear_screen(){
    for(int x = 0; x < SCRWIDTH; x++){
        for(int y = 0; y < SCRHEIGHT; y++){
            newFramePixels[x][y] = 0x0000;
        }
    }
}

/**
 * Sets a block of pixels to the given image
 * @param x Position to draw
 * @param y Position to draw
 * @param image_p Pointer to an array of pixels to draw
 * @param h Height of the image
 * @param w Width of the image
 */ 
void GD_set_block(int x, int y, uint16_t *image_p, int h, int w){
    for(int i = 0; i < h; i++){
        for(int z = 0; z < w; z++){
            newFramePixels[x+z][y+i] = *(image_p + (i * w) + z);
        }
    }
}

/**
 * Goes through the pixels and checks for changes
 * @param size Pointer to the size of the array of coordinates returned
 * @return Returnes a pointer to an array of coordinates to change. If NULL then there are no changes
 */ 
static coordinates_t *GD_screen_changes(int *size){
    coordinates_t *changedCoordinates = NULL;
    bool readingChanges = false;
    int numberOfPixels = 0;
    coordinates_t coordinates;
    for(int y = 0; y < SCRHEIGHT; y++){
        for(int x = 0; x < SCRWIDTH; x++){
            if(screenPixels[x][y] != newFramePixels[x][y]){
                if(readingChanges == false){
                    coordinates.x1 = x;
                    coordinates.y1 = y;
                    readingChanges = true;
                } else {
                    numberOfPixels++;
                }

                if(numberOfPixels == 100){
                    coordinates.x2 = x;
                    coordinates.y2 = y;
                    if(changedCoordinates == NULL){
                        changedCoordinates = (coordinates_t *) malloc(sizeof(coordinates_t));
                    } else {
                        changedCoordinates = (coordinates_t *) realloc(changedCoordinates, (*size * sizeof(coordinates_t)) + sizeof(coordinates_t));
                    }
                    *(changedCoordinates + *size) = coordinates;
                    *size += 1;
                    readingChanges = false;
                    numberOfPixels = 0;
                }
            } else {
                if(readingChanges == true){
                    coordinates.x2 = x - 1;
                    coordinates.y2 = y;
                    if(changedCoordinates == NULL){
                        changedCoordinates = (coordinates_t *) malloc(sizeof(coordinates_t));
                    } else {
                        changedCoordinates = (coordinates_t *) realloc(changedCoordinates, (*size * sizeof(coordinates_t)) + sizeof(coordinates_t));
                    }
                    *(changedCoordinates + *size) = coordinates;
                    *size += 1;
                    readingChanges = false;
                }
            }
            
        }
    }
    if(readingChanges == true){
    coordinates.x2 = 320;
    coordinates.y2 = 480;
        if(changedCoordinates == NULL){
            changedCoordinates = (coordinates_t *) malloc(sizeof(coordinates_t));
        } else {
            changedCoordinates = (coordinates_t *) realloc(changedCoordinates, (*size * sizeof(coordinates_t)) + sizeof(coordinates_t));
        }
        *(changedCoordinates + *size) = coordinates;
        *size += 1;
        readingChanges = false;
    }
    
    return changedCoordinates;
}

/**
 * Converts RGB to the hex number needed to send to the LCD
 * @param r Red value 0-255
 * @param g Green value 0-255
 * @param b Blue value 0-255
 */ 
uint16_t convertRGBtoHex(int r, int g, int b){
    uint16_t hexValue = 0x0000;
    int rValue = round((r/255.0) * 31);
    int gValue = round((g/255.0) * 0b111111);
    int bValue = round((b/255.0) * 0b11111);
    hexValue |= rValue << 11;
    hexValue |= gValue << 5;
    hexValue |= bValue; 
    return hexValue;
}