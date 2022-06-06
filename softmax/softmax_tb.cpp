#include <systemc.h>

#include <string>

#include "config.h"
#include "softmax.h"
#include "softmax_driver.h"
#include "softmax_monitor.h"

int sc_main(int argc, char* argv[]) {
    std::string data_file = std::string(argv[1]);
    std::string out_file = std::string(argv[2]);
    int run_time = std::stoi(argv[3]);
    int data_set_num = std::stoi(argv[4]);

    sc_clock clk("clk", 10, SC_NS);

    sc_signal<bool>     rst_n;
    sc_signal<uint16_t> in_data[softmax_input_num];
    sc_signal<bool>     in_valid;
    sc_signal<bool>     in_ready;
    sc_signal<uint16_t> out_data[softmax_input_num];
    sc_signal<bool>     out_valid;
    sc_signal<bool>     out_ready;

    sc_trace_file* fp;
    fp = sc_create_vcd_trace_file("softmax_wave");
    fp->set_time_unit(1, SC_NS);

    Softmax softmax("softmax");
    softmax.clk(clk);
    softmax.rst_n(rst_n);
    softmax.in_valid(in_valid);
    softmax.in_ready(in_ready);
    softmax.out_valid(out_valid);
    softmax.out_ready(out_ready);
    for (int i = 0; i < softmax_input_num; i++) {
        softmax.in_data[i](in_data[i]);
        softmax.out_data[i](out_data[i]);
    }

    softmax_driver driver("softmax driver");
    driver.set_data_file(data_file, data_set_num);
    driver.in_ready(in_ready);
    driver.rst_n(rst_n);
    driver.in_valid(in_valid);
    driver.out_ready(out_ready);
    for (int i = 0; i < softmax_input_num; i++) {
        driver.in_data[i](in_data[i]);
    }

    softmax_monitor monitor("monitor");
    monitor.set_out_file(out_file);
    monitor.clk(clk);
    monitor.out_valid(out_valid);
    monitor.out_ready(out_ready);
    for (int i = 0; i < softmax_input_num; i++) {
        monitor.out_data[i](out_data[i]);
    }

    sc_trace(fp, clk, "clk");
    sc_trace(fp, rst_n, "rst_n");
    sc_trace(fp, in_valid, "in_valid");
    sc_trace(fp, in_ready, "in_ready");
    sc_trace(fp, out_valid, "out_valid");
    sc_trace(fp, out_ready, "out_ready");
    for (int i = 0; i < softmax_input_num; i++) {
        std::string name = "in_data[" + std::to_string(i) + "]";
        sc_trace(fp, in_data[i], name);
        name = "out_data[" + std::to_string(i) + "]";
        sc_trace(fp, out_data[i], name);
    }

    for (int i = 0; i < 10; i++) {
        std::string name = "exp[" + std::to_string(i) + "]_in_data";
        sc_trace(fp, softmax.exp[i]->in_data, name);
        name = "exp[" + std::to_string(i) + "]_in_valid";
        sc_trace(fp, softmax.exp[i]->in_valid, name);
        name = "exp[" + std::to_string(i) + "]_out_data";
        sc_trace(fp, softmax.exp[i]->out_data, name);
        name = "exp[" + std::to_string(i) + "]_out_valid";
        sc_trace(fp, softmax.exp[i]->out_valid, name);
    }

    sc_start(run_time, SC_NS);

    sc_close_vcd_trace_file(fp);  // close(fp)
    return 0;
}