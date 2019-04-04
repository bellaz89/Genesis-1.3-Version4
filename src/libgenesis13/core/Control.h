#ifndef __GENESIS_CONTROL__
#define __GENESIS_CONTROL__

#include <vector>
#include <string>
#include <complex>
#include "Field.h"
#include "Beam.h"
#include "Undulator.h"

using std::vector;
using std::string;
using std::complex;

class Control : public HDF5Base {
public:
    void applySlippage(double, Field*);
    bool applyMarker(Beam*, vector<Field*>*, Undulator*);
    bool init(int, int, const char*, Beam*, vector<Field*>*, Undulator*, bool, bool);
    void output(Beam*, vector<Field*>*, Undulator*);

private:
    bool timerun, scanrun, one4one;
    int nslice, noffset;
    int rank, size;
    double accushift;
    double sample, reflen, slen;
    vector<complex<double>> receive_buffer;
    string root;
};


#endif
