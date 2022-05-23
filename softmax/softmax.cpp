#include "softmax.h"

void Softmax::SetStatus() {
    status_reg.write(false);
    while (true) {
        wait();
        if (rst_n.read() == false) {
            status_reg.write(false);
            continue;
        }

        if (exp_out_all_valid.read()) {
            status_reg.write(!status_reg.read());
        }
    }
}

void Softmax::SetInDataReg() {
    for (int i = 0; i < softmax_input_num; i++) {
        in_data_reg[i].write(0);
    }

    while (true) {
        wait();
        if (rst_n.read() == false) {
            for (int i = 0; i < softmax_input_num; i++) {
                in_data_reg[i].write(0);
            }
            continue;
        }

        for (int i = 0; i < softmax_input_num; i++) {
            if (in_valid.read() && in_ready.read()) {
                in_data_reg[i].write(in_data[i].read());
            } else if (!status_reg.read() && sub_out_valid[i].read()) {
                in_data_reg[i].write(sub_out[i].read());
            }
        }
    }
}

void Softmax::AssignMaxTreeInValid() {
    max_tree_in_valid.write(in_valid.read() && in_ready.read());
}

void Softmax::AssignSubSignals() {
    if (!status_reg.read()) {
        sub_in2.write(max_tree_out.read());
        sub_in_valid.write(max_tree_out_valid.read());
    } else {
        sub_in2.write(log_out_data.read());
        sub_in_valid.write(log_out_valid.read());
    }
}

void Softmax::SetExpOutRegs() {
    for (int i = 0; i < softmax_input_num; i++) {
        exp_out_data_reg[i].write(0);
        exp_out_valid_reg[i].write(false);
    }

    while (true) {
        wait();
        if (rst_n.read() == false) {
            for (int i = 0; i < softmax_input_num; i++) {
                exp_out_data_reg[i].write(0);
                exp_out_valid_reg[i].write(false);
            }
            continue;
        }

        for (int i = 0; i < softmax_input_num; i++) {
            if (exp_out_valid[i].read()) {
                exp_out_data_reg[i].write(exp_out_data[i].read());
                exp_out_valid_reg[i].write(true);
            } else if (exp_out_all_valid.read()) {
                exp_out_data_reg[i].write(0);
                exp_out_valid_reg[i].write(false);
            }
        }
    }
}

void Softmax::AssignExpOutAllValid() {
    bool result = true;
    for (int i = 0; i < softmax_input_num; i++) {
        result = result && exp_out_valid_reg[i].read();
    }
    exp_out_all_valid.write(result);
}

void Softmax::AssignAddTreeInValid() {
    add_tree_in_valid.write(exp_out_all_valid.read() && !status_reg.read());
}

void Softmax::SetOut() {
    for (int i = 0; i < softmax_input_num; i++) {
        out_data[i].write(0);
    }
    out_valid.write(false);

    while (true) {
        wait();
        if (rst_n.read() == false) {
            for (int i = 0; i < softmax_input_num; i++) {
                out_data[i].write(0);
            }
            out_valid.write(false);
            continue;
        }

        if (exp_out_all_valid.read() && status_reg.read()) {
            for (int i = 0; i < softmax_input_num; i++) {
                out_data[i].write(exp_out_data_reg[i].read());
            }
            out_valid.write(true);
        } else if (out_valid.read() && out_ready.read()) {
            for (int i = 0; i < softmax_input_num; i++) {
                out_data[i].write(0);
            }
            out_valid.write(false);
        }
    }
}

void Softmax::SetInReady() {
    in_ready.write(true);
    while (true) {
        wait();
        if (rst_n.read() == false) {
            in_ready.write(true);
            continue;
        }

        if (in_valid.read() && in_ready.read()) {
            in_ready.write(false);
        } else if (out_valid.read() && out_ready.read()) {
            in_ready.write(true);
        }
    }
}