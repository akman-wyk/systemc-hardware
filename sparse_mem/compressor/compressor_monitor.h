#pragma once
#include <systemc.h>

#include <cmath>
#include <fstream>
#include <string>

#include "config.h"

SC_MODULE(compressor_monitor) {
    std::string out_file_;

    sc_in<bool>                       clk;
    sc_in<sc_uint<data_width> >       read_data_value;
    sc_in<sc_uint<max_offset_width> > read_data_offset;
    sc_in<bool>                       read_valid;
    sc_in<bool>                       read_ready;

    void set_out_file(const std::string& out_file) {
        this->out_file_ = out_file;
    }

    void monitor_data() {
        std::ofstream os;
        os.open(this->out_file_);
        while (true) {
            wait();
            if (read_valid.read() && read_ready.read()) {
                os << read_data_value.read() << " " << read_data_offset.read() << std::endl;
            }
        }
        os.close();
    }

    SC_CTOR(compressor_monitor) {
        SC_THREAD(monitor_data);
        sensitive_pos << clk;
    };
};