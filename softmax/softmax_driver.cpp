#include "softmax_driver.h"

#include <fstream>
#include <vector>

#include "half_float/half_float.h"

void softmax_driver::generate_input() {
    std::vector<uint16_t> inputs;
    std::ifstream         is;
    is.open("./hardware_data", std::ios_base::in);
    for (int i = 0; i < softmax_input_num; i++) {
        double input;
        is >> input;
        inputs.emplace_back(half(input).GetBits());
    }
    is.close();

    for (int i = 0; i < softmax_input_num; i++) {
        in_data[i].write(0);
    }
    in_valid.write(false);
    out_ready.write(false);

    wait(47, SC_NS);
    out_ready.write(true);

    in_valid.write(true);
    for (int i = 0; i < softmax_input_num; i++) {
        in_data[i].write(inputs[i]);
    }

    wait(10, SC_NS);
    in_valid.write(false);
}

void softmax_driver::generate_reset() {
    rst_n.write(1);
    wait(1, SC_NS);
    rst_n.write(0);
    wait(1, SC_NS);
    rst_n.write(1);
}