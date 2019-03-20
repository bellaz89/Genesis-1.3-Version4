#ifndef __GEN_WRITEBEAMHDF5__
#define __GEN_WRITEBEAMHDF5__

#include <hdf5.h>
#include <libgenesis13/core/Beam.h>
#include "HDF5Base.h"

using namespace std;

class WriteBeamHDF5 : public HDF5Base {
public:
    WriteBeamHDF5();
    virtual ~WriteBeamHDF5();
    void write(string, Beam*);

private:
    int rank, size;
    hid_t fid;
    void writeGlobal(int, bool, double, double, double, int);
};

#endif

