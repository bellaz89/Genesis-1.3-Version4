//------------------------------------------------------------------------------
// Lattice configuration structure for Genensis13V4
//------------------------------------------------------------------------------

#ifndef __GEN_LATTICE_CONFIG__
#define __GEN_LATTICE_CONFIG__

#include<string>
#include<tuple>
#include<variant>
#include<vector>

using std::string;
using std::tuple;
using std::variant;
using std::vector;

class LatticeElementConfig {
public:
    string name;
};

class UndulatorConfig : LatticeElementConfig {
public:
    double aw;
    double lambdau;
    unsigned int nwig;
    bool helical;
    double kx;
    double ky;
    double ax;
    double ay;
    double gradx;
};

class DriftConfig : LatticeElementConfig {
public:
    double l;
};

class QuadrupoleConfig : LatticeElementConfig {
public:
    double l;
    double k1;
    double dx;
    double sy;
};

class CorrectorConfig : LatticeElementConfig {
public:
    double l;
    double cx;
    double cy;
};

class ChicaneConfig : LatticeElementConfig {
public:
    double l;
    double lb;
    double ld;
    double delay;
};

class PhaseShifterConfig : LatticeElementConfig {
public:
    double l;
    double phi;
};

class MarkerConfig : LatticeElementConfig {
public:
    bool dumpfield;
    bool dumpbeam;
    bool sort;
    bool stop;
};

class LineConfig : LatticeElementConfig {
    vector<variant<UndulatorConfig,
           DriftConfig,
           QuadrupoleConfig,
           CorrectorConfig,
           ChicaneConfig,
           PhaseShifterConfig,
           MarkerConfig,
           LineConfig>> elements;
};

#endif

