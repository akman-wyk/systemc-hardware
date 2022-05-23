#include "exponent.h"

#include <cmath>
#include <iostream>

const uint16_t Exponent::min_input = half(-9.70405981103949).GetBits();
const uint16_t Exponent::max_input = half(11.089866488461016).GetBits();
const uint16_t Exponent::log2e = half(1.4426950408889634).GetBits();

std::vector<std::unordered_map<std::string, uint16_t>> Exponent::GetLinearExp2Params(int n) {
    std::vector<std::unordered_map<std::string, uint16_t>> params;
    for (int i = 0; i < n; i++) {
        double x1 = (i + 0.0) / n;
        double x2 = (i + 1.0) / n;
        double k = (exp2(x2) - exp2(x1)) / (x2 - x1);
        double b1 = exp2(x1) - k * x1;

        double x_ = log2(k) - log2(log(2));
        double b2 = exp2(x_) - k * x_;

        double b = (b1 + b2) / 2.0;

        params.emplace_back(std::unordered_map<std::string, uint16_t>{{"x1", half{x1}.GetBits()},
                                                                      {"x2", half{x2}.GetBits()},
                                                                      {"k", half{k}.GetBits()},
                                                                      {"b", half{b}.GetBits()}});
    }
    return std::move(params);
}

void Exponent::SetDataValidReg() {
    data_reg.write(0);
    valid_reg.write(false);
    output_zero_reg.write(false);

    while (true) {
        wait();
        if (rst_n.read() == false) {
            data_reg.write(0);
            valid_reg.write(false);
            output_zero_reg.write(false);
            continue;
        }

        if (in_valid.read() && HalfFloatCompare(in_data.read(), min_input) == 1) {
            data_reg.write(in_data.read());
            valid_reg.write(true);
        } else {
            data_reg.write(0);
            valid_reg.write(false);
        }
        output_zero_reg.write(in_valid.read() && HalfFloatCompare(in_data.read(), min_input) == -1);
    }
}

void Exponent::SetMultStatus() {
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

void Exponent::AssignMultSignals() {
    if (!mult_status_reg.read()) {
        mult_in1.write(data_reg.read());
        mult_in2.write(log2e);
        mult_in_valid.write(valid_reg.read());
    } else {
        mult_in1.write(float_v_reg.read());
        mult_in2.write(k_reg.read());
        mult_in_valid.write(kb_valid_reg.read());
    }
}

void Exponent::GetUV() {
    u_reg.write(0);
    v_reg.write(0);
    uv_valid_reg.write(false);

    while (true) {
        wait();
        if (rst_n.read() == false) {
            u_reg.write(0);
            v_reg.write(0);
            uv_valid_reg.write(false);
            continue;
        }

        int      exp_num = ((mult_out.read() >> 10) & 0x1f) - 15;
        uint16_t frac_num = (mult_out.read() & 0x3ff) | 0x400;
        uint16_t sign_num = ((mult_out.read() >> 15) & 0x1);
        if (exp_num >= 0) {
            frac_num <<= exp_num;
        } else {
            frac_num >>= (-exp_num);
        }
        int      u1 = (frac_num >> 10);
        uint16_t v1 = (frac_num & 0x3ff);
        if (sign_num == 0x1) {
            u1 = -u1 - 1;
            v1 = 0x400 - v1;
        }

        if (!mult_status_reg.read() && mult_out_valid.read()) {
            u_reg.write(u1);
            v_reg.write(v1);
        }
        uv_valid_reg.write(!mult_status_reg.read() && mult_out_valid.read());
    }
}

void Exponent::GetFloatV() {
    static int index = 0;
    float_v_reg.write(0);
    float_v_valid_reg.write(false);

    while (true) {
        wait();
        if (rst_n.read() == false) {
            float_v_reg.write(0);
            float_v_valid_reg.write(false);
            continue;
        }

        uint16_t flip_v = FlipBinary<uint16_t, 10>(v_reg.read());

        uint16_t float_v;
        if (flip_v == 0) {
            float_v = 0;
        } else {
            int shift_n = OHToUInt(MaskFirst(flip_v)) + 1;
            uint16_t v_f = ((v_reg.read() << shift_n) & 0x3ff);
            uint16_t v_e = (uint16_t)(15 - shift_n);
            float_v = half(v_f, v_e, 0x0).GetBits();
        }

        if (uv_valid_reg.read()) {
            float_v_reg.write(float_v);
        }
        float_v_valid_reg.write(uv_valid_reg.read());
    }
}

void Exponent::GetKB() {
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

        uint16_t k_list[exp_linear_scope_num];
        uint16_t b_list[exp_linear_scope_num];
        for (int i = 0; i < exp_linear_scope_num; i++) {
            if (params_[i]["x1"] <= float_v_reg.read() && float_v_reg.read() < params_[i]["x2"]) {
                k_list[i] = params_[i]["k"];
                b_list[i] = params_[i]["b"];
            } else {
                k_list[i] = 0;
                b_list[i] = 0;
            }
        }

        uint16_t k_result = 0;
        uint16_t b_result = 0;
        for (int i = 0; i < exp_linear_scope_num; i++) {
            k_result |= k_list[i];
            b_result |= b_list[i];
        }

        if (float_v_valid_reg.read()) {
            k_reg.write(k_result);
            b_reg.write(b_result);
        }
        kb_valid_reg.write(float_v_valid_reg.read());
    }
}

void Exponent::AssignAdderValid() {
    add_in_valid.write(mult_status_reg.read() && mult_out_valid.read());
}

void Exponent::SetOutReg() {
    out_data_reg.write(0);
    out_valid_reg.write(false);

    while (true) {
        wait();
        if (rst_n.read() == false) {
            out_data_reg.write(0);
            out_valid_reg.write(false);
            continue;
        }

        uint16_t frac = (add_out.read() & 0x3ff);
        uint16_t exp = ((add_out.read() >> 10) & 0x1f);
        uint16_t sign = ((add_out.read() >> 15) & 0x1);
        exp = (uint16_t)((int)exp + u_reg.read());
        uint16_t out_data_result = half(frac, exp, sign).GetBits();

        if (add_out_valid) {
            out_data_reg.write(out_data_result);
            out_valid_reg.write(true);
        } else {
            out_data_reg.write(0);
            out_valid_reg.write(false);
        }
    }
}

void Exponent::AssignOut() {
    if (output_zero_reg.read()) {
        out_data.write(0);
        out_valid.write(true);
    } else {
        out_data.write(out_data_reg.read());
        out_valid.write(out_valid_reg.read());
    }
}