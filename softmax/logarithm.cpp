#include "logarithm.h"

#include <cmath>

const uint16_t Logarithm::ln2 = half(0.6931471805599453).GetBits();

std::vector<std::unordered_map<std::string, uint16_t>> Logarithm::GetLinearLog2Params(int n) {
    std::vector<std::unordered_map<std::string, uint16_t>> params;
    for (int i = 0; i < n; i++) {
        double x1 = (i + 0.0) / n + 1.0;
        double x2 = (i + 1.0) / n + 1.0;
        double k = (log2(x2) - log2(x1)) / (x2 - x1);
        double b1 = log2(x1) - k * x1;

        double x_ = 1.0 / log(2) / k;
        double b2 = log2(x_) - k * x_;

        double b = (b1 + b2) / 2.0;

        params.emplace_back(std::unordered_map<std::string, uint16_t>{{"x1", half{x1}.GetBits()},
                                                                      {"x2", half{x2}.GetBits()},
                                                                      {"k", half{k}.GetBits()},
                                                                      {"b", half{b}.GetBits()}});
    }
    return std::move(params);
}

void Logarithm::AssignM() {
    m.write((in_data.read() & 0x3ff) | 0x3c00);
}

void Logarithm::GetMEReg() {
    e_reg.write(0);
    m_reg.write(0);

    while (true) {
        wait();
        if (rst_n.read() == false) {
            e_reg.write(0);
            m_reg.write(0);
            continue;
        }

        uint16_t e_src = ((in_data.read() >> 10) & 0x1f);
        uint16_t e_result;
        if (e_src == 0xf) {
            e_result = 0;
        } else {
            uint16_t e_s = ((e_src >> 4) & 0x1) ? 0x0 : 0x1;
            uint16_t e_src_abs = (e_s) ? 0xf - e_src : e_src - 0xf;

            // hardware impl
            // uint16_t e_src_abs_0 = e_src_abs & 0x1;
            // uint16_t e_src_abs_1 = e_src_abs & 0x2;
            // uint16_t e_src_abs_2 = e_src_abs & 0x4;
            // uint16_t e_src_abs_3 = e_src_abs & 0x8;

            // uint16_t switch_0 = e_src_abs_3 | (~e_src_abs_2 & ~e_src_abs_1);
            // uint16_t switch_1 = ~e_src_abs_3 & ~e_src_abs_2;
            // switch ((switch_1 << 1) | switch_0)
            uint16_t e_e, e_m;
            if (e_src_abs & 0x8) {
                e_e = 0x12;
                e_m = (e_src_abs & 0x7) << 7;
            } else if (e_src_abs & 0x4) {
                e_e = 0x11;
                e_m = (e_src_abs & 0x3) << 8;
            } else if (e_src_abs & 0x2) {
                e_e = 0x10;
                e_m = (e_src_abs & 0x1) << 9;
            } else {
                e_e = 0x0f;
                e_m = 0;
            }

            e_result = (e_s << 15) | (e_e << 10) | e_m;
        }

        if (in_valid.read()) {
            e_reg.write(e_result);
            m_reg.write(m.read());
        }
    }
}

void Logarithm::GetKB() {
    k_reg.write(0);
    b_reg.write(0);
    kb_valid_reg.write(false);

    while (true) {
        wait();
        if (rst_n.read() == false) {
            k_reg.write(0);
            b_reg.write(0);
            kb_valid_reg.write(false);
            continue;
        }

        uint16_t k_list[log_linear_scope_num];
        uint16_t b_list[log_linear_scope_num];
        for (int i = 0; i < log_linear_scope_num; i++) {
            if (params_[i]["x1"] <= m.read() && m.read() < params_[i]["x2"]) {
                k_list[i] = params_[i]["k"];
                b_list[i] = params_[i]["b"];
            } else {
                k_list[i] = 0;
                b_list[i] = 0;
            }
        }

        uint16_t k_result = 0;
        uint16_t b_result = 0;
        for (int i = 0; i < log_linear_scope_num; i++) {
            k_result |= k_list[i];
            b_result |= b_list[i];
        }

        if (in_valid.read()) {
            k_reg.write(k_result);
            b_reg.write(b_result);
        }
        kb_valid_reg.write(in_valid.read());
    }
}

void Logarithm::SetMultStatus() {
    mult_status_reg.write(false);
    while (true) {
        wait();
        if (rst_n.read() == false) {
            mult_status_reg.write(false);
            continue;
        }

        if (mult_out_valid.read()) {
            mult_status_reg.write(!mult_status_reg.read());
        }
    }
}

void Logarithm::AssignMultSignals() {
    if (!mult_status_reg.read()) {
        mult_in1.write(m_reg.read());
        mult_in2.write(k_reg.read());
        mult_in_valid.write(kb_valid_reg.read());
    } else {
        mult_in1.write(add_out_2.read());
        mult_in2.write(ln2);
        mult_in_valid.write(add_out_valid_2.read());
    }
}

void Logarithm::AssignAdder1Valid() {
    add_in_valid_1.write(!mult_status_reg.read() && mult_out_valid.read());
}

void Logarithm::AssignOut() {
    if (mult_status_reg.read() && mult_out_valid.read()) {
        out_data.write(mult_out.read());
        out_valid.write(true);
    } else {
        out_data.write(0);
        out_valid.write(false);
    }
}