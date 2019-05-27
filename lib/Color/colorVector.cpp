#include "colorVector.h"

colorVector::colorVector(colorPoint start, colorPoint end){
        dr = end.getR() - start.getR();
        dg = end.getG() - start.getG();
        db = end.getB() - start.getB();
        dl = end.getL() - start.getL();
}
colorVector::colorVector(colorVector vector, float abs){
        dr = vector.getR()/abs;
        dg = vector.getG()/abs;
        db = vector.getB()/abs;
        dl = vector.getL()/abs;
}

float colorVector::getR(){
        return dr;
}
float colorVector::getG(){
        return dg;
}
float colorVector::getB(){
        return db;
}
float colorVector::getL(){
        return dl;
}

float colorVector::getAbs(){
        return sqrt(pow(dr, 2) + pow(dg, 2) + pow(db, 2));
}
float colorVector::getAbsAll(){
        return sqrt(pow(dr, 2) + pow(dg, 2) + pow(db, 2) + pow(dl,2));
}

colorVector colorVector::getNorm(){
        return colorVector(*this, this->getAbs());
}

colorVector colorVector::getNormAll(){
        return colorVector(*this, this->getAbsAll());
}
