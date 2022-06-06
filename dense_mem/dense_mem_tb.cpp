#include <systemc.h>

#include <fstream>
#include <string>

#include "config.h"
#include "dense_mem.h"
#include "dense_mem_driver.h"
#include "dense_mem_monitor.h"

int sc_main(int argc, char* argv[]) {
    std::string data_file = std::string(argv[1]);
    std::string out_file = std::string(argv[2]);
    int         run_time = std::stoi(argv[3]);
    int         config = std::stoi(argv[4]);

    sc_clock clk("clk", 10, SC_NS);

    sc_signal<bool>                 rst_n;
    sc_signal<bool>                 write_config;  // 0: 4 * 16, 1: 8 * 16
    sc_signal<sc_uint<data_width> > write_data[dense_mem_width];
    sc_signal<bool>                 write_valid;
    sc_signal<bool>                 write_ready;
    sc_signal<sc_uint<data_width> > read_data;
    sc_signal<bool>                 read_valid;
    sc_signal<bool>                 read_ready;

    sc_trace_file* fp;
    fp = sc_create_vcd_trace_file("dense_mem_wave");
    fp->set_time_unit(1, SC_NS);

    DenseMem dense_mem("dense_mem");
    dense_mem.clk(clk);
    dense_mem.rst_n(rst_n);
    dense_mem.write_config(write_config);
    dense_mem.write_valid(write_valid);
    dense_mem.write_ready(write_ready);
    dense_mem.read_data(read_data);
    dense_mem.read_valid(read_valid);
    dense_mem.read_ready(read_ready);
    for (int i = 0; i < dense_mem_width; i++) {
        dense_mem.write_data[i](write_data[i]);
    }

    DenseMemDriver driver("driver");
    driver.set_data_file(data_file, config);
    driver.write_ready(write_ready);
    driver.rst_n(rst_n);
    driver.write_config(write_config);
    driver.write_valid(write_valid);
    driver.read_ready(read_ready);
    for (int i = 0; i < dense_mem_width; i++) {
        driver.write_data[i](write_data[i]);
    }

    DenseMemMonitor monitor("monitor");
    monitor.set_out_file(out_file);
    monitor.clk(clk);
    monitor.read_data(read_data);
    monitor.read_valid(read_valid);
    monitor.read_ready(read_ready);

    sc_trace(fp, clk, "clk");
    sc_trace(fp, rst_n, "rst_n");
    sc_trace(fp, write_config, "write_config");
    sc_trace(fp, write_valid, "write_valid");
    sc_trace(fp, write_ready, "write_ready");
    sc_trace(fp, read_data, "read_data");
    sc_trace(fp, read_valid, "read_valid");
    sc_trace(fp, read_ready, "read_ready");
    for (int i = 0; i < dense_mem_width; i++) {
        std::string name = "write_data[" + std::to_string(i) + "]";
        sc_trace(fp, write_data[i], name);
    }
    sc_trace(fp, dense_mem.read_last_signal, "read_last_signal");
    sc_trace(fp, dense_mem.read_row_last_signal, "read_row_last_signal");
    sc_trace(fp, dense_mem.read_col, "read_col");
    sc_trace(fp, dense_mem.read_row, "read_row");
    sc_trace(fp, dense_mem.status, "status");
    sc_trace(fp, dense_mem.write_counter, "write_counter");

    sc_start(run_time, SC_NS);

    sc_close_vcd_trace_file(fp);
    return 0;
}