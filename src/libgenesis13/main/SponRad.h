#ifndef __GENESIS_SPONRAD__
#define __GENESIS_SPONRAD__

#include <vector>
#include <iostream>
#include <string>
#include <complex>
#include <libgenesis13/util/StringProcessing.h>
#include <libgenesis13/core/Particle.h>
#include "Time.h"
#include "Setup.h"

class Beam;

using namespace std;

class SponRad: public StringProcessing{
 public:
   SponRad();
   virtual ~SponRad();
   bool init(int, int, map<string, string>*, Beam*);

 private:
   void usage();
   bool doLoss,doSpread;
   int seed;
};

#endif
