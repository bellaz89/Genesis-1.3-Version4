/*
 *  RandomU.h
 *  Genesis
 *
 *  Created by Sven Reiche on 5/26/10.
 *  Copyright 2010 Paul Scherrer Institut. All rights reserved.
 *
 */


#ifndef __GENESIS_RANDOMU__
#define __GENESIS_RANDOMU__

#include "Sequence.h"

class RandomU : public Sequence {
public:
    RandomU(unsigned int = 0 );
    virtual ~RandomU() override {};
    void set(int idx_) override;
    double getElement() override;
private:

    int iv[32], iy;
    int iseed, iseed2;
};


#endif


