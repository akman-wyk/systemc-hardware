#pragma once
#include <systemc.h>

#include <cstdint>
#include <string>

#include "config.h"

SC_MODULE(softmax_driver) {
    std::string data_file_;
    int data_set_num_;

    sc_in<bool> in_ready;

    sc_out<bool>     rst_n;
    sc_out<uint16_t> in_data[softmax_input_num];
    sc_out<bool>     in_valid;
    sc_out<bool>     out_ready;

    void set_data_file(const std::string& data_file, int data_set_num);

    void generate_input();

    void generate_reset();

    SC_CTOR(softmax_driver) {
        SC_THREAD(generate_input);
        SC_THREAD(generate_reset);
    }
};