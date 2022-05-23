INCLUDE=-I./ -I../
OUT=../compile_out
TARGET=$(OUT)/libsoftmax.a
OBJ=$(OUT)/exponent.o $(OUT)/logarithm.o $(OUT)/softmax.o

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJ)
	ar crv $(TARGET) $^

$(OUT)/exponent.o: exponent.cpp
	g++ $(LIBDIR) $(INCDIR) $(INCLUDE) $(LIBS) -c $^ -o $(OUT)/exponent.o

$(OUT)/logarithm.o: logarithm.cpp
	g++ $(LIBDIR) $(INCDIR) $(INCLUDE) $(LIBS) -c $^ -o $(OUT)/logarithm.o

$(OUT)/softmax.o: softmax.cpp
	g++ $(LIBDIR) $(INCDIR) $(INCLUDE) $(LIBS) -c $^ -o $(OUT)/softmax.o

include ../include.mk