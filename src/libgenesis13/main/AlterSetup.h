#ifndef __GENESIS_ALTERSETUP__
#define __GENESIS_ALTERSETUP__

#include <vector>
#include <string>
#include <map>
#include <libgenesis13/util/StringProcessing.h>
#include <libgenesis13/lattice/Lattice.h>
#include <libgenesis13/core/Field.h>
#include <libgenesis13/core/Beam.h>
#include "Time.h"
#include "Setup.h"

using namespace std;

class Lattice;

class AlterSetup: public StringProcessing, public HDF5Base {
public:
    AlterSetup();
    virtual ~AlterSetup();
    bool init(int, map<string, string>*, Setup*, Lattice*, Time*, Beam*,
              vector<Field*>*);

private:
    void usage();
    string rootname, lattice, beamline;
    double delz;
    bool resample, disable;
    int harmonic, subharmonic, rank;
};


#endif
