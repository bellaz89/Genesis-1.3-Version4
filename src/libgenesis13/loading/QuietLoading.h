#ifndef __GENESIS_QUIETLOADING__
#define __GENESIS_QUIETLOADING__

#include <memory>
#include <libgenesis13/core/Particle.h>
#include <libgenesis13/util/Sequence.h>

typedef struct {
    double gamma;
    double delgam;
    double current;
    double ex;
    double ey;
    double xcen;
    double ycen;
    double pxcen;
    double pycen;
    double betax;
    double betay;
    double alphax;
    double alphay;
    double bunch;
    double bunchphase;
    double emod;
    double emodphase;
} BeamSlice;

class QuietLoading  {
public:
    void loadQuiet(Particle* beam, BeamSlice*, int, int, double, int);
    void init(bool, int*);

private:
    std::shared_ptr<Sequence> sx, sy, st, spx, spy, sg;
};

#endif
