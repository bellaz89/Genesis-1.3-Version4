#ifndef __GENESIS_SPONRAD__
#define __GENESIS_SPONRAD__

#include <vector>
#include <string>
#include <map>
#include <libgenesis13/util/StringProcessing.h>
#include <libgenesis13/core/Particle.h>
#include "Time.h"
#include "Setup.h"

using namespace std;

class Beam;

class SponRad: public StringProcessing {
public:
    SponRad();
    virtual ~SponRad();
    bool init(int, int, map<string, string>*, Beam*);

private:
    void usage();
    bool doLoss, doSpread;
    int seed;
};

#endif
