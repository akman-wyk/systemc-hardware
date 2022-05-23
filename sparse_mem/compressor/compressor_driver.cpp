#include "compressor_driver.h"

void compressor_driver::generate_input() {
    compress_en.write(true);
    offset_width_cfg.write(2);

    write_valid.write(false);
    write_last.write(false);
    write_group.write(0);
    write_begin.write(false);
    write_size.write(0);
    read_ready.write(false);
    for (int i = 0; i < max_data_num; i++) {
        write_data[i].write(0);
    }

    wait(47, SC_NS);
    read_ready.write(true);

    write_valid.write(true);
    write_last.write(false);
    write_group.write(0);
    write_begin.write(true);
    write_size.write(16);
    for (int i = 0; i < max_data_num; i++) {
        if (i % 3 == 0) {
            write_data[i].write(i);
        } else {
            write_data[i].write(0);
        }
    }

    wait(50, SC_NS);
    write_valid.write(false);

    wait(100, SC_NS);
    write_valid.write(true);
    write_last.write(false);
    write_group.write(1);
    write_begin.write(false);
    write_size.write(16);
    for (int i = 0; i < max_data_num; i++) {
        if (i % 3 == 1) {
            write_data[i].write(i);
        } else {
            write_data[i].write(0);
        }
    }

    wait(50, SC_NS);
    write_valid.write(false);

    wait(100, SC_NS);
    write_valid.write(true);
    write_last.write(true);
    write_group.write(2);
    write_begin.write(false);
    write_size.write(16);
    for (int i = 0; i < max_data_num; i++) {
        if (i % 3 == 2) {
            write_data[i].write(i);
        } else {
            write_data[i].write(0);
        }
    }

    wait(50, SC_NS);
    write_valid.write(false);
}

void compressor_driver::generate_reset() {
    rst_n.write(1);
    wait(1, SC_NS);
    rst_n.write(0);
    wait(1, SC_NS);
    rst_n.write(1);
}