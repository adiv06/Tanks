#include <cstdint>
#include <stdio.h>
#include <stdint.h>
#include "Tank.h"
#include "Object.h"

#include "GameLogic.h"


#define TERRAINOFFSET 5

//THIS SHOULD BE CHANGED IF WE ARE CHANGING IMAGE OR MAKING MAP PROGRAMATICALLY
void updateTerrainHeights(int16_t terrainHeights[],  int length, const uint16_t *image){
    const uint8_t terrainWidth = 160;
    const uint8_t terrainImageHeight = 128;

    // //iterate and populate, after its done, fill in the rest with same as before
    // for(int i = 0; i < terrainWidth; i++){
    //     for(int j = 0; j < terrainImageHeight; j++){
    //         if(image[terrainWidth * j + i] == 0){
    //             while(image[terrainWidth * j + i] == 0){j++;}
    //             terrainHeights[i] = terrainWidth - j;
    //             break;
    //         }
    //     }
    // }

    // //Filling in the rest of the terrainheights array, assinging all elements to the last value (we repeated the colums)
    // for(int i = 127; i < length; i++){
    //     terrainHeights[i] = terrainHeights[terrainWidth - 1];
    // }



     //iterate and populate, after its done, fill in the rest with same as before
    for(int i = 0; i < terrainWidth; i++){
        for(int j = 0; j < terrainImageHeight; j++){
            if(image[terrainWidth * j + i] == 0){
                terrainHeights[i] = terrainImageHeight - j;
                terrainHeights[i] = terrainHeights[i] + TERRAINOFFSET;
            }
        }
    }

}

void updateSlopes(int16_t terrainHeights[],  int length, int16_t slopes[], int lengthSlope, bool playerOne){
    //Slopes are 1 to 4, have an array that is 1 to 5 with indexes
    if(length != lengthSlope){
        while(1){}
    }

    //Filling with initial slopes
    for(int i = 0; i < length - 3; i++){
        int sum = (int16_t)terrainHeights[i] - (int16_t)terrainHeights[i + 1];
        // slopes[i] = sum >> 2; //sum / 4
        slopes[i] = (int16_t)sum;
    }

    for(int j = 0; j < 3; j++){
            slopes[lengthSlope - 1 - j] = slopes[lengthSlope - 4];
    }
    //Done with filling with initial slopes

    //Cascading the slopes
    for(int i = 0; i < lengthSlope; i++){
        if(slopes[i] != 0 && i < lengthSlope - 15){
            int j = i;
            while(j < lengthSlope - 1){
                if(j >= i + 3 || slopes[j + 1] != 0){
                    break;
                }
                slopes[j + 1] = slopes[j];
            }
        }
    }

    // //Offsetting
    // for(int i = 0; i < length; i++){
    //     int sum = terrainHeights[i] - terrainHeights[i + 1];
    //     if(i >= 12){
    //         slopes[i - 12] = sum; // Shift slope to align with tank center
    //     }
    // }

    // //Creating the offsets in the slopes
    // if(playerOne){
    //     for(int i = 24; i < lengthSlope; i++){
    //         if(slopes[i] < 0){
    //             slopes[i - 24] = slopes[i];
    //         }
    //     }
        
    // }
    //else{
    //     for(int i = 0; i < lengthSlope - 24; i++){
    //         if(slopes[i] < 0){
    //             slopes[i + 24] = slopes[i];
    //     }
    //     }
    // }

}

void offsetHeights(int16_t terrainHeights[],  int length, int16_t slopes[], int lengthSlope, bool playerOne){
    for(int i = 0; i < length; i++){
        if(slopes[i] > 0){
            terrainHeights[i] -= 15;
        }
    }
}





// class Object {
// public:
//     Object(int xPos, int yPos, const uint16_t *imageData);


// protected:
//     int x;
//     int y;
//     const uint16_t *image; 
// };

// Constructor
Object::Object(int xPos, int yPos, const uint16_t *imageData) {
    x = xPos;
    y = yPos;
    image = imageData;
}

// Method definitions
int Object::getX() {
    return x;
}

int Object::getY() {
    return y;
}

const uint16_t*Object::getImage(){
    return image;
}

void Object::setX(int newX) {
    if(newX >= 0 && newX < 160){
        x = newX;
    }
}

void Object::setY(int newY) {
    if(newY >= 0 && newY < 128){
        y = newY;
    }
}

void Object::setImage(const uint16_t *newImage) {
    image = newImage;  // just repointing, not modifying
}


// class Tank : public Object {
// public:
//     Tank(int xPos, int yPos, const uint16_t *imageData, int hp, int spd, int fire);

// private:
//     int health;
//     int speed;
//     int firePower;
// };

// Tank constructor (uses base class constructor too)
Tank::Tank(int xPos, int yPos, const uint16_t *imageData, int hp, int spd, int fire)
    : Object(xPos, yPos, imageData), health(hp), speed(spd), firePower(fire) {
}

// Tank method implementations
int Tank::getHealth() {
    return health;
}

int Tank::getSpeed() {
    return speed;
}

int Tank::getFirePower() {
    return firePower;
}

uint8_t Tank::getAngleDeg() const {
    return angleDeg;
}

void Tank::setAngleDeg(uint8_t angle) {
    angleDeg = angle;
}


void Tank::takeDamage(int dmg) {
    health -= dmg;
    if (health < 0) health = 0;
}



