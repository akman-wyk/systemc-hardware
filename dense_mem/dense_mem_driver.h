#pragma once
#include <systemc.h>

#include <string>

#include "config.h"

SC_MODULE(DenseMemDriver) {
    std::string data_file_;
    int         config_;

    sc_in<bool> write_ready;

    sc_out<bool>                 rst_n;
    sc_out<bool>                 write_config;
    sc_out<sc_uint<data_width> > write_data[dense_mem_width];
    sc_out<bool>                 write_valid;
    sc_out<bool>                 read_ready;

    void set_data_file(const std::string& data_file, int config);

    void generate_input();

    void generate_reset();

    SC_CTOR(DenseMemDriver) {
        SC_THREAD(generate_input);
        SC_THREAD(generate_reset);
    };
};