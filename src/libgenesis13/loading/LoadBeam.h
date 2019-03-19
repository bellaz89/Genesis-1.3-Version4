#ifndef __GENESIS_LOADBEAM__
#define __GENESIS_LOADBEAM__

#include <string>
#include <map>
#include <libgenesis13/util/StringProcessing.h>
#include <libgenesis13/core/Beam.h>
#include <libgenesis13/main/Setup.h>
#include <libgenesis13/main/Time.h>
#include <libgenesis13/lattice/Lattice.h>
#include "Profile.h"

using namespace std;

extern const double ce;

class LoadBeam : public StringProcessing{
    public:
        LoadBeam();
        virtual ~LoadBeam();
        bool init(int, int, map<string,string>*, Beam*, Setup*, Time*, Profile*, Lattice*);

    private:
        void usage();
        double gamma, delgam, ex, ey, betax, betay, alphax, alphay;
        double xcen, ycen, pxcen, pycen, current, bunch, bunchphase;
        double emod, emodphase;
        string gammaref, delgamref, exref, eyref, betaxref, betayref;
        string alphaxref, alphayref, xcenref, ycenref, pxcenref, pycenref;
        string currentref, bunchref, bunchphaseref, emodref, emodphaseref; 

};


#endif
