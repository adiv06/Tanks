// Object.h
#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>

/**
 * Represents a basic game object with a position and image reference.
 */
class Object {
public:
    /**
     * Constructor for Object class
     * @param xPos The initial X position
     * @param yPos The initial Y position
     * @param imageData A pointer to the object's image data
     */
    Object(int xPos, int yPos, const uint16_t *imageData);

    int getX();
    int getY();
    const uint16_t* getImage();
    void setX(int newX);
    void setY(int newY);
    void setImage(const uint16_t *newImage);

protected:
    int x;
    int y;
    const uint16_t *image;
};

#endif // OBJECT_H
