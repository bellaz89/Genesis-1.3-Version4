#ifndef __GEN_WRITEFIELDHDF5__
#define __GEN_WRITEFIELDHDF5__

#include <string>
#include <vector>
#include <hdf5.h>
#include <libgenesis13/core/Field.h>
#include "HDF5Base.h"

using std::string;
using std::vector;

class WriteFieldHDF5 : public HDF5Base {
public:
    void write(string fileroot, vector<Field*>* field);

private:
    void writeMain(string fileroot, Field* field, int rank, int size);
    void writeGlobal(double reflen, double slicelen, double s0,
                     double dx, int nx, int count, hid_t fid);
};
#endif

