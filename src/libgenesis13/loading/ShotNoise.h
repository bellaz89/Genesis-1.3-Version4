/*
 *  QuietLoading.h
 *  Genesis
 *
 *  Created by Sven Reiche on 5/14/10.
 *  Copyright 2010 Paul Scherrer Institut. All rights reserved.
 *
 */

#include <libgenesis13/core/Particle.h>
#include <libgenesis13/util/RandomU.h>

#ifndef __GENESIS_SHOTNOISE__
#define __GENESIS_SHOTNOISE__

using namespace std;

class ShotNoise  {
    public:
        ShotNoise();
        ~ShotNoise();
        void applyShotNoise(Particle *beam, int, int, double); 
        void init(int,int); 
    private:
        RandomU *sran;
        double *work;
        int nwork;
};

#endif
