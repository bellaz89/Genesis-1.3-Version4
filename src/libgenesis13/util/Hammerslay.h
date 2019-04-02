/*
 *  Hammerslay.h
 *  Genesis
 *
 *  Created by Sven Reiche on 5/26/10.
 *  Copyright 2010 Paul Scherrer Institut. All rights reserved.
 *
 */

#include "Sequence.h"

#ifndef __GENESIS_HAMMERSLAY__
#define __GENESIS_HAMMERSLAY__

class Hammerslay : public Sequence {
public:
    Hammerslay(unsigned int base_in = 0);
    virtual ~Hammerslay() override {};
    void set(int idx_) override;
    double getElement() override;
private:
    int base;
    unsigned int idx;
};

#endif
