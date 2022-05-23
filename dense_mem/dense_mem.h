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

    sc_signal<bool> read_last_signal;

    void FSM() {
        status.write(0);
        while (true) {
            wait();
            if (rst_n.read() == false) {
                status.write(0);
                continue;
            }

            bool write_third_line_data =
                write_counter.read() == 0x3 && write_valid.read() && write_ready.read();
            bool read_last_data = read_last_signal.read() && read_valid.read() && read_ready.read();
            switch (status.read()) {
                case 0: {
                    if (write_third_line_data) {
                        status.write(write_config.read() ? 1 : 4);
                    }
                    break;
                }
                case 1: {
                    if (write_third_line_data) {
                        status.write(2);
                    }
                    break;
                }
                case 2: {
                    if (write_third_line_data) {
                        status.write(3);
                    }
                    break;
                }
                case 3: {
                    if (write_third_line_data) {
                        status.write(5);
                    }
                    break;
                }
                case 4:
                case 5: {
                    if (read_last_data) {
                        status.write(0);
                    }
                    break;
                }
                default: break;
            }
        }
    }

    void AccWriteCounter() {
        write_counter.write(0);
        while (true) {
            wait();
            if (rst_n.read() == false) {
                write_counter.write(0);
                continue;
            }

            if (write_valid.read() && write_ready.read()) {
                write_counter.write(write_counter.read() + 1);  // TODO: 测试这种写法是否正确
            }
        }
    }

    void WriteData() {
        for (int i = 0; i < dense_mem_height; i++) {
            for (int j = 0; j < dense_mem_width; j++) {
                mem[i][j].write(0);
            }
        }
        while (true) {
            wait();
            if (rst_n.read() == false) {
                for (int i = 0; i < dense_mem_height; i++) {
                    for (int j = 0; j < dense_mem_width; j++) {
                        mem[i][j].write(0);
                    }
                }
                continue;
            }

            if (write_valid.read() && write_ready.read()) {
                int write_line = (status.read() == 0 || status.read() == 2)
                                     ? write_counter.read()
                                     : write_counter.read() + 4;
                if (status.read() == 0 || status.read() == 1) {
                    for (int j = 0; j < dense_mem_width; j++) {
                        mem[write_line][j].write(write_data[j].read());
                    }
                } else if (status.read() == 2 || status.read() == 3) {
                    for (int j = 0; j < dense_mem_width; j++) {
                        uint16_t out =
                            HalfFloatAdd(mem[write_line][j].read(), write_data[j].read());
                        mem[write_line][j].write(out);
                    }
                }
            }
        }
    }
}