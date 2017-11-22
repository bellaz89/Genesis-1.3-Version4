#ifndef __GENESIS_BEAM__
#define __GENESIS_BEAM__

#include <vector>
#include <iostream>
#include <string>

#include "Particle.h"
#include "Undulator.h"
#include "BeamSolver.h"
#include "Incoherent.h"
#include "Sorting.h"

using namespace std;

extern const double ce;

class Beam{
 public:
   Beam();
   virtual ~Beam();
   void initDiagnostics(int);
   void diagnostics(bool,double);
   void diagnosticsStart();
   void init(int, int, double,double, double,bool);
   void initSorting(int,int,bool,bool);
   void initEField(double,int,int,int,double);
   bool harmonicConversion(int,bool);
   bool subharmonicConversion(int,bool);
   int sort();
   void track(double, vector<Field *> *, Undulator *);

   vector< vector<Particle> > beam;
   vector<double> current;
   double reflength,slicelength;   // for conversion of theta in Particle to real position
   double s0;         // starting position of the time-window
   bool one4one;     // flag whether one4one simulation is done
   int nbins;

   // output buffer
   vector<double> zpos,gavg,gsig,xavg,xsig,yavg,ysig,pxavg,pyavg,bunch,bphi;
   vector<double> bx,by,ax,ay,ex,ey,cu;
   
 private:
   BeamSolver solver;
   Incoherent incoherent;
   Sorting sorting;
   int idx;
};


inline void Beam::initEField(double rmax, int ngrid, int nz, int nphi, double lambda){
  solver.initEField(rmax,ngrid,nz,nphi,lambda);
  return;
}

#endif
