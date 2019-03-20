#ifndef __GENESIS_GENCORE__
#define __GENESIS_GENCORE__

#include <vector>
#include <mpi.h>
#include "Beam.h"
#include "Field.h"
#include "Undulator.h"
#include "Control.h"

using namespace std;

class Gencore {
public:
    Gencore() {};
    virtual ~Gencore() {};
    int run(const char*, Beam*, vector<Field*>*, Undulator*, bool, bool, bool);
};

#endif
