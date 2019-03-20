#ifndef __GENESIS_TRACKBEAM__
#define __GENESIS_TRACKBEAM__

#include "Undulator.h"
#include "Particle.h"

class Beam;

class TrackBeam {
public:
    TrackBeam();
    virtual ~TrackBeam();
    void track(double, Beam*, Undulator*, bool);

    void (TrackBeam::*ApplyX) (double, double, double*, double*, double, double);
    void (TrackBeam::*ApplyY) (double, double, double*, double*, double, double);
    void applyDrift(double, double, double*, double*, double, double);
    void applyFQuad(double, double, double*, double*, double, double);
    void applyDQuad(double, double, double*, double*, double, double);
    void applyCorrector(Beam*, double, double);
    void applyChicane(Beam*, double, double, double, double, double);
    void applyR56(Beam*, Undulator*, double);

private:
    void matmul(double a[][4], double b[][4]);
};


#endif
