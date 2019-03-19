#!/usr/bin/env python3
'''
    Genesis4 toml to lattice converter
'''

import sys
import argparse
import toml
from lattice_names import LATTICE_NAMES

def dict2lat(dictionary):
    '''
        Convert dictionary to lattice of Genesis4 in a dict

    :param dictionary: a dictionary that represents the input TOML file
    :type dictionary: dict
    :returns: the string representation
    '''
    result = ''
    for el_type in LATTICE_NAMES[:-1]:
        if el_type in dictionary:
            for el_name in dictionary[el_type]:
                
                result += f'{el_name}: {el_type.upper()} = ' + '{'
                for key, value in dictionary[el_type][el_name].items():
                    result += f'{key} = {str(value)}, '
                
                result = result[:-2] + '}\n'
            result += '\n'

    if 'line' in dictionary:
        for el_name in dictionary['line']:
            result += f'{el_name}: LINE = ' + '{ '
            for component in dictionary['line'][el_name]:
                result += f'{component}, '
        result = result[:-2] + '}\n'
    result += '\n'
    return result

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert toml to lattice file')
    parser.add_argument('file_path', metavar='file_path', type=str,
                        nargs=1, help='path to the .toml file')
    
    dictionary = toml.load(parser.parse_args().file_path[0])
    generated = dict2lat(dictionary)
    print(generated)


