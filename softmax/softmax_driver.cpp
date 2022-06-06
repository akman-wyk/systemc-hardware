#include "softmax_driver.h"

#include <fstream>
#include <vector>

#include "half_float/half_float.h"

void softmax_driver::set_data_file(const std::string& data_file, int data_set_num) {
    this->data_file_ = data_file;
    this->data_set_num_ = data_set_num;
}

void softmax_driver::generate_input() {
    std::vector<std::vector<uint16_t> > input_data;
    std::ifstream                       is;
    is.open(this->data_file_);
    for (int i = 0; i < this->data_set_num_; i++) {
        std::vector<uint16_t> inputs;
        for (int j = 0; j < softmax_input_num; j++) {
            uint16_t data;
            is >> data;
            inputs.emplace_back(data);
        }
        input_data.emplace_back(std::move(inputs));
    }
    is.close();

    for (int i = 0; i < softmax_input_num; i++) {
        in_data[i].write(0);
    }
    in_valid.write(false);
    out_ready.write(false);

    wait(47, SC_NS);
    out_ready.write(true);

    for (int i = 0; i < test_data_num_for_softmax; i++) {
        in_valid.write(true);
        for (int j = 0; j < softmax_input_num; j++) {
            in_data[j].write(input_data[i][j]);
        }

        wait(10, SC_NS);
        in_valid.write(false);

        while (!in_ready.read()) {
            wait(10, SC_NS);
        }
    }
}

void softmax_driver::generate_reset() {
    rst_n.write(1);
    wait(1, SC_NS);
    rst_n.write(0);
    wait(1, SC_NS);
    rst_n.write(1);
}