#ifndef COLORVECTOR_H_
#define COLORVECTOR_H_

#include <stdint.h>
#include <math.h>
#include "colorPoint.h"

class colorVector {
private:
float dr, dg, db, dl;
colorVector(colorVector vector, float abs);


public:
colorVector(colorPoint start, colorPoint end);
float getR();
float getG();
float getB();
float getL();
colorVector getNorm();
colorVector getNormAll();
float getAbs();
float getAbsAll();
};

#endif
