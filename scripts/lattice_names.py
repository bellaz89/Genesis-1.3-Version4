#!/usr/bin/env python3
'''
    lattice file section names
'''

LATTICE_NAMES = ['undulator', 'drift',
                 'quadrupole', 'corrector',
                 'chicane', 'phaseshifter',
                 'marker', 'line']


if __name__ == '__main__':
    for name in LATTICE_NAMES:
        print(name)
