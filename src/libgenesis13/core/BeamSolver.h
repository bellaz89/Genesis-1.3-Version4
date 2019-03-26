#ifndef __GENESIS_BEAMSOLVER__
#define __GENESIS_BEAMSOLVER__

#include <vector>
#include <complex>
#include "Undulator.h"
#include "EFieldSolver.h"
#include "TrackBeam.h"

using std::vector;
using std::complex;

class Field;

class BeamSolver {
public:

    BeamSolver(bool _onlyFundamental = false);
    void initEField(double rmax, int ngrid, int nz, int nphi, double lambda);
    void advance(double, Beam*, vector< Field*>*, Undulator*);
    void track(double, Beam*, Undulator*, bool);
    void applyR56(Beam*, Undulator*, double);

protected:

    // needed to pass simulation params in the various sections
    class SimulationParams {

    public:
        SimulationParams(vector<double> &_rharm, vector<complex<double>> &_rpart) :
            rharm(_rharm), rpart(_rpart) {};
        double gamma = 0.0;
        double theta = 0.0;
        double xku = 0.0;
        double xks = 0.0;
        double btpar = 0.0;
        vector<double> &rharm;
        vector<complex<double>> &rpart;
    };

private:

    bool onlyFundamental;
    void RungeKutta(const double delz, SimulationParams &sim_params);
    void ODE(double &k2gg, double &k2pp, const SimulationParams &sim_params);

    EFieldSolver efield;
    TrackBeam tracker;

};

#endif
