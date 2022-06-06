OUT=./compile_out

modules=sparse_mem half_float softmax
compressor_tb_file=sparse_mem/compressor/compressor_driver.cpp sparse_mem/compressor/compressor_tb.cpp
softmax_tb_file=softmax/softmax_driver.cpp softmax/softmax_tb.cpp
dense_mem_tb_file=dense_mem/dense_mem.cpp dense_mem/dense_mem_tb.cpp dense_mem/dense_mem_driver.cpp

INCLUDE=-I./

.PHONY: all clean $(modules)

all: $(modules) compressor_tb softmax_tb dense_mem_tb

compressor_tb: $(OUT)/libsparse_mem.a $(compressor_tb_file)
	g++ $(compressor_tb_file) $(OUT)/libsparse_mem.a $(LIBDIR) $(INCDIR) $(INCLUDE) $(LIBS) -o compressor_tb

softmax_tb: $(OUT)/libsoftmax.a $(OUT)/libhalf_float.a $(softmax_tb_file)
	g++ $(softmax_tb_file) $(OUT)/libsoftmax.a $(OUT)/libhalf_float.a $(LIBDIR) $(INCDIR) $(INCLUDE) $(LIBS) -o softmax_tb

dense_mem_tb: $(dense_mem_tb_file) $(OUT)/libhalf_float.a
	g++ $(dense_mem_tb_file) $(OUT)/libhalf_float.a $(LIBDIR) $(INCDIR) $(INCLUDE) $(LIBS) -o test/dense_mem/dense_mem_tb

$(modules):
	$(MAKE) --directory=$@

clean:
	rm -rf $(OUT)/*.o $(OUT)/*.a compressor_tb softmax_tb


include ./include.mk