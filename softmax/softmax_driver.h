#pragma once
#include <systemc.h>

#include <cstdint>

#include "config.h"

SC_MODULE(softmax_driver) {
    sc_out<bool>     rst_n;
    sc_out<uint16_t> in_data[softmax_input_num];
    sc_out<bool>     in_valid;
    sc_out<bool>     out_ready;

    void generate_input();

    void generate_reset();

    SC_CTOR(softmax_driver) {
        SC_THREAD(generate_input);
        SC_THREAD(generate_reset);
    }
};