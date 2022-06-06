import os
import filecmp
from subprocess import Popen
from subprocess import PIPE
from time import sleep


def test(module_name, params, input_file, out_file, exec_file, ref_file, work_dir, run_time, check=False, config=0):
    print(f'Test {module_name}: ', end='')
    for k in params.keys():
        print(f'{k} = {params[k]}, ', end='')
    os.chdir(work_dir)
    if module_name == 'Dense Memory':
        with open('./test_log', 'w') as f_out:
            Popen(f'./{exec_file} {input_file} {out_file} {run_time} {config}',
                  stdin=PIPE, stdout=f_out, stderr=f_out, shell=True, text=True)
        sleep(0.5)
    elif module_name != 'Softmax':
        with open('./test_log', 'w') as f_out:
            Popen(f'./{exec_file} {input_file} {out_file} {run_time}',
                  stdin=PIPE, stdout=f_out, stderr=f_out, shell=True, text=True)
        sleep(0.5)
    elif not check:
        with open('./test_log', 'w') as f_out:
            Popen(f'./{exec_file} {input_file} {out_file} {run_time} 100',
                  stdin=PIPE, stdout=f_out, stderr=f_out, shell=True, text=True)
        sleep(0.5)
    else:
        with open(out_file, 'w') as fw2:
            fw2.write('')
        with open(input_file, 'r') as fr:
            for li in fr.readlines():
                with open('./softmax_test_data', 'w') as fw:
                    fw.write(li)
                with open('./test_log', 'w') as f_out:
                    Popen(f'./{exec_file} softmax_test_data softmax_test_out {run_time} 1',
                          stdin=PIPE, stdout=f_out, stderr=f_out, shell=True, text=True)
                sleep(0.1)
                with open(out_file, 'a') as fw1:
                    with open('softmax_test_out', 'r') as fr1:
                        s = fr1.readlines()
                        fw1.write(s[0])
    status = filecmp.cmp(ref_file, out_file)
    if status:
        print(f'\nResult: Pass')
    else:
        print(f'\nResult: Error')
    print('#' * 20)
    os.chdir('../..')
    return status


def hardware_test():
    test_module = [
        {
            'module_name': 'Sparse Memory',
            'test case': [
                {
                    'params': {'prune': False, 'compress': False, 'data sparsity': '0%'},
                    'input_file': 'compressor_test_data_0',
                    'out_file': 'compressor_test_out_0',
                    'exec_file': 'compressor_tb',
                    'ref_file': 'compressor_std_out_0',
                    'work_dir': './test/compressor',
                    'run_time': 10000,
                    'test': True,
                    'check': False,
                    'config': 0
                },
                {
                    'params': {'prune': True, 'compress': True, 'data sparsity': '10%'},
                    'input_file': 'compressor_test_data_1',
                    'out_file': 'compressor_test_out_1',
                    'exec_file': 'compressor_tb',
                    'ref_file': 'compressor_std_out_1',
                    'work_dir': './test/compressor',
                    'run_time': 2000,
                    'test': True,
                    'check': False,
                    'config': 0
                },
                {
                    'params': {'prune': True, 'compress': True, 'data sparsity': '30%'},
                    'input_file': 'compressor_test_data_2',
                    'out_file': 'compressor_test_out_2',
                    'exec_file': 'compressor_tb',
                    'ref_file': 'compressor_std_out_2',
                    'work_dir': './test/compressor',
                    'run_time': 4000,
                    'test': True,
                    'check': False,
                    'config': 0
                },
                {
                    'params': {'prune': True, 'compress': True, 'data sparsity': '50%'},
                    'input_file': 'compressor_test_data_3',
                    'out_file': 'compressor_test_out_3',
                    'exec_file': 'compressor_tb',
                    'ref_file': 'compressor_std_out_3',
                    'work_dir': './test/compressor',
                    'run_time': 5000,
                    'test': True,
                    'check': False,
                    'config': 0
                },
                {
                    'params': {'prune': True, 'compress': True, 'data sparsity': '80%'},
                    'input_file': 'compressor_test_data_4',
                    'out_file': 'compressor_test_out_4',
                    'exec_file': 'compressor_tb',
                    'ref_file': 'compressor_std_out_4',
                    'work_dir': './test/compressor',
                    'run_time': 7000,
                    'test': True,
                    'check': False,
                    'config': 0
                },
            ]
        },
        {
            'module_name': 'Dense Memory',
            'test case': [
                {
                    'params': {'data rows': 4},
                    'input_file': 'dense_mem_test_data_1',
                    'out_file': 'dense_mem_test_out_1',
                    'exec_file': 'dense_mem_tb',
                    'ref_file': 'dense_mem_std_out_1',
                    'work_dir': './test/dense_mem',
                    'run_time': 40000,
                    'test': True,
                    'check': False,
                    'config': 0
                },
                {
                    'params': {'data rows': 8},
                    'input_file': 'dense_mem_test_data_2',
                    'out_file': 'dense_mem_test_out_2',
                    'exec_file': 'dense_mem_tb',
                    'ref_file': 'dense_mem_std_out_2',
                    'work_dir': './test/dense_mem',
                    'run_time': 40000,
                    'test': True,
                    'check': False,
                    'config': 1
                },
            ]
        },
        {
            'module_name': 'Softmax',
            'test case': [
                {
                    'params': {'data min': -5.0, 'data max': 5.0},
                    'input_file': 'softmax_test_data_1',
                    'out_file': 'softmax_test_out_1',
                    'exec_file': 'softmax_tb',
                    'ref_file': 'softmax_std_out_1',
                    'work_dir': './test/softmax',
                    'run_time': 40000,
                    'test': True,
                    'check': False,
                    'config': 0
                },
                {
                    'params': {'data min': -10.0, 'data max': 10.0},
                    'input_file': 'softmax_test_data_2',
                    'out_file': 'softmax_test_out_2',
                    'exec_file': 'softmax_tb',
                    'ref_file': 'softmax_std_out_2',
                    'work_dir': './test/softmax',
                    'run_time': 40000,
                    'test': True,
                    'check': False,
                    'config': 0
                },
                {
                    'params': {'data min': -100.0, 'data max': 100.0},
                    'input_file': 'softmax_test_data_3',
                    'out_file': 'softmax_test_out_3',
                    'exec_file': 'softmax_tb',
                    'ref_file': 'softmax_std_out_3',
                    'work_dir': './test/softmax',
                    'run_time': 40000,
                    'test': True,
                    'check': True,
                    'config': 0
                },
            ]
        }
    ]
    all_pass = True
    for module in test_module:
        for test_case in module['test case']:
            if test_case['test']:
                test_case_pass = test(module['module_name'], test_case['params'], test_case['input_file'],
                                      test_case['out_file'], test_case['exec_file'], test_case['ref_file'],
                                      test_case['work_dir'], test_case['run_time'], test_case['check'], test_case['config'])
            else:
                test_case_pass = True
            if not test_case_pass:
                all_pass = False
    return all_pass


if __name__ == '__main__':
    print('#####Start Test#####')
    print('#' * 20)
    _all_pass = hardware_test()
    if _all_pass:
        print('######All Pass######')
    else:
        print('######Error!!!######')
