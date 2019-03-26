#ifndef __GENESIS_PHYSICAL_CONSTANTS__
#define __GENESIS_PHYSICAL_CONSTANTS__
#include <cmath>

// Impedance of free space Z0~346.730 Ohm
#define VACUUM_IMPEDANCE (119.9169832*M_PI)
// From NIST (https://physics.nist.gov/cgi-bin/cuu/Value?mec2mev) . Previous value 510999.06
#define ELECTRON_MASS_EV 510998.9461
// Electron charge in A.s
#define ELECTRON_CHARGE 1.602176620898e-19
// Speed of light in m/s
#define LIGHT_SPEED 299792458 
// Electron charge multiplied by the speed of light in A.m
#define ELECTRON_CHARGE_X_C (ELECTRON_CHARGE*LIGHT_SPEED)

#endif
