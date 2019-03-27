//------------------------------------------------------------------------------
// Configuration structure for Genensis13V4
//------------------------------------------------------------------------------

#ifndef __GEN_CONFIG__
#define __GEN_CONFIG__

#include <string>
#include <vector>
#include <variant>
#include <map>
#include "LatticeConfig.h"

using std::string;
using std::variant;
using std::vector;
using std::map;

// Holds the entire configuration to pass to genesis
class Config {
public:
    // setup section
    class SetupConfig {
    public:
        string rootname;
        string lattice;
        string beamline;
        double gamma;
        double lambda0;
        double delz;
        unsigned int seed;
        unsigned int npart;
        unsigned int nbins;
        bool one4one;
        bool shotnoise;
    } setup;

    // alter_setup
    class {
    public:
        string rootname;
        string beamline;
        double delz;
        unsigned int harmonic;
        unsigned int subharmonic;
        bool resample;
    } alter_setup;

    // lattice
    class {
    public:
        double zmatch;
    } lattice;

    // time
    class {
    public:
        double s0;
        double slen;
        unsigned int sample;
        bool dependent;
    } time;

    // profile const
    class ProfileConst {
    public:
        double c0;
    };

    // profile gauss
    class ProfileGauss {
    public:
        double c0;
        double s0;
        double sig;
    };

    // profile step
    class ProfileStep {
    public:
        double c0;
        double s_start;
        double s_end;
    };

    // profile polynom
    class ProfilePolynom {
    public:
        double c0, c1, c2, c3, c4;
    };

    // profile file
    class ProfileFile {
    public:
        string xdatal;
        string ydata;
        bool is_time;
        bool reverse;
    };

    // profiles
    map<string, variant<ProfileConst,
        ProfileGauss,
        ProfileStep,
        ProfilePolynom,
        ProfileFile>> profiles;
    // beam
    class BeamConfig {
    public:
        variant<double, string> gamma;
        variant<double, string> delgam;
        variant<double, string> current;
        variant<double, string> ex;
        variant<double, string> betax, betay;
        variant<double, string> alphax, alphay;
        variant<double, string> xcenter, ycenter;
        variant<double, string> pxcenter, pycenter;
        variant<double, string> bunch;
        variant<double, string> bunchphase;
        variant<double, string> emod;
        variant<double, string> emodphase;
    } beam;

    class FieldConfig {
    public:
        double lambda;
        variant<double, string> power;
        variant<double, string> phase;
        variant<double, string> waist_pos;
        variant<double, string> waist_size;
        double xcenter, ycenter;
        double xangle, yangle;
        double dgrid;
        unsigned int ngrid;
        unsigned int harm;
        unsigned int nx, ny;
        bool accumulate;
    } field;

    class ImportDistribution {
    public:
        string file;
        bool sdds;
        double charge;
        double slicewidth;
        bool output;
        bool center;
        double gamma0;
        double x0, y0;
        double px0, py0;
        bool match;
        double betax, betay;
        double alphax, alphay;
    } import_distribution;

    class ImportBeam {} import_beam;
    class ImportField {} import_field;

    class EField {
    public:
        double rmax;
        unsigned int nz;
        unsigned int nphi;
        unsigned int ngrid;
    } efield;

    class SponRad {
    public:
        unsigned int seed;
        bool doLoss;
        bool doSpread;
    } sponrad;

    class Sort {
    public:
        bool enable;
    };

    class Write {
    public:
        string field;
        string beam;
    };

    class Track {
    public:
        double zstop;
        unsigned int output_step;
        unsigned int field_dump_step;
        unsigned beam_dump_step;
        unsigned int sort_step;
    };
};

#endif
