#pragma once

#include <systemc.h>

#include "config.h"
#include "umHalf.h"
#include "util.h"

half ParseHalfFloat(uint16_t v);

uint16_t HalfFloatAdd(uint16_t h1, uint16_t h2);

uint16_t HalfFloatSub(uint16_t h1, uint16_t h2);

uint16_t HalfFloatMul(uint16_t h1, uint16_t h2);

uint16_t HalfFloatDiv(uint16_t h1, uint16_t h2);

int HalfFloatCompare(uint16_t h1, uint16_t h2);


SC_MODULE(hf_multiplier) {
    sc_in<bool>      clk, rst_n;
    sc_in<uint16_t>  in1;
    sc_in<uint16_t>  in2;
    sc_in<bool>      in_valid;
    sc_out<uint16_t> out;
    sc_out<bool>     out_valid;

    sc_signal<uint16_t> cal_out;

    delay_buffer<multiplier_delay, uint16_t> out_buffer{"mult out delay buffer"};
    delay_buffer<multiplier_delay, bool>     valid_buffer{"mult valid delay buffer"};

    void Calculate() {
        cal_out.write(HalfFloatMul(in1.read(), in2.read()));
    }

    SC_CTOR(hf_multiplier) {
        out_buffer.clk(clk);
        out_buffer.rst_n(rst_n);
        out_buffer.in(cal_out);
        out_buffer.out(out);

        valid_buffer.clk(clk);
        valid_buffer.rst_n(rst_n);
        valid_buffer.in(in_valid);
        valid_buffer.out(out_valid);

        SC_METHOD(Calculate);
        sensitive << in1 << in2;
    }
};

SC_MODULE(hf_adder) {
    sc_in<bool>      clk, rst_n;
    sc_in<uint16_t>  in1;
    sc_in<uint16_t>  in2;
    sc_in<bool>      in_valid;
    sc_out<uint16_t> out;
    sc_out<bool>     out_valid;

    sc_signal<uint16_t> cal_out;

    delay_buffer<add_delay, uint16_t> out_buffer{"add out delay buffer"};
    delay_buffer<add_delay, bool>     valid_buffer{"add valid delay buffer"};

    void Calculate() {
        cal_out.write(HalfFloatAdd(in1.read(), in2.read()));
    }

    SC_CTOR(hf_adder) {
        out_buffer.clk(clk);
        out_buffer.rst_n(rst_n);
        out_buffer.in(cal_out);
        out_buffer.out(out);

        valid_buffer.clk(clk);
        valid_buffer.rst_n(rst_n);
        valid_buffer.in(in_valid);
        valid_buffer.out(out_valid);

        SC_METHOD(Calculate);
        sensitive << in1 << in2;
    }
};

SC_MODULE(hf_subtractor) {
    sc_in<bool>      clk, rst_n;
    sc_in<uint16_t>  in1;
    sc_in<uint16_t>  in2;
    sc_in<bool>      in_valid;
    sc_out<uint16_t> out;
    sc_out<bool>     out_valid;

    sc_signal<uint16_t> cal_out;

    delay_buffer<sub_delay, uint16_t> out_buffer{"sub out delay buffer"};
    delay_buffer<sub_delay, bool>     valid_buffer{"sub valid delay buffer"};

    void Calculate() {
        cal_out.write(HalfFloatSub(in1.read(), in2.read()));
    }

    SC_CTOR(hf_subtractor) {
        out_buffer.clk(clk);
        out_buffer.rst_n(rst_n);
        out_buffer.in(cal_out);
        out_buffer.out(out);

        valid_buffer.clk(clk);
        valid_buffer.rst_n(rst_n);
        valid_buffer.in(in_valid);
        valid_buffer.out(out_valid);

        SC_METHOD(Calculate);
        sensitive << in1 << in2;
    }
};

template <int N, int M>
SC_MODULE(hf_max_tree) {
    sc_in<bool>      clk, rst_n;
    sc_in<uint16_t>  in[N];
    sc_in<bool>      in_valid;
    sc_out<uint16_t> out;
    sc_out<bool>     out_valid;

    sc_signal<uint16_t> cal_out;

    delay_buffer<M, uint16_t> out_buffer{"max out delay buffer"};
    delay_buffer<M, bool>     valid_buffer{"max valid delay buffer"};

    void Calculate() {
        uint16_t result = in[0].read();
        for (int i = 0; i < N; i++) {
            if (HalfFloatCompare(in[i].read(), result) == 1) {
                result = in[i].read();
            }
        }
        cal_out.write(result);
    }

    SC_CTOR(hf_max_tree) {
        out_buffer.clk(clk);
        out_buffer.rst_n(rst_n);
        out_buffer.in(cal_out);
        out_buffer.out(out);

        valid_buffer.clk(clk);
        valid_buffer.rst_n(rst_n);
        valid_buffer.in(in_valid);
        valid_buffer.out(out_valid);

        SC_METHOD(Calculate);
        for (int i = 0; i < N; i++) {
            sensitive << in[i];
        }
    }
};

template <int N, int M>
SC_MODULE(hf_add_tree) {
    sc_in<bool>      clk, rst_n;
    sc_in<uint16_t>  in[N];
    sc_in<bool>      in_valid;
    sc_out<uint16_t> out;
    sc_out<bool>     out_valid;

    sc_signal<uint16_t> cal_out;

    delay_buffer<M, uint16_t> out_buffer{"add out delay buffer"};
    delay_buffer<M, bool>     valid_buffer{"add out delay buffer"};

    void Calculate() {
        uint16_t result = in[0].read();
        for (int i = 1; i < N; i++) {
            result = HalfFloatAdd(result, in[i].read());
        }
        cal_out.write(result);
    }

    SC_CTOR(hf_add_tree) {
        out_buffer.clk(clk);
        out_buffer.rst_n(rst_n);
        out_buffer.in(cal_out);
        out_buffer.out(out);

        valid_buffer.clk(clk);
        valid_buffer.rst_n(rst_n);
        valid_buffer.in(in_valid);
        valid_buffer.out(out_valid);

        SC_METHOD(Calculate);
        for (int i = 0; i < N; i++) {
            sensitive << in[i];
        }
    }
};
