#include "Undulator.h"
#include <boost/math/special_functions/bessel.hpp>
#include <libgenesis13/lattice/LatticeElements.h>

using boost::math::cyl_bessel_j;

Undulator::~Undulator() {}

Undulator::Undulator() {
    istepz=-1;
    zstop=1e9;
}

void Undulator::updateOutput(double zstop_in, int nzout) {
    // calculate the size of the output record
    zstop=zstop_in;
    istepz=-1;
    nstepz=aw.size();
    nout=1;
    out.resize(nstepz+1);
    out[0]=true;  // first is always output
    for (int i=1; i<(nstepz+1); i++) {
        out[i]=false;
        if (((i % nzout)==0)&&(z[i-1]<zstop)) {
            out[i]=true;
            nout++;
        }
    }
    return;
}

void Undulator::updateMarker(int nfld, int npar, int nsort, double zstop) {
    for (int i=0; i<marker.size(); i++) {
        if (nfld > 0) { // field dump
            if ((i % nfld) == 0) {
                marker[i]|=1;
            }
        }
        if (npar > 0) {   // particle dump
            if ((i % npar) == 0) {
                marker[i]|=2;
            }
        }
        if (nsort > 0) {   // sorting
            if ((i % nsort) == 0) {
                marker[i]|=4;
            }
        }
        if (z[i]>zstop) {  // stop calculation
            marker[i]|=8;
        }
    }
    return;
}

bool Undulator::advance(int rank) {
    istepz++;
    if (istepz >= nstepz) { // end reached?
        return false;
    }
    if ((marker[istepz]&8)
            >0) { // check for 3rd bit set in marker value for stoping calculation
        if (rank==0) {
            cout << "Calculation terminated due to requested stop.";
            cout << " Missing output padded with zeros in output file" << endl;
        }
        return false;
    }
    int dstepz=nstepz/10;
    if (dstepz<1) {
        dstepz=1;
    }
    if (((istepz % dstepz) == 0) && (istepz >0) && (rank==0)) {
        cout << "  Calculation: " <<10*istepz/dstepz << "% done" << endl;
    }
    return true;
}

double Undulator::fc(int h) {
    double coup=aw[istepz];
    if (this->isHelical()) {
        if (h==1) {
            return coup;
        } else {
            return 0;
        }
    } else {
        double xi = aw[istepz]*aw[istepz];
        xi=0.5 * xi /(1+xi)*static_cast<double>(h);
        int h0, h1;
        if ((h % 2) == 1) {
            h0=(h-1)/2;
            h1 = h0+1;
            return coup*(cyl_bessel_j(h0, xi)-cyl_bessel_j(h1, xi))*pow(-1., h0);
        } else {
            h0=(h-2)/2;
            h1 = h0+2;
            return coup*0.5*(cyl_bessel_j(h0, xi)-cyl_bessel_j(h1, xi))*pow(-1., h0);
        }
    }
}

// square of the transverse dependence of the undulator field.
double Undulator::faw2(double x, double y) {
    double dx=x-ax[istepz];
    double dy=y-ay[istepz];
    // note kx is scaled as XKX*ku*ku in Lattice.cpp, gradx as ku*GRADX.
    return (1+kx[istepz]*dx*dx+ky[istepz]*dy*dy+2*(gradx[istepz]*dx+grady[istepz]*dy));
}

// transverse dependence of the undulator field.
double Undulator::faw(double x, double y) {
    double dx=x-ax[istepz];
    double dy=y-ay[istepz];
    // note kx is scaled as XKX*ku*ku in Lattice.cpp, gradx as ku*GRADX.
    return (1+0.5*(kx[istepz]*dx*dx+ky[istepz]*dy*dy)+gradx[istepz]*dx+grady[istepz]*dy);
}
