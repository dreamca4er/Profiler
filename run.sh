#!/bin/bash

echo "Input wrapper file name:"

read name

echo "Input name of the function you wanna wrap:"

read foo

mkdir -p results
rm -f results/$foo h.o* prog prog_real

chpl prog.chpl -o prog $name --ldflags "-Xlinker --wrap="$foo

qsub h.job
