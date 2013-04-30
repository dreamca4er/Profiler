#!/bin/bash

name="wrap.c"
foo1="chpl_comm_get"
foo2="chpl_comm_put"
options="-Xlinker --wrap=$foo1 -Xlinker --wrap=$foo2"

read -p "Do you wanna change defaults? ($name) (y to change)" reply

if [ "$reply" == 'y' ]
then
  read -p "Input wrapper file name: " name
fi

mkdir -p results
rm -f results/chpl_comm* h.o* prog prog_real

chpl prog.chpl -o prog $name --ldflags "$options"

qsub h.job
