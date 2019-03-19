#ifndef __GENESIS_LOADFIELD__
#define __GENESIS_LOADFIELD__

#include <vector>
#include <string>
#include <map>
#include <libgenesis13/util/StringProcessing.h>
#include <libgenesis13/main/Setup.h>
#include <libgenesis13/main/Time.h>
#include <libgenesis13/core/Field.h>
#include "Profile.h"

using namespace std;

class LoadField : public StringProcessing{
 public:
   LoadField();
   virtual ~LoadField();
   bool init(int, int, map<string,string>*, vector<Field*>*, Setup*, Time*, Profile*);

 private:
   void usage();
   double lambda,power,phase,z0,w0;
   string lambdaref,powerref,phaseref,z0ref,w0ref;
   double dgrid,xcen,ycen,xangle,yangle;
   int ngrid;
   int harm,nx,ny;
   bool add;
};

#endif
