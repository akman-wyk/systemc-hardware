#include "dense_mem.h"

void DenseMem::FSM() {
    status.write(0);
    while (true) {
        wait();
        if (rst_n.read() == false) {
            status.write(0);
            continue;
        }

        bool write_third_line_data =
            write_counter.read() == 0x3 && write_valid.read() && write_ready.read();
        bool read_last_data = read_last_signal.read() && read_valid_signal.read() && read_ready.read();
        switch (status.read()) {
            case 0: {
                if (write_third_line_data) {
                    status.write(write_config.read() ? 1 : 4);
                }
                break;
            }
            case 1: {
                if (write_third_line_data) {
                    status.write(2);
                }
                break;
            }
            case 2: {
                if (write_third_line_data) {
                    status.write(3);
                }
                break;
            }
            case 3: {
                if (write_third_line_data) {
                    status.write(5);
                }
                break;
            }
            case 4:
            case 5: {
                if (read_last_data) {
                    status.write(0);
                }
                break;
            }
            default: break;
        }
    }
}

void DenseMem::AccWriteCounter() {
    write_counter.write(0);
    while (true) {
        wait();
        if (rst_n.read() == false) {
            write_counter.write(0);
            continue;
        }

        if (write_valid.read() && write_ready.read()) {
            write_counter.write(write_counter.read() + 1);  // TODO: 测试这种写法是否正确
        }
    }
}

void DenseMem::WriteData() {
    for (int i = 0; i < dense_mem_height; i++) {
        for (int j = 0; j < dense_mem_width; j++) {
            mem[i][j].write(0);
        }
    }
    while (true) {
        wait();
        if (rst_n.read() == false) {
            for (int i = 0; i < dense_mem_height; i++) {
                for (int j = 0; j < dense_mem_width; j++) {
                    mem[i][j].write(0);
                }
            }
            continue;
        }

        if (write_valid.read() && write_ready.read()) {
            int write_line = (status.read() == 0 || status.read() == 2) ? write_counter.read() + 0
                                                                        : write_counter.read() + 4;
            if (status.read() == 0 || status.read() == 1) {
                for (int j = 0; j < dense_mem_width; j++) {
                    mem[write_line][j].write(write_data[j].read());
                }
            } else if (status.read() == 2 || status.read() == 3) {
                for (int j = 0; j < dense_mem_width; j++) {
                    uint16_t out = HalfFloatAdd(mem[write_line][j].read(), write_data[j].read());
                    mem[write_line][j].write(out);
                }
            }
        }
    }
}

void DenseMem::ReadData() {
    read_data.write(0);
    read_col.write(0);
    read_row.write(0);
    read_valid.write(false);

    while (true) {
        wait();
        if (rst_n.read() == false) {
            read_data.write(0);
            read_col.write(0);
            read_row.write(0);
            read_valid.write(false);
            continue;
        }

        if (read_valid_signal.read() && read_ready.read()) {
            read_data.write(mem[read_row.read()][read_col.read()]);
            read_valid.write(true);
            if (read_last_signal.read()) {
                read_col.write(0);
                read_row.write(0);
            } else if (read_row_last_signal.read()) {
                read_col.write(0);
                read_row.write(read_row.read() + 1);
            } else {
                read_col.write(read_col.read() + 1);
            }
        } else {
            read_data.write(0);
            read_valid.write(false);
        }
    }
}

void DenseMem::AssignReadSignals() {
    bool read_row_last = read_valid_signal.read() && read_ready.read() && read_col.read() == 15;
    read_row_last_signal.write(read_row_last);
    read_last_signal.write(read_row_last && ((status.read() == 4 && read_row.read() == 3) ||
                                             (status.read() == 5 && read_row.read() == 7)));
}

void DenseMem::AssignOutSignals() {
    write_ready.write(status.read() == 0 || status.read() == 1 || status.read() == 2 ||
                      status.read() == 3);
    read_valid_signal.write(status.read() == 4 || status.read() == 5);
}