#ifndef COLORPOINT_H_
#define COLORPOINT_H_

#include <stdint.h>

class colorPoint {
private:
float r, g, b, l;

float validate(uint8_t value);
float validate(float value);

public:
colorPoint(uint8_t r, uint8_t g, uint8_t b, uint8_t l = 0);
colorPoint(float r, float g, float b, float l = 0);
colorPoint(uint32_t color);
colorPoint();
void setRGBL(float r, float g, float b, float l = 0);
void setRGBL(uint8_t r, uint8_t g, uint8_t b, uint8_t l = 0);
void setL(uint8_t l);
void setL(float l);
uint8_t getIntR();
uint8_t getIntG();
uint8_t getIntB();
uint8_t getIntL();
float getR();
float getG();
float getB();
float getL();
void setRGBL(uint32_t color);
uint32_t getRGB();
};


#endif
