#pragma once

#include <systemc.h>

#include "half_float/half_float.h"
#include "util.h"

SC_MODULE(DenseMem) {
    sc_in<bool>                  clk, rst_n;
    sc_in<bool>                  write_config;  // 0: 4 * 16, 1: 8 * 16
    sc_in<sc_uint<data_width> >  write_data[dense_mem_width];
    sc_in<bool>                  write_valid;
    sc_out<bool>                 write_ready;
    sc_out<sc_uint<data_width> > read_data;
    sc_out<bool>                 read_valid;
    sc_in<bool>                  read_ready;

    sc_signal<sc_uint<data_width> > mem[dense_mem_height][dense_mem_width];
    sc_signal<sc_uint<3> >          status;
    sc_signal<sc_uint<2> >          write_counter;

    sc_signal<bool>        read_last_signal;
    sc_signal<bool>        read_row_last_signal;
    sc_signal<sc_uint<4> > read_col;
    sc_signal<sc_uint<3> > read_row;
    sc_signal<bool>        read_valid_signal;

    void FSM();

    void AccWriteCounter();

    void WriteData();

    void ReadData();

    void AssignReadSignals();

    void AssignOutSignals();

    SC_CTOR(DenseMem) {
        SC_THREAD(FSM);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(AccWriteCounter);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(WriteData);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(ReadData);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_METHOD(AssignReadSignals);
        sensitive << read_valid << read_ready << read_col << status << read_row;

        SC_METHOD(AssignOutSignals);
        sensitive << status;
    }
};