#pragma once
#include <systemc.h>

#include <fstream>
#include <string>

#include "config.h"

SC_MODULE(DenseMemMonitor) {
    std::string out_file_;

    sc_in<bool>                  clk;
    sc_in<sc_uint<data_width> > read_data;
    sc_in<bool>                 read_valid;
    sc_in<bool>                 read_ready;

    void set_out_file(const std::string& out_file) {
        this->out_file_ = out_file;
    }

    void monitor_data() {
        std::ofstream os;
        os.open(this->out_file_);
        int read_num = 0;
        while (true) {
            wait();
            if (read_valid.read() && read_ready.read()) {
                os << read_data.read() << ((read_num == dense_mem_width - 1) ? "" : " ");
                read_num++;
                if (read_num == dense_mem_width) {
                    os << std::endl;
                    read_num = 0;
                }
            }
        }
        os.close();
    }

    SC_CTOR(DenseMemMonitor) {
        SC_THREAD(monitor_data);
        sensitive_pos << clk;
    };
};