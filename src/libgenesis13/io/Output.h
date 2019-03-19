#ifndef __GENESIS_OUTPUT__
#define __GENESIS_OUTPUT__

#include <string>
#include <hdf5.h>
#include <libgenesis13/core/Beam.h>
#include <libgenesis13/core/Field.h>
#include <libgenesis13/core/Undulator.h>
#include "HDF5Base.h"

using namespace std;

extern string* meta_inputfile;
extern string* meta_latfile;

class Output : public HDF5Base {
    public:
        Output();
        virtual ~Output();
        void open(string,int,int);
        void close();
        void writeFieldBuffer(Field*);
        void writeBeamBuffer(Beam*);
        void writeLattice(Beam*, Undulator*);
        void writeGlobal(double, double, double, double, bool, bool, bool);
        void writeMeta();

    private:
        void write(hsize_t, string, string, double*);
        bool noOutput;
        hid_t fid;   
};

#endif
