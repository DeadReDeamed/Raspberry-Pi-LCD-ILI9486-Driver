#include "Graphics_Driver.h"

#define SCRWIDTH 480
#define SCRHEIGHT 320

typedef struct coordinates{
    int x1;
    int x2;
    int y1;
    int y2;
} coordinates_t;

uint16_t screenPixels[SCRWIDTH][SCRHEIGHT];
uint16_t newFramePixels[SCRWIDTH][SCRHEIGHT];

static coordinates_t * GD_screen_changes(int *size);

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

void GD_set_screen(uint16_t screenPixels[480][320]){

}

void GD_clear_screen(){
    for(int x = 0; x < SCRWIDTH; x++){
        for(int y = 0; y < SCRHEIGHT; y++){
            newFramePixels[x][y] = 0x0000;
        }
    }
}

void GD_set_block(int x, int y, uint16_t *image_p, int h, int w){
    for(int i = 0; i < h; i++){
        for(int z = 0; z < w; z++){
            newFramePixels[x+z][y+i] = *(image_p + (i * w) + z);
        }
    }
}

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