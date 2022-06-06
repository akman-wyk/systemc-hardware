#include <systemc.h>

#include <cmath>
#include <fstream>
#include <string>

#include "compressor.h"
#include "compressor_driver.h"
#include "compressor_monitor.h"
#include "config.h"

int sc_main(int argc, char* argv[]) {
    std::string data_file = std::string(argv[1]);
    std::string out_file = std::string(argv[2]);
    int run_time = std::stoi(argv[3]);

    sc_clock clk("clk", 10, SC_NS);

    sc_signal<bool>        rst_n;
    sc_signal<bool>        compress_en;
    sc_signal<sc_uint<2> > offset_width_cfg;

    sc_signal<bool>                  write_valid;
    sc_signal<bool>                  write_ready;
    sc_signal<bool>                  write_last;
    sc_signal<sc_uint<group_width> > write_group;
    sc_signal<bool>                  write_begin;
    sc_signal<sc_uint<data_width> >  write_data[max_data_num];
    sc_signal<sc_uint<size_width> >  write_size;

    sc_signal<bool>                       read_valid;
    sc_signal<bool>                       read_ready;
    sc_signal<bool>                       read_last;  // the last of all data
    sc_signal<sc_uint<group_width> >      read_group;
    sc_signal<bool>                       read_begin;
    sc_signal<bool>                       read_data_last;  // the last of a batch of write data
    sc_signal<sc_uint<data_width> >       read_data_value;
    sc_signal<sc_uint<max_offset_width> > read_data_offset;

    sc_trace_file* fp;
    fp = sc_create_vcd_trace_file("compressor_wave");
    fp->set_time_unit(1, SC_NS);

    compressor comp("comp");
    comp.clk(clk);
    comp.rst_n(rst_n);
    comp.compress_en(compress_en);
    comp.offset_width_cfg(offset_width_cfg);
    comp.write_valid(write_valid);
    comp.write_ready(write_ready);
    comp.write_last(write_last);
    comp.write_group(write_group);
    comp.write_begin(write_begin);
    comp.write_size(write_size);
    comp.read_valid(read_valid);
    comp.read_ready(read_ready);
    comp.read_last(read_last);
    comp.read_group(read_group);
    comp.read_begin(read_begin);
    comp.read_data_last(read_data_last);
    comp.read_data_value(read_data_value);
    comp.read_data_offset(read_data_offset);
    for (int i = 0; i < max_data_num; i++) {
        comp.write_data[i](write_data[i]);
    }

    compressor_driver dri("driver");
    dri.set_data_file(data_file);
    dri.write_ready(write_ready);
    dri.rst_n(rst_n);
    dri.write_valid(write_valid);
    dri.write_last(write_last);
    dri.write_group(write_group);
    dri.write_begin(write_begin);
    dri.write_size(write_size);
    dri.read_ready(read_ready);
    dri.compress_en(compress_en);
    dri.offset_width_cfg(offset_width_cfg);
    for (int i = 0; i < max_data_num; i++) {
        dri.write_data[i](write_data[i]);
    }

    compressor_monitor monitor("monitor");
    monitor.set_out_file(out_file);
    monitor.clk(clk);
    monitor.read_data_value(read_data_value);
    monitor.read_data_offset(read_data_offset);
    monitor.read_valid(read_valid);
    monitor.read_ready(read_ready);

    // in out signal
    sc_trace(fp, clk, "clk");
    sc_trace(fp, rst_n, "rst_n");
    sc_trace(fp, compress_en, "compress_en");
    sc_trace(fp, offset_width_cfg, "offset_width_cfg");
    sc_trace(fp, write_valid, "write_valid");
    sc_trace(fp, write_ready, "write_ready");
    sc_trace(fp, write_last, "write_last");
    sc_trace(fp, write_group, "write_group");
    sc_trace(fp, write_begin, "write_begin");
    sc_trace(fp, write_size, "write_size");
    sc_trace(fp, read_valid, "read_valid");
    sc_trace(fp, read_ready, "read_ready");
    sc_trace(fp, read_last, "read_last");
    sc_trace(fp, read_group, "read_group");
    sc_trace(fp, read_begin, "read_begin");
    sc_trace(fp, read_data_last, "read_data_last");
    sc_trace(fp, read_data_value, "read_data_value");
    sc_trace(fp, read_data_offset, "read_data_offset");
    for (int i = 0; i < max_data_num; i++) {
        std::string name = "write_data[" + std::to_string(i) + "]";
        sc_trace(fp, write_data[i], name);
    }

    // module internal signal
    // sc_trace(fp, comp.mask_reg, "mask_reg");
    // sc_trace(fp, comp.read_ptr_reg, "read_ptr_reg");
    // sc_trace(fp, comp.read_ptr, "read_ptr");
    // sc_trace(fp, comp.read_ptr_offset, "read_ptr_offset");
    // for (int i = 0; i < max_data_num; i++) {
    //     std::string name = "write_data_reg[" + std::to_string(i) + "]";
    //     sc_trace(fp, comp.write_data_reg[i], name);
    // }

    sc_start(run_time, SC_NS);

    sc_close_vcd_trace_file(fp);  // close(fp)
    return 0;
}