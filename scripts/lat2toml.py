#!/usr/bin/env python3
'''
    Genesis4 lattice to toml converter
'''

import re
import argparse
import toml
from lattice_names import LATTICE_NAMES

def lat2dict(lattice_file):
    '''
        Convert a  lattice file of Genesis4 in a dict

    :param lattice_file: a string that represents the input file
    :type lattice_file: str
    :returns: the parsed dict
    '''
    result_dict = {name : {} for name in LATTICE_NAMES}
    
    for line in [sent.replace('\n', '') for sent in lattice_file.split(';')]:
        if ':' in line:
            match = re.search('(\w+)\s*:\s*(\w+)\s*=\s*\{(.*)\}', line)
            if match is not None:
                
                el_name = match[1].strip()
                el_type = match[2].strip().lower()

                if el_type == 'line':
                    el_line_els = [el.strip() for el in match[3].split(',')]
                    result_dict['line'][el_name] = el_line_els

                else:
                    el_params = {}
                    for el_param_str in match[3].split(','):
                        if '=' in el_param_str:
                            [key, val] = el_param_str.split('=')
                            key = key.strip()
                            val = val.strip()
                            
                            if val == 'false':
                                el_params[key] = False
                                continue
                            if val == 'true':
                                el_params[key] = True
                                continue
                            
                            try:
                                el_params[key] = int(val, 0)
                                continue
                            except ValueError:
                                pass
                            
                            try:
                                el_params[key] = float(val)
                                continue
                            except ValueError:
                                pass

                            el_params[key] = val
                        
                    el_line_els = result_dict[el_type][el_name] = el_params
    for key in list(result_dict):
        if not result_dict[key]:
            del result_dict[key]

    return result_dict

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert lattice files to toml')
    parser.add_argument('file_path', metavar='file_path', type=str,
                        nargs=1, help='path to the .lat file')

    dict_transf = lat2dict(open(parser.parse_args().file_path[0]).read())
    print(toml.dumps(dict_transf))
