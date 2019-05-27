#include "colorPoint.h"

colorPoint::colorPoint(uint8_t r, uint8_t g, uint8_t b, uint8_t l){
        setRGBL((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)l);
}
colorPoint::colorPoint(float r, float g, float b, float l){
        setRGBL((float)r, (float)g, (float)b, (float)l);
}
colorPoint::colorPoint(uint32_t color){
        setRGBL(color);
}
colorPoint::colorPoint(){
        setRGBL(0);
}

float colorPoint::validate(uint8_t value){
        if(value < (uint8_t)0) return 0;
        if(value > (uint8_t)255) return 255;
        return (float) value;
}
float colorPoint::validate(float value){
        if(value < (float)0) return (float)0;
        if(value > (float)255) return (float)255;
        return (float)value;
}

void colorPoint::setRGBL(uint8_t r, uint8_t g, uint8_t b, uint8_t l){
        this->r = validate(r);
        this->g = validate(g);
        this->b = validate(b);
        this->l = validate(l);
}

void colorPoint::setRGBL(float r, float g, float b, float l){
        this->r= validate(r);
        this->g = validate(g);
        this->b = validate(b);
        this->l = validate(l);
}

void colorPoint::setRGBL(uint32_t color){
        r = validate((uint8_t) (color >> 16));
        g = validate((uint8_t) (color >> 8));
        b = validate((uint8_t) color);
        l = 0;
}

void colorPoint::setL(uint8_t l){
        this->l = validate(l);
}
void colorPoint::setL(float l){
        this->l = validate(l);
}

float colorPoint::getR(){
        return r;
}

float colorPoint::getG(){
        return g;
}

float colorPoint::getB(){
        return b;
}

float colorPoint::getL(){
        return l;
}

uint8_t colorPoint::getIntR(){
        return (uint8_t) r;
}

uint8_t colorPoint::getIntG(){
        return (uint8_t) g;
}

uint8_t colorPoint::getIntB(){
        return (uint8_t) b;
}

uint8_t colorPoint::getIntL(){
        return (uint8_t) l;
}

uint32_t colorPoint::getRGB(){
        uint32_t color = 0;
        color = ((uint8_t)r << 16) | ((uint8_t)g << 8) | ((uint8_t)b);
        return color;
}
