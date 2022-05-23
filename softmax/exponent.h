#pragma once

#include <systemc.h>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "config.h"
#include "half_float/half_float.h"
#include "util.h"

SC_MODULE(Exponent) {
    // static var and method
    static const uint16_t min_input;
    static const uint16_t max_input;
    static const uint16_t log2e;

    static std::vector<std::unordered_map<std::string, uint16_t>> GetLinearExp2Params(int n);

    // linear params
    std::vector<std::unordered_map<std::string, uint16_t>> params_;

    // ports and signals
    sc_in<bool>      clk, rst_n;
    sc_in<uint16_t>  in_data;
    sc_in<bool>      in_valid;
    sc_out<uint16_t> out_data;
    sc_out<bool>     out_valid;

    sc_signal<uint16_t> data_reg;
    sc_signal<bool>     valid_reg;
    sc_signal<bool>     output_zero_reg;

    sc_signal<int>      u_reg;
    sc_signal<uint16_t> v_reg;
    sc_signal<bool>     uv_valid_reg;

    sc_signal<uint16_t> float_v_reg;
    sc_signal<bool>     float_v_valid_reg;

    hf_multiplier       mult{"exp half float multiplier"};
    sc_signal<uint16_t> mult_in1;
    sc_signal<uint16_t> mult_in2;
    sc_signal<bool>     mult_in_valid;
    sc_signal<uint16_t> mult_out;
    sc_signal<bool>     mult_out_valid;
    sc_signal<bool> mult_status_reg;  // false: first mult with constant, true: second mult with k

    sc_signal<uint16_t> k_reg;
    sc_signal<uint16_t> b_reg;
    sc_signal<bool>     kb_valid_reg;

    hf_adder            adder{"exp half float adder"};
    sc_signal<bool>     add_in_valid;
    sc_signal<uint16_t> add_out;
    sc_signal<bool>     add_out_valid;

    sc_signal<uint16_t> out_data_reg;
    sc_signal<bool>     out_valid_reg;

    void SetDataValidReg();

    void SetMultStatus();

    void AssignMultSignals();

    void GetUV();

    void GetFloatV();

    void GetKB();

    void AssignAdderValid();

    void SetOutReg();

    void AssignOut();

    SC_CTOR(Exponent) {
        params_ = GetLinearExp2Params(exp_linear_scope_num);

        mult.clk(clk);
        mult.rst_n(rst_n);
        mult.in1(mult_in1);
        mult.in2(mult_in2);
        mult.in_valid(mult_in_valid);
        mult.out(mult_out);
        mult.out_valid(mult_out_valid);

        adder.clk(clk);
        adder.rst_n(rst_n);
        adder.in1(mult_out);
        adder.in2(b_reg);
        adder.in_valid(add_in_valid);
        adder.out(add_out);
        adder.out_valid(add_out_valid);

        SC_THREAD(SetDataValidReg);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(SetMultStatus);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_METHOD(AssignMultSignals);
        sensitive << mult_status_reg << data_reg << valid_reg << float_v_reg << k_reg
                  << kb_valid_reg;

        SC_THREAD(GetUV);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(GetFloatV);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(GetKB);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_METHOD(AssignAdderValid);
        sensitive << mult_status_reg << mult_out_valid;

        SC_THREAD(SetOutReg);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_METHOD(AssignOut);
        sensitive << in_valid << in_data << out_data_reg << out_valid_reg;
    }
};