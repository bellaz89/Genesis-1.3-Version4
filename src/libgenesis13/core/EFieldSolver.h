#ifndef __GENESIS_EFIELDSOLVER__
#define __GENESIS_EFIELDSOLVER__

#include <vector>
#include <complex>
#include "Particle.h"

using namespace std;

class EFieldSolver {
public:
    void init(double, int, int, int, double);
    void shortRange(vector<Particle>*, vector<double> &, double, double);

private:
    vector<int> idx;
    vector<double> azi;
    vector<complex<double>> csrc, clow, cmid, cupp, celm, gam;
    vector<double> lupp, lmid, llow, rlog, vol;

    int nz = 0, nphi = 0, ngrid_ref = 0;
    double rmax_ref, ks;
};

#endif
