#pragma once

#include <systemc.h>

#include "half_float.h"

SC_MODULE(vector_mul) {
    sc_in<bool> clk, rst_n;
    sc_in<bool> acc_en;
    sc_in<bool> acc_direct;

    sc_in<sc_uint<16> > acc_finish;
    sc_in<sc_uint<16> > acc_in_data;
    sc_in<bool>         acc_in_valid;
    sc_out<bool>        acc_in_ready;

    sc_in<sc_uint<16> > acc_prune_threshold;
    sc_in<bool>         acc_prune_valid;
    sc_in<sc_uint<16> > acc_seed_data;

    sc_out<sc_uint<16> > acc_out_data;
    sc_out<bool>         acc_out_valid;
    sc_in<bool>          acc_out_ready;

    sc_out<sc_uint<16> > acc_out_mean_data;
    sc_out<bool>         acc_out_mean_valid;

    void accumulate() {
        uint16_t temp = 0.0;
        uint16_t cnt = 0;
        while (true) {
            wait();
            if (acc_in_valid.read() && acc_in_ready.read()) {
                if (cnt < acc_finish.read()) {
                    temp = HalfFloatAdd(temp, acc_in_data.read());
                    cnt++;
                } else {
                    acc_out_mean_data.write(HalfFloatDiv(temp, acc_finish.read()));
                    acc_out_mean_valid.write(true) temp = 0;
                    cnt = 0;
                }
            }
        }
    }

    uint16_t gen_random();

    void prune();
}