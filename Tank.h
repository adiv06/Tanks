#ifndef TANK_H
#define TANK_H

#include "Object.h"

class Tank : public Object {
public:

/**
 * @brief Constructor for the Tank class.
 * 
 * @param xPos The initial X position of the tank.
 * @param yPos The initial Y position of the tank.
 * @param imageData A pointer to the tank's image data (sprite).
 * @param health The tank's starting health value.
 * @param speed How fast the tank can move.
 * @param firePower The tank's attack power or damage value.
 */

    Tank(int xPos, int yPos, const uint16_t *imageData, int hp, int spd, int fire);

    int getHealth();
    int getSpeed();
    int getFirePower();
    uint8_t getAngleDeg() const;

    void setAngleDeg(uint8_t angle);
    void takeDamage(int dmg);
    void boostSpeed(int amount);

private:
    int health;
    int speed;
    int firePower;
    uint8_t angleDeg;
};

#endif // TANK_H
