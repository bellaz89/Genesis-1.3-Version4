#ifndef __GENESIS_COLLECTIVE__
#define __GENESIS_COLLECTIVE__

#include "Particle.h"
#include "Undulator.h"

class Beam;

class Collective {
public:
    Collective();
    virtual ~Collective();
    void initWake(unsigned int, double, double*, double, bool);
    void apply(Beam*, Undulator*, double );

private:
    bool transient, hasWake;
    double ztrans;
    double ds;
    unsigned int ns;
    double* wakeres, *current;
};

#endif
