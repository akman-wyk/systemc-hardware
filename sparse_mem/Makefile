INCLUDE=-I./ -I../
OUT=../compile_out
TARGET=$(OUT)/libsparse_mem.a

.PHONY: all

all: $(TARGET)

$(TARGET): $(OUT)/compressor.o
	ar crv $(TARGET) $^

$(OUT)/compressor.o: compressor/compressor.cpp
	g++ $(LIBDIR) $(INCDIR) $(INCLUDE) $(LIBS) -c $^ -o $(OUT)/compressor.o

# %.o: %.cpp
# 	g++ $(LIBDIR) $(INCDIR) $(INCLUDE) $(LIBS) -c $< -o $(OUT)/$@

include ../include.mk