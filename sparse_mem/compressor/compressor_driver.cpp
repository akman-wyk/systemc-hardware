#include "compressor_driver.h"

#include <fstream>
#include <vector>

#include "config.h"

void compressor_driver::set_data_file(const std::string& data_file) {
    this->data_file_ = data_file;
}

void compressor_driver::generate_input() {
    std::vector<std::vector<uint16_t> > input_data;
    std::ifstream                       is;
    is.open(this->data_file_);
    for (int i = 0; i < test_data_num_for_compressor; i++) {
        std::vector<uint16_t> inputs;
        for (int j = 0; j < max_data_num; j++) {
            uint16_t data;
            is >> data;
            inputs.emplace_back(data);
        }
        input_data.emplace_back(std::move(inputs));
    }
    is.close();

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

    for (int i = 0; i < test_data_num_for_compressor; i++) {
        write_valid.write(true);
        write_last.write((i == test_data_num_for_compressor - 1));
        write_group.write(i);
        write_begin.write((i == 0));
        write_size.write(16);
        for (int j = 0; j < max_data_num; j++) {
            write_data[j].write(input_data[i][j]);
        }

        wait(10, SC_NS);
        write_valid.write(false);

        while (!write_ready.read()) {
            wait(10, SC_NS);
        }
    }
}

void compressor_driver::generate_reset() {
    rst_n.write(1);
    wait(1, SC_NS);
    rst_n.write(0);
    wait(1, SC_NS);
    rst_n.write(1);
}