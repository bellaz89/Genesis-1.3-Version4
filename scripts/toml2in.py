#!/usr/bin/env python3
'''
    Genesis4 toml to input converter
'''

import os
import argparse
import toml
from lattice_names import LATTICE_NAMES

def dict2in(dictionary):
    '''
        Convert dictionary to input of Genesis4 in a dict

    :param dictionary: a dictionary that represents the input TOML file
    :type dictionary: dict
    :returns: the string representation
    '''
    result = ''
    for table_name in dictionary:
        if table_name not in LATTICE_NAMES:
            result += f'&{table_name}\n'
            for key, value in dictionary[table_name].items():
                result += f'{key}={value}\n'
            result += f'&end\n\n'
    
    return result

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert toml to input file')
    parser.add_argument('file_path', metavar='file_path', type=str,
                        nargs=1, help='path to the .toml file')
   
    file_path = parser.parse_args().file_path[0]
    dictionary = toml.load(file_path)
    dictionary['setup']['lattice'] = os.path.splitext(file_path)[0] + '.in'
    print(dict2in(dictionary))

