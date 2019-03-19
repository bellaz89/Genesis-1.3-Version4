#ifndef __GENESIS_INCOHERENT__
#define __GENESIS_INCOHERENT__

#include <vector>
#include <iostream>
#include <string>
#include <complex>
#include <cmath>
#include <libgenesis13/util/RandomU.h>
#include "Undulator.h"

class Beam;

using namespace std;

extern const double vacimp;
extern const double eev;



class Incoherent{
 public:
   Incoherent();
   virtual ~Incoherent();
   void init(int, int,bool,bool);
   void apply(Beam *,Undulator *und, double );

 private:
   bool doLoss,doSpread;
   RandomU *sran;
};

#endif
