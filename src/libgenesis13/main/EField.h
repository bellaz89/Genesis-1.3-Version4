#ifndef __GENESIS_EFIELD__
#define __GENESIS_EFIELD__

#include <vector>
#include <iostream>
#include <string>
#include <complex>
#include <libgenesis13/main/Time.h>
#include <libgenesis13/main/Setup.h>
#include <libgenesis13/core/Particle.h>

class Beam;

using namespace std;

class EField{
    public:
        EField();
        virtual ~EField();
        bool init(int,int,map<string,string>*, Beam*, Setup*, Time*);


    private:
        void usage();
        double rmax,dr,lambda;
        int nz,nphi,ngrid;

};

#endif
