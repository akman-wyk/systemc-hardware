#pragma once

#include <systemc.h>

#include <cmath>
#include <iostream>
#include <unordered_map>

#include "config.h"
#include "half_float/half_float.h"
#include "util.h"

SC_MODULE(compressor) {
    // config
    sc_in<bool>        clk, rst_n;
    sc_in<bool>        compress_en;
    sc_in<sc_uint<2> > offset_width_cfg;

    // data write
    sc_in<bool>                  write_valid;
    sc_out<bool>                 write_ready;
    sc_in<bool>                  write_last;
    sc_in<sc_uint<group_width> > write_group;
    sc_in<bool>                  write_begin;
    sc_in<sc_uint<data_width> >  write_data[max_data_num];
    sc_in<sc_uint<size_width> >  write_size;

    // data read
    sc_out<bool>                       read_valid;
    sc_in<bool>                        read_ready;
    sc_out<bool>                       read_last;  // the last of all data
    sc_out<sc_uint<group_width> >      read_group;
    sc_out<bool>                       read_begin;
    sc_out<bool>                       read_data_last;  // the last of a batch of write data
    sc_out<sc_uint<data_width> >       read_data_value;
    sc_out<sc_uint<max_offset_width> > read_data_offset;

    // offset reg
    sc_signal<sc_uint<max_data_num> >     offset_mask_reg;
    sc_signal<sc_uint<max_offset_width> > offset_reg;

    // write reg
    sc_signal<bool>                  write_valid_reg;
    sc_signal<bool>                  write_last_reg;
    sc_signal<sc_uint<group_width> > write_group_reg;
    sc_signal<bool>                  write_begin_reg;
    sc_signal<sc_uint<data_width> >  write_data_reg[max_data_num];

    sc_signal<sc_uint<max_data_num> >     mask_reg;
    sc_signal<bool>                       first_data_reg;
    sc_signal<sc_uint<max_offset_width> > read_ptr_reg;

    // read signal
    sc_signal<sc_uint<max_offset_width> > read_ptr;
    sc_signal<sc_uint<max_offset_width> > read_ptr_offset;
    sc_signal<bool>                       read_zero;
    sc_signal<bool>                       read_last_signal;

    std::unordered_map<int, int> cfg_to_offset_width = {{0, 1}, {1, 2}, {2, 4}};

    void SetOffsetRegsByConfig();

    void SetReadSignals();

    void WriteData();

    void ReadData();

    SC_CTOR(compressor) {
        SC_THREAD(SetOffsetRegsByConfig);
        sensitive_pos << clk;
        sensitive_neg << rst_n;
        SC_METHOD(SetReadSignals);
        sensitive << mask_reg << read_ptr_reg << offset_mask_reg << offset_reg;
        SC_THREAD(WriteData);
        sensitive_pos << clk;
        sensitive_neg << rst_n;
        SC_METHOD(ReadData);
        sensitive << write_valid_reg << write_last_reg << write_group_reg << write_begin_reg
                  << read_last_signal << read_valid << read_ready << first_data_reg
                  << read_ptr_offset << read_zero << read_ptr;
        for (int i = 0; i < max_data_num; i++) {
            sensitive << write_data_reg[i];
        }
    }
};