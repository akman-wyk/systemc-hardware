#pragma once

#include <systemc.h>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "half_float/half_float.h"

SC_MODULE(Logarithm) {
    // static var and method
    static const uint16_t ln2;

    static std::vector<std::unordered_map<std::string, uint16_t>> GetLinearLog2Params(int n);

    // linear params
    std::vector<std::unordered_map<std::string, uint16_t>> params_;

    // ports and signals
    sc_in<bool>      clk, rst_n;
    sc_in<uint16_t>  in_data;
    sc_in<bool>      in_valid;
    sc_out<uint16_t> out_data;
    sc_out<bool>     out_valid;

    sc_signal<uint16_t> m;
    sc_signal<uint16_t> e_reg;
    sc_signal<uint16_t> m_reg;

    sc_signal<uint16_t> k_reg;
    sc_signal<uint16_t> b_reg;
    sc_signal<bool>     kb_valid_reg;

    hf_multiplier       mult{"log half float multiplier"};
    sc_signal<uint16_t> mult_in1;
    sc_signal<uint16_t> mult_in2;
    sc_signal<bool>     mult_in_valid;
    sc_signal<uint16_t> mult_out;
    sc_signal<bool>     mult_out_valid;
    sc_signal<bool> mult_status_reg;  // false: first mult with k, true: second mult with constant

    hf_adder            adder_1{"log half float adder 1"};
    sc_signal<bool>     add_in_valid_1;
    sc_signal<uint16_t> add_out_1;
    sc_signal<bool>     add_out_valid_1;

    hf_adder            adder_2{"log half float adder 2"};
    sc_signal<uint16_t> add_out_2;
    sc_signal<bool>     add_out_valid_2;

    void AssignM();

    void GetMEReg();

    void GetKB();

    void SetMultStatus();

    void AssignMultSignals();

    void AssignAdder1Valid();

    void AssignOut();

    SC_CTOR(Logarithm) {
        params_ = GetLinearLog2Params(log_linear_scope_num);

        mult.clk(clk);
        mult.rst_n(rst_n);
        mult.in1(mult_in1);
        mult.in2(mult_in2);
        mult.in_valid(mult_in_valid);
        mult.out(mult_out);
        mult.out_valid(mult_out_valid);

        adder_1.clk(clk);
        adder_1.rst_n(rst_n);
        adder_1.in1(mult_out);
        adder_1.in2(b_reg);
        adder_1.in_valid(add_in_valid_1);
        adder_1.out(add_out_1);
        adder_1.out_valid(add_out_valid_1);

        adder_2.clk(clk);
        adder_2.rst_n(rst_n);
        adder_2.in1(add_out_1);
        adder_2.in2(e_reg);
        adder_2.in_valid(add_out_valid_1);
        adder_2.out(add_out_2);
        adder_2.out_valid(add_out_valid_2);

        SC_METHOD(AssignM);
        sensitive << in_data;

        SC_THREAD(GetMEReg);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(GetKB);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(SetMultStatus);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_METHOD(AssignMultSignals);
        sensitive << mult_status_reg << m_reg << k_reg << kb_valid_reg << add_out_2
                  << add_out_valid_2;

        SC_METHOD(AssignAdder1Valid);
        sensitive << mult_status_reg << mult_out_valid;

        SC_METHOD(AssignOut);
        sensitive << mult_status_reg << mult_out_valid << mult_out;
    }
};