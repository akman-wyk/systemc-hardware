#pragma once

#include <systemc.h>

template <class V>
V MaskFirst(V input) {
    V   v = input;
    int i = 0;
    while (v != 0) {
        if ((v & 0x1) != 0) {
            return (input & (0x1 << i));
        }
        v >>= 1;
        i++;
    }
    return 0;
}

template <class V>
unsigned int OHToUInt(V input) {
    unsigned int result = 0;
    input >>= 1;
    while (input != 0) {
        result++;
        input >>= 1;
    }
    return result;
}

template <class V, int N>
V FlipBinary(V input) {
    V v = 0;
    for (int i = 0; i < N; i++) {
        V tmp = (input & 0x1);
        input >>= 1;
        v = ((v << 1) | tmp);
    }
    return v;
}

template <int N = 1, class V = uint16_t>
SC_MODULE(delay_buffer) {
    sc_in<bool> clk, rst_n;
    sc_in<V>    in;
    sc_out<V>   out;

    sc_signal<V> buffer[N];

    void Exec() {
        for (int i = 0; i < N; i++) {
            buffer[i].write(0);
        }

        while (true) {
            wait();
            if (rst_n.read() == false) {
                for (int i = 0; i < N; i++) {
                    buffer[i].write(0);
                }
                continue;
            }

            buffer[0].write(in.read());
            for (int i = 1; i < N; i++) {
                buffer[i].write(buffer[i - 1].read());
            }
        }
    }

    void Output() {
        out.write(buffer[N - 1].read());
    }

    SC_CTOR(delay_buffer) {
        SC_THREAD(Exec);
        sensitive_pos << clk;
        sensitive_neg << rst_n;
        SC_METHOD(Output);
        sensitive << buffer[N - 1];
    }
};