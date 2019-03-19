#!/usr/bin/env python3
'''
    Genesis4 input and lattice to toml converter
'''

import os
import argparse
import toml
from in2toml import in2dict
from lat2toml import lat2dict

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert input and lattice files to toml')
    parser.add_argument('file_path', metavar='file_path', type=str,
                        nargs=1, help='path to the .in file.\n The .lat path is derived')

    file_path = parser.parse_args().file_path[0]
    dict_transf = in2dict(open(file_path).read())
    lat_addr = None

    if os.path.isabs(dict_transf['setup']['lattice']):
        lat_addr = dict_transf['lat']
    else:
        lat_addr = os.path.dirname(file_path) + '/' + dict_transf['setup']['lattice']

    del dict_transf['setup']['lattice']
    dict_transf.update(lat2dict(open(lat_addr).read()))
    print(toml.dumps(dict_transf)) 

