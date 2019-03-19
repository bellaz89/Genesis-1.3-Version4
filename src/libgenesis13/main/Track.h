#ifndef __GENESIS_TRACK__
#define __GENESIS_TRACK__

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <map>
#include <cstdlib>
#include <libgenesis13/lattice/AlterLattice.h>
#include <libgenesis13/lattice/Lattice.h>
#include <libgenesis13/core/Beam.h>
#include <libgenesis13/core/Field.h>
#include "Setup.h"
#include "Time.h"
using namespace std;

class Track{
    public:
        Track();
        virtual ~Track();
        bool init(int, int, map<string,string> *,Beam *, vector<Field *> *,Setup *, Lattice *, AlterLattice *, Time *,bool);
    private:
        void usage();
        double zstop,slen,s0;
        int output_step,dumpFieldStep,dumpBeamStep,sort_step,bunchharm;
        int rank, size;
};


#endif
