#ifndef LEDCONTROLL_HPP_
#define LEDCONTROLL_HPP_
#include <Arduino.h>
#include "colorPoint.h"
#include "colorVector.h"

class LEDControll {
private:
uint8_t pinRed, pinGreen, pinBlue;
colorPoint currentColor;
public:
LEDControll(uint8_t pinRed, uint8_t pinGreen, uint8_t pinBlue);
void init();
void setStripColor(uint8_t r, uint8_t g, uint8_t b);
void setStripColor(colorPoint c);
void moveToColor(colorPoint newColor);
void moveToColor(uint8_t r, uint8_t g, uint8_t b);

colorPoint getColor();
};


#endif
