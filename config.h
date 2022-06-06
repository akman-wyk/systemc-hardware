#pragma once
#include <cmath>

const int max_data_num = 16;
const int group_width = 4;
const int data_width = 16;
const int size_width = (int)ceilf(log2f((float)(max_data_num + 1)));
const int max_offset_width = (int)ceilf(log2f((float)(max_data_num)));

const int multiplier_delay = 1;
const int add_delay = 1;
const int sub_delay = 1;
const int max_tree_delay = 4;
const int add_tree_delay = 4;

const int softmax_input_num = 10;
const int exp_linear_scope_num = 10;
const int log_linear_scope_num = 50;

const int dense_mem_width = 16;
const int dense_mem_height = 8;

//test
const int test_data_num_for_compressor = 50;
const int test_data_num_for_softmax = 100;
const int test_data_num_for_dense_mem = 160;