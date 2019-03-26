#ifndef __GENESIS_FIELD__
#define __GENESIS_FIELD__

#include <vector>
#include <complex>
#include "Undulator.h"
#include "FieldSolver.h"

using namespace std;

class Beam;

class Field {
public:

    Field();
    virtual ~Field();
    void initDiagnostics(int);
    void diagnostics(bool);
    void init(int, int, double, double, double, double, int);
    bool harmonicConversion(int, bool);
    bool subharmonicConversion(int, bool);
    void track(double, Beam*, Undulator*);
    bool getLLGridpoint(double, double, double*, double*, int*);
    void setStepsize(double);
    void disable(double);
    bool isEnabled();
    int getHarm();
    double getRHarm();

    vector< vector< complex<double>>> field;
    double xlambda, dgrid, xks, gridmax, dz_save;
    int ngrid, first;  // first points to first slice due to slippage
    int harm;
    bool polarization;
    double slicelength, s0;

    vector<double> power, xsig, xavg, ysig,
           yavg ; // buffer to accumulate before writing it out
    vector<double> nf_intensity, nf_phi, ff_intensity, ff_phi;

private:
    int idx;
    bool disabled;
    double rharm;
    double accuslip;

    FieldSolver solver;
};

#endif
