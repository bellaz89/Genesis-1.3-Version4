#ifndef __GENESIS_TRACKBEAM__
#define __GENESIS_TRACKBEAM__

#include <Eigen/Core>
#include "Undulator.h"
#include "Particle.h"

using Eigen::Matrix4d;
using Eigen::MatrixXd;
using Eigen::Ref;

class Beam;

class TrackBeam {
public:
    static void track(double, Beam*, Undulator*, bool);
    static void applyDrift(double, double, double*, double*, double, double);
    static void applyFQuad(double, double, double*, double*, double, double);
    static void applyDQuad(double, double, double*, double*, double, double);
    static void applyCorrector(Beam*, double, double);
    static void applyChicane(Beam*, double, double, double, double, double);
    static void chicaneTransferMatrix(Ref<Matrix4d> tmatrix, double angle, 
            double lb, double ld, double lt); 
    static void applyR56(Beam*, Undulator*, double);
};

#endif
