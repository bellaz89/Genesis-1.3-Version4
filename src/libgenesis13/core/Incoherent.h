#ifndef __GENESIS_INCOHERENT__
#define __GENESIS_INCOHERENT__

#include <libgenesis13/util/RandomU.h>
#include "Undulator.h"

using namespace std;

extern const double vacimp;
extern const double eev;

class Beam;

class Incoherent{
    public:
        Incoherent();
        virtual ~Incoherent();
        void init(int, int, bool, bool);
        void apply(Beam*, Undulator* und, double);

    private:
        bool doLoss, doSpread;
        RandomU* sran;
};

#endif
