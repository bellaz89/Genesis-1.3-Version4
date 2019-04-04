// include all routine which are arising from a collective contribution from the beam. This are:
// Wake
// Longrange Space Charge
// CSR

#include "Collective.h"
#include <cmath>
#include <mpi.h>
#include "PhysicalConstants.h"
#include "Beam.h"

extern bool MPISingle;

Collective::Collective() {
    hasWake=false;
}

Collective::~Collective() {
}


void Collective::initWake(unsigned int ns_in, double ds_in, double* wakeres_in,
                          double ztrans_in, bool trans_in) {
    ztrans=ztrans_in;
    transient=trans_in;
    ns=ns_in;
    ds=ds_in;
    wakeres=new double [ns];
    current=new double[ns];
    for (unsigned i=0; i<ns; i++) {
        wakeres[i]=wakeres_in[i];
    }
    wakeres[0]*=0.5;  // self-loading theorem
    hasWake=true;
    return;
}

void Collective::apply(Beam* beam, Undulator* und, double delz) {
    if (!hasWake) {
        return;
    }
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (MPISingle) {
        rank=0;
        size=1;
    }
    // get full current profile
    int ncur=beam->current.size();
    double dscur=beam->slicelength;
    double* cur;
    cur=new double [size*ncur+1];
    MPI_Allgather(&beam->current[0], ncur, MPI_DOUBLE, &cur[0], ncur,
                  MPI_DOUBLE, MPI_COMM_WORLD);
    ncur=ncur*size;
    cur[ncur]=cur[ncur-1];  // needed for interpolation
    // interpolate current profile to high resolution and initialize the totak wake function
    double* wake=new double[ns];
    for (unsigned is=0; is <ns ; is++) {
        double s=ds*static_cast<double> (is);
        unsigned int idx=static_cast<int> (floor(s/dscur));
        double wei=1-(s-idx*dscur)/dscur;
        current[is]=wei*cur[idx]+(1-wei)*cur[idx+1];
        // convert current to number of electrons
        current[is]*=ds/ELECTRON_CHARGE_X_C;
        wake[is]=0;
    }
    // do the convolution
    //// loop the evaluation point from back to front
    for (unsigned int is=0; is< ns; is++) {
        // loob from evaluation point till the head of the bunch
        for (unsigned int i=0; i < ns-is; i++) {
            wake[is]+=current[is+i]*wakeres[i];
        }
    }
    ncur=beam->eloss.size();
    int ioff = rank*ncur;
    double sc0=dscur*(ioff);
    double sc1=dscur*(ncur+ioff);
    int* count=new int [ncur];
    for (int ic = 0; ic <ncur; ic++) {
        count[ic]=0;
    }
    for (unsigned int is=0; is < ns; is++) {
        double s=is*ds;
        if ((s >= sc0) and (s < sc1)) {
            int idx = floor((s-sc0)/dscur);
            count[idx]++;
            beam->eloss[idx]+=wake[is];
        }
    }
    for (int ic = 0; ic <ncur; ic++) {
        if (count[ic]>0) {
            beam->eloss[ic]/=static_cast<double>(count[ic]);
        } else {
            beam->eloss[ic]=0;
        }
        // actuall beam loss per integration step
        double dg=beam->eloss[ic]*delz/511000;
        int npart=beam->beam.at(ic).size();
        for (int ip=0; ip<npart; ip++) {
            beam->beam.at(ic).at(ip).gamma+=dg;
        }
    }
    delete[] cur;
    delete[] wake;
    delete[] count;
    return;
}
