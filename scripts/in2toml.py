#!/usr/bin/env python3
'''
    Genesis4 input to toml converter
'''

import sys
import toml
import argparse

def in2dict(input_file):
    '''
        Convert an input file of Genesis4 in a dict

    :param input_file: a string that represents the input file
    :type input_file: str
    :returns: the parsed dict
    '''
    result_dict = {}
    tables_str = input_file.replace('&end', '').split('&')
    for table_str in tables_str:
        lines = table_str.split('\n')
        table_name = lines[0].strip()
        if table_name != '':
            result_dict[table_name] = {}
            for line in lines:
                if '=' in line:
                    [key, val] = line.split('=')
                    key = key.strip()
                    val = val.strip()

                    if val == 'false':
                        result_dict[table_name][key] = False
                        continue
                    if val == 'true':
                        result_dict[table_name][key] = True
                        continue
                    try:
                        result_dict[table_name][key] = int(val, 0)
                        continue
                    except ValueError:
                        pass
 
                    try:
                        result_dict[table_name][key] = float(val)
                        continue
                    except ValueError:
                        pass

                    result_dict[table_name][key] = val

    return result_dict

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert input files to toml')
    parser.add_argument('file_path', metavar='file_path', type=str,
                        nargs=1, help='path to the .in file')

    dict_transf = in2dict(open(parser.parse_args().file_path[0]).read())
    print(toml.dumps(dict_transf))
