#include "LEDControll.hpp"


LEDControll::LEDControll(uint8_t pinRed, uint8_t pinGreen, uint8_t pinBlue){
        this->pinRed = pinRed;
        this->pinGreen = pinGreen;
        this->pinBlue = pinBlue;
}

void LEDControll::init(){
        pinMode(this->pinRed, OUTPUT);
        pinMode(this->pinGreen, OUTPUT);
        pinMode(this->pinBlue, OUTPUT);
        this->setStripColor(0, 0, 0);
}

void LEDControll::setStripColor(uint8_t r, uint8_t g, uint8_t b){
        uint32_t red, green, blue;
        red = 4*r;
        green = 4*g;
        blue = 4*b;
        colorPoint color = colorPoint(r, g, b);
        this->currentColor = color;
        #if INVERTED
        analogWrite(this->pinRed, 1024 - red);
        analogWrite(this->pinGreen, 1024 - green);
        analogWrite(this->pinBlue, 1024 - blue);
        #else
        analogWrite(this->pinRed, red);
        analogWrite(this->pinGreen, green);
        analogWrite(this->pinBlue, blue);
        #endif

}

void LEDControll::setStripColor(colorPoint c){
        this->setStripColor(c.getIntR(), c.getIntG(), c.getIntB());
}

colorPoint LEDControll::getColor(){
        return this->currentColor;
}

void LEDControll::moveToColor(colorPoint newColor){
        colorPoint current = this->getColor();
        colorVector deltaColor = colorVector(current, newColor);
        colorVector norm = deltaColor.getNorm();
        float abs = deltaColor.getAbs();
        int32_t i = 0;
        if(SERIAL) {
                Serial.print("abs "); Serial.println(abs);
                Serial.print("delta r/g/b ");
                Serial.print(deltaColor.getR()); Serial.print("/");
                Serial.print(deltaColor.getG()); Serial.print("/");
                Serial.println(deltaColor.getB());
                Serial.print("newColor r/g/b ");
                Serial.print(newColor.getR()); Serial.print("/");
                Serial.print(newColor.getG()); Serial.print("/");
                Serial.println(newColor.getB());
                Serial.print("current r/g/b ");
                Serial.print(current.getR()); Serial.print("/");
                Serial.print(current.getG()); Serial.print("/");
                Serial.println(current.getB());
                Serial.print("norm r/g/b ");
                Serial.print(norm.getR()); Serial.print("/");
                Serial.print(norm.getG()); Serial.print("/");
                Serial.println(norm.getB());
        }
        for(i=0; i<abs; i++) {
                float r = current.getR() + norm.getR();
                float g = current.getG() + norm.getG();
                float b = current.getB() + norm.getB();
                current.setRGBL(r, g, b);
                this->setStripColor(current);
                delay(5);
        }
}

void LEDControll::moveToColor(uint8_t r, uint8_t g, uint8_t b){
        colorPoint color = colorPoint(r, g, b);
        this->moveToColor(color);
}
