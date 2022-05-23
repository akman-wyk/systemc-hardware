#pragma once

#include <systemc.h>

#include <cstdint>
#include <string>

#include "exponent.h"
#include "half_float/half_float.h"
#include "logarithm.h"

SC_MODULE(Softmax) {
    sc_in<bool>      clk, rst_n;
    sc_in<uint16_t>  in_data[softmax_input_num];
    sc_in<bool>      in_valid;
    sc_out<bool>     in_ready;
    sc_out<uint16_t> out_data[softmax_input_num];
    sc_out<bool>     out_valid;
    sc_in<bool>      out_ready;

    sc_signal<bool> status_reg;  // false: first exp, true: second exp

    sc_signal<uint16_t> in_data_reg[softmax_input_num];

    hf_max_tree<softmax_input_num, max_tree_delay> max_tree{"softmax max tree"};
    sc_signal<bool>                                max_tree_in_valid;
    sc_signal<uint16_t>                            max_tree_out;
    sc_signal<bool>                                max_tree_out_valid;

    hf_subtractor*      sub[softmax_input_num];
    sc_signal<uint16_t> sub_in2;
    sc_signal<bool>     sub_in_valid;
    sc_signal<uint16_t> sub_out[softmax_input_num];
    sc_signal<bool>     sub_out_valid[softmax_input_num];

    Exponent*           exp[softmax_input_num];
    sc_signal<uint16_t> exp_out_data[softmax_input_num];
    sc_signal<bool>     exp_out_valid[softmax_input_num];

    sc_signal<uint16_t> exp_out_data_reg[softmax_input_num];
    sc_signal<bool>     exp_out_valid_reg[softmax_input_num];
    sc_signal<bool>     exp_out_all_valid;

    hf_add_tree<softmax_input_num, add_tree_delay> add_tree{"softmax add tree"};
    sc_signal<bool>                                add_tree_in_valid;
    sc_signal<uint16_t>                            add_tree_out;
    sc_signal<bool>                                add_tree_out_valid;

    Logarithm           log{"softmax log"};
    sc_signal<uint16_t> log_out_data;
    sc_signal<bool>     log_out_valid;

    void SetStatus();

    void SetInDataReg();

    void AssignMaxTreeInValid();

    void AssignSubSignals();

    void SetExpOutRegs();

    void AssignExpOutAllValid();

    void AssignAddTreeInValid();

    void SetOut();

    void SetInReady();

    SC_CTOR(Softmax) {
        max_tree.clk(clk);
        max_tree.rst_n(rst_n);
        max_tree.in_valid(max_tree_in_valid);
        max_tree.out(max_tree_out);
        max_tree.out_valid(max_tree_out_valid);
        for (int i = 0; i < softmax_input_num; i++) {
            max_tree.in[i](in_data[i]);
        }

        for (int i = 0; i < softmax_input_num; i++) {
            std::string name = "softmax subtractor " + std::to_string(i);
            sub[i] = new hf_subtractor(name.c_str());
            sub[i]->clk(clk);
            sub[i]->rst_n(rst_n);
            sub[i]->in1(in_data_reg[i]);
            sub[i]->in2(sub_in2);
            sub[i]->in_valid(sub_in_valid);
            sub[i]->out(sub_out[i]);
            sub[i]->out_valid(sub_out_valid[i]);
        }

        for (int i = 0; i < softmax_input_num; i++) {
            std::string name = "softmax exponent " + std::to_string(i);
            exp[i] = new Exponent(name.c_str());
            exp[i]->clk(clk);
            exp[i]->rst_n(rst_n);
            exp[i]->in_data(sub_out[i]);
            exp[i]->in_valid(sub_out_valid[i]);
            exp[i]->out_data(exp_out_data[i]);
            exp[i]->out_valid(exp_out_valid[i]);
        }

        add_tree.clk(clk);
        add_tree.rst_n(rst_n);
        add_tree.in_valid(add_tree_in_valid);
        add_tree.out(add_tree_out);
        add_tree.out_valid(add_tree_out_valid);
        for (int i = 0; i < softmax_input_num; i++) {
            add_tree.in[i](exp_out_data_reg[i]);
        }

        log.clk(clk);
        log.rst_n(rst_n);
        log.in_data(add_tree_out);
        log.in_valid(add_tree_out_valid);
        log.out_data(log_out_data);
        log.out_valid(log_out_valid);

        SC_THREAD(SetStatus);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(SetInDataReg);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_METHOD(AssignMaxTreeInValid);
        sensitive << in_valid << in_ready;

        SC_METHOD(AssignSubSignals);
        sensitive << status_reg << max_tree_out << max_tree_out_valid << log_out_data
                  << log_out_valid;

        SC_THREAD(SetExpOutRegs);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_METHOD(AssignExpOutAllValid);
        for (int i = 0; i < softmax_input_num; i++) {
            sensitive << exp_out_valid_reg[i];
        }

        SC_METHOD(AssignAddTreeInValid);
        sensitive << exp_out_all_valid << status_reg;

        SC_THREAD(SetOut);
        sensitive_pos << clk;
        sensitive_neg << rst_n;

        SC_THREAD(SetInReady);
        sensitive_pos << clk;
        sensitive_neg << rst_n;
    }

    ~Softmax() {
        for (int i = 0; i < softmax_input_num; i++) {
            if (sub[i] != nullptr) {
                delete sub[i];
                sub[i] = nullptr;
            }

            if (exp[i] != nullptr) {
                delete exp[i];
                exp[i] = nullptr;
            }
        }
    }
};