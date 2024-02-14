import os
import sys

def generate_unit_test(vsp_file, unit_file):
    unit_test = ''
    unit_test +='''
import unittest
import os, sys

curr_path = os.path.dirname(os.path.realpath(__file__))
vsp_path = os.path.join(curr_path, '../..')
sys.path.insert(1, vsp_path)

from openvsp import *

class TestOpenVSP(unittest.TestCase):
	def setUp(self):
		VSPRenew()
'''
    with open(vsp_file, 'r') as vsp:

        func_dict = {}
        lines = vsp.readlines()
        for iline, line in enumerate(lines):
            check_line = line.replace(' ','').lower()

            if 'def' in check_line and '):' in check_line and not "(self" in check_line:
                func_line = line

                idx = iline + 1
                idx_python_start = 0
                idx_python_end = 0
                check_for_python_flag = True

                second_quote = False

                while check_for_python_flag and idx < len(lines):
                    curr_line = lines[idx]

                    if '"""' in curr_line or "'''" in curr_line:
                        if not second_quote:
                            second_quote = True
                        elif second_quote:
                            idx_python_end = idx - 1
                            check_for_python_flag = False
                    elif ':rtype' in curr_line or \
                        ':return' in curr_line or \
                        ':param' in curr_line or \
                        'See also:' in curr_line or \
                        ('def' in curr_line and '):' in curr_line):
                        idx_python_end = idx - 1
                        check_for_python_flag = False

                    elif 'code-block' in curr_line and \
                        'python' in curr_line:
                        idx_python_start = idx + 1
                    else:
                        pass

                    idx += 1


                if idx_python_start != 0:
                    python_code = lines[idx_python_start:idx_python_end]
                    func_dict[add_test(func_line)] = clean_up(python_code)
                else:
                    pass


    for k, v in func_dict.items():
        if not 'def ' == k.lstrip()[:4]:
            unit_test += '\t'
        unit_test += '\t' + k.lstrip() +'\n'
        unit_test += v

    unit_test += '''
if __name__ == '__main__':
    unittest.main()
'''

    with open(unit_file, 'w') as unit:
        unit.write(unit_test)

def count_space(l):
    leading_space = l[:l.find(l.strip())]

    import math
    total_num_tab = math.ceil(leading_space.count(' ')/4) + leading_space.count('\t')

    return total_num_tab

def clean_up(code):
    space_cnt = 0
    new_code = ''
    for l in code:
        if l.strip() != '':
            space_cnt = count_space(l)
            break

    for i, l in enumerate(code):
        if l.strip() == '':
            if (i ==0 or i==len(code)):
                pass
            else:
                new_code += l
        else:
            curr_space_cnt = count_space(l)
            diff = curr_space_cnt - space_cnt
            new_code += '\t'*(diff+ 2) + l.lstrip()


    return new_code

def add_test(line):
    new_line = line[:4] + 'test_' + line[4:]
    split_line = new_line.split('(')
    new_line = split_line[0] + '(self):'
    return new_line

if __name__ == '__main__':
    base_dir = sys.argv[1]
    openvsp_dir = os.path.join(base_dir, 'openvsp')
    vsp_file = os.path.join(openvsp_dir, 'vsp.py')
    unit_file = os.path.join(openvsp_dir, 'tests', 'test_vsp_api.py')
    generate_unit_test(vsp_file, unit_file)
