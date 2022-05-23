#!/bin/bash
module=("softmax" "compressor")
if [ $# -ne 1 ];then
    echo "Please input the name(only one) of the module to be tested"
else
    if echo "${module[@]}" | grep -w $1 &>/dev/null; then
        echo "make"
        make
        echo "./$1_tb &> $1_tb.log"
        ./"$1_tb" &> "$1_tb.log"
        echo "gtkwave $1_wave.vcd"
        gtkwave "$1_wave.vcd"
    else
        echo "Please input the name of the module in list: ${module[@]}"
    fi
fi