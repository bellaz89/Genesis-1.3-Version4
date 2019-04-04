#ifndef __GEN_WRITEBEAMHDF5__
#define __GEN_WRITEBEAMHDF5__

#include <string>
#include <hdf5.h>
#include <libgenesis13/core/Beam.h>

using std::string;

class WriteBeamHDF5 : public HDF5Base {
public:
    void write(string, Beam*);

private:
    void writeGlobal(int, bool, double, double, double, int, hid_t fid);
};

#endif

