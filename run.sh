#!/bin/bash

name="wrap.c"
foo="chpl_comm_get"

read -p "Do you wanna change defaults? ($name $foo) (y to change)" reply

if [ "$reply" == 'y' ]
then
  read -p "Input wrapper file name: " name
  read -p "Input name of the function you wanna wrap:" foo
fi

mkdir -p results
rm -f results/$foo h.o* prog prog_real

chpl prog.chpl -o prog $name --ldflags "-Xlinker --wrap="$foo

qsub h.job
