#pragma once
#include <systemc.h>

#include <cmath>

#include "config.h"

SC_MODULE(compressor_driver) {
    sc_out<bool>                  rst_n;
    sc_out<bool>                  write_valid;
    sc_out<bool>                  write_last;
    sc_out<sc_uint<group_width> > write_group;
    sc_out<bool>                  write_begin;
    sc_out<sc_uint<data_width> >  write_data[max_data_num];
    sc_out<sc_uint<size_width> >  write_size;
    sc_out<bool>                  read_ready;
    sc_out<bool>                  compress_en;
    sc_out<sc_uint<2> >           offset_width_cfg;

    void generate_input();

    void generate_reset();

    SC_CTOR(compressor_driver) {
        SC_THREAD(generate_input);
        SC_THREAD(generate_reset);
    };
};