#include "compressor.h"

void compressor::SetOffsetRegsByConfig() {
    offset_mask_reg.write(0);
    offset_reg.write(0);

    while (true) {
        wait();
        if (rst_n.read() == false) {
            offset_mask_reg.write(0);
            offset_reg.write(0);
            continue;
        }
        int max_offset = 1 << cfg_to_offset_width[offset_width_cfg.read()];
        offset_mask_reg.write((1 << max_offset) - 1);
        offset_reg.write(max_offset - 1);
    }
}

void compressor::SetReadSignals() {
    sc_uint<max_data_num> s_mask;      // shifted mask
    sc_uint<max_data_num> so_mask;     // shifted mask with offset mask
    sc_uint<max_data_num> oh_so_mask;  // onehot shifted mask with offset_mask

    s_mask = mask_reg.read();
    so_mask = s_mask & offset_mask_reg.read();
    oh_so_mask = MaskFirst(so_mask);

    if (so_mask != 0) {
        read_zero.write(false);
        read_ptr_offset.write(OHToUInt(oh_so_mask));
        read_ptr.write(OHToUInt(oh_so_mask) + read_ptr_reg.read());
    } else {
        read_zero.write(true);
        read_ptr_offset.write(offset_reg.read());
        read_ptr.write(offset_reg.read() + read_ptr_reg.read());
    }
    read_last_signal.write((s_mask & (~oh_so_mask)) == 0);
}

void compressor::WriteData() {
    write_valid_reg.write(false);
    mask_reg.write(0);
    first_data_reg.write(false);
    read_ptr_reg.write(0);

    while (true) {
        wait();
        if (rst_n.read() == false) {
            write_valid_reg.write(false);
            mask_reg.write(0);
            first_data_reg.write(false);
            read_ptr_reg.write(0);
            continue;
        }

        if (write_valid.read() && write_ready.read()) {
            write_valid_reg.write(write_valid.read());
            write_last_reg.write(write_last.read());
            write_group_reg.write(write_group.read());
            write_begin_reg.write(write_begin.read());
            for (int i = 0; i < max_data_num; i++) {
                write_data_reg[i].write(write_data[i].read());
            }

            if (compress_en.read()) {
                sc_uint<max_data_num> mask_with_size = (1 << write_size.read()) - 1;
                for (int i = 0; i < max_data_num; i++) {
                    if (write_data[i].read() == 0) {
                        sc_uint<max_data_num> x = 1;
                        x <<= i;
                        mask_with_size &= (~x);
                    }
                }
                mask_reg.write(mask_with_size);
            } else {
                mask_reg.write((1 << write_size.read()) - 1);
            }

            first_data_reg.write(true);
            read_ptr_reg.write(0);
        } else if (read_valid.read() && read_ready.read()) {
            if (read_last_signal.read()) {
                write_valid_reg.write(false);
            }
            read_ptr_reg.write(read_ptr.read() + 1);
            mask_reg.write(mask_reg.read() >> (read_ptr_offset.read() + 1));
            first_data_reg.write(false);
        }
    }
}

void compressor::ReadData() {
    write_ready.write((!write_valid_reg.read()) ||
                      (read_last_signal.read() && read_valid.read() && read_ready.read()));
    read_valid.write(write_valid_reg.read());
    read_last.write(write_last_reg.read() && read_last_signal.read());
    read_group.write(write_group_reg.read());
    read_begin.write(write_begin_reg.read() && first_data_reg.read());
    read_data_last.write(read_last_signal.read());
    read_data_offset.write(read_ptr_offset.read());
    if (read_zero.read()) {
        read_data_value.write(0);
    } else {
        read_data_value.write(write_data_reg[read_ptr.read()].read());
    }
}