#ifndef __GENESIS_EFIELDSOLVER__
#define __GENESIS_EFIELDSOLVER__

#include <vector>
#include <complex>
#include "Particle.h"

using namespace std;

class EFieldSolver {
public:
    EFieldSolver();
    virtual ~EFieldSolver();
    void init(double, int, int, int, double);
    void shortRange(vector<Particle>*, vector<double> &, double, double);

private:
    vector<int> idx;
    vector<double> azi;
    vector<complex<double>> csrc, clow, cmid, cupp, celm, gam;
    vector<double> lupp, lmid, llow, rlog, vol;

    int nz, nphi, ngrid_ref;
    double rmax_ref, ks;
};

#endif
