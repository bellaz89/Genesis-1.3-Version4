#ifndef __GENESIS__LATTICEELEMENTS__
#define __GENESIS__LATTICEELEMENTS__

#include <string>

using namespace std;

class Element {
public:
    double z, l;
    string type;
    //enum LatticeElement type;
};

class ID : public Element {
public:
    double aw, lambdau, kx, ky, nwig, ax, ay;
    double gradx, grady, pdadx, pdady; // gradients
    double paw, pkx, pky, phase; // perpendicular phase
    bool helical;
};

class Phaseshifter : public Element {
public:
    double phi;
};

class Quadrupole : public Element {
public:
    double k1, dx, dy;
};

class Drift : public Element {};

class Marker : public Element {
public:
    int action;  // this is a bitwise coded element
};

class Chicane : public Element {
public:
    double lb, ld, delay;
};

class Corrector : public Element {
public:
    double cx, cy;
};

#endif
