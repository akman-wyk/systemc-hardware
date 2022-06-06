#pragma once

#include <systemc.h>

#include <cstdlib>
#include <iomanip>
#include <random>

#include "half_float.h"

SC_MODULE(Prune) {
    sc_in<bool> clk, rst_n;
    sc_in<bool> prune_en;

    sc_in<sc_uint<data_width> > prune_threshold;
    sc_in<sc_uint<data_width> > seed_data;

    sc_in<sc_uint<data_width> > in_data;
    sc_in<bool>                 in_valid;

    sc_out<sc_uint<data_width> > out_data;
    sc_out<bool>                 out_valid;

    sc_signal<sc_uint<data_width> > random_data;

    void gen_random() {
        static std::random_device                     rd;
        static std::default_random_engine             eng(rd());
        static std::uniform_real_distribution<double> dis(0.0, 1.0);

        random_data.write(seed_data.read());
        srand(seed_data.read());
        while (true) {
            wait();
            if (rst_n.read() == false) {
                random_data.write(seed_data.read());
                srand(seed_data.read());
                continue;
            }

            double rd_num = dis(eng);
            random_data.write(half(rd_num).GetBits());
        }
    }

    void prune() {
        out_data.write(0);
        out_valid.write(false);

        while (true) {
            wait();
            if (rst_n.read() == false) {
                out_data.write(0);
                out_valid.write(false);
                continue;
            }

            if (in_valid.read()) {
                if (prune_en.read()) {
                    uint16_t in_data_abs = in_data.read();
                    if (HalfFloatCompare(in_data_abs, 0) == -1) {
                        in_data_abs = HalfFloatSub(0, in_data_abs);
                    }
                    uint16_t random_threshold = HalfFloatMul(random_data.read(), prune_threshold.read());
                    if (HalfFloatCompare(in_data_abs, random_threshold) == -1) {
                        out_data.write(0);
                    } else if (HalfFloatCompare(in_data_abs, prune_threshold.read())) {
                        if (HalfFloatCompare(in_data.read(), 0) == -1) {
                            out_data.write(HalfFloatSub(0, prune_threshold.read()));
                        } else {
                            out_data.write(prune_threshold.read());
                        }
                    } else {
                        out_data.write(in_data.read());
                    }
                } else {
                    out_data.write(in_data.read());
                }
            }
            out_valid.write(in_valid.read());
        }
    }

    SC_CTOR(Prune) {
        SC_THREAD(gen_random);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(prune);
        sensitive_pos << clk;
        sensitive_neg << rst_n;
    }
}