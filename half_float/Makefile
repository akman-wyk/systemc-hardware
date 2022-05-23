INCLUDE=-I./ -I../
OUT=../compile_out
TARGET=$(OUT)/libhalf_float.a

.PHONY: all

all: $(TARGET)

$(TARGET): half_float.cpp half_float.h
	g++ $(LIBDIR) $(INCDIR) $(INCLUDE) $(LIBS) -c half_float.cpp -o $(OUT)/half_float.o
	ar crv $(TARGET) $(OUT)/half_float.o

include ../include.mk