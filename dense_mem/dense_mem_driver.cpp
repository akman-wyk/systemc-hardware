#include "dense_mem_driver.h"

#include <fstream>
#include <vector>

#include "config.h"

void DenseMemDriver::set_data_file(const std::string& data_file, int config) {
    this->data_file_ = data_file;
    this->config_ = config;
}

void DenseMemDriver::generate_input() {
    std::vector<std::vector<uint16_t> > input_data;
    std::ifstream                       is;
    is.open(this->data_file_);
    for (int i = 0; i < test_data_num_for_dense_mem; i++) {
        std::vector<uint16_t> inputs;
        for (int j = 0; j < dense_mem_width; j++) {
            uint16_t data;
            is >> data;
            inputs.emplace_back(data);
        }
        input_data.emplace_back(std::move(inputs));
    }
    is.close();

    write_config.write(this->config_ == 1);

    write_valid.write(false);
    read_ready.write(false);
    for (int i = 0; i < dense_mem_width; i++) {
        write_data[i].write(0);
    }

    wait(47, SC_NS);
    read_ready.write(true);

    for (int i = 0; i < test_data_num_for_dense_mem; i++) {
        write_valid.write(true);
        for (int j = 0; j < dense_mem_width; j++) {
            write_data[j].write(input_data[i][j]);
        }

        wait(10, SC_NS);
        write_valid.write(false);

        while (!write_ready.read()) {
            wait(10, SC_NS);
        }
    }
}

void DenseMemDriver::generate_reset() {
    rst_n.write(1);
    wait(1, SC_NS);
    rst_n.write(0);
    wait(1, SC_NS);
    rst_n.write(1);
}