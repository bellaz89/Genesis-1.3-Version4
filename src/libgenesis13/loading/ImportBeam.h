#ifndef __GENESIS_IMPORTBEAM__
#define __GENESIS_IMPORTBEAM__

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>
#include <map>
#include <fstream>
#include <complex>
#include <libgenesis13/util/StringProcessing.h>
#include <libgenesis13/core/Beam.h>
#include <libgenesis13/main/Setup.h>
#include <libgenesis13/main/Time.h>

using namespace std;

extern const double ce;

class ImportBeam : public StringProcessing{
    public:
        ImportBeam();
        virtual ~ImportBeam();
        bool init(int, int, map<string,string>*, Beam*, Setup*, Time*);

    private:
        void usage();
        string file;
        double offset;
        bool dotime;
};

#endif
