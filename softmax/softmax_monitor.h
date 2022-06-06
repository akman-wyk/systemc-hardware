#pragma once
#include <systemc.h>

#include <fstream>
#include <string>

#include "config.h"

SC_MODULE(softmax_monitor) {
    std::string out_file_;

    sc_in<bool>     clk;
    sc_in<uint16_t> out_data[softmax_input_num];
    sc_in<bool>     out_valid;
    sc_in<bool>     out_ready;

    void set_out_file(const std::string& out_file) {
        this->out_file_ = out_file;
    }

    void monitor_data() {
        std::ofstream os;
        os.open(this->out_file_);
        while (true) {
            wait();
            if (out_valid.read() && out_ready.read()) {
                for (int i = 0; i < softmax_input_num; i++) {
                    os << out_data[i].read() << ((i == softmax_input_num - 1) ? "" : " ");
                }
                os << std::endl;
            }
        }
        os.close();
    }

    SC_CTOR(softmax_monitor) {
        SC_THREAD(monitor_data);
        sensitive_pos << clk;
    };
};