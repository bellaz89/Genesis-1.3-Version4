#ifndef __GENESIS_READBEAMHDF5__
#define __GENESIS_READBEAMHDF5__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <hdf5.h>
#include <libgenesis13/io/HDF5Base.h>
#include <libgenesis13/core/Particle.h>
#include <libgenesis13/main/Setup.h>
#include <libgenesis13/main/Time.h>

using namespace std;

class ReadBeamHDF5 : public HDF5Base {
    public:
        ReadBeamHDF5();
        virtual ~ReadBeamHDF5();
        bool readGlobal(int, int, string, Setup*, Time*, bool);
        bool readSlice(double, vector<Particle>*, double*, bool);
        void close();

    private:
        hid_t fid;
        bool isOpen;
        double s0, slicelen,slen;
        int nwork,count;
        double* work;
};



#endif

