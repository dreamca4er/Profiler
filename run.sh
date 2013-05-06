#!/bin/bash

name="wrap.c"
foo1="chpl_comm_get"
foo2="chpl_comm_put"
foo3="chpl_comm_init"
options="-Xlinker --wrap=$foo1 -Xlinker --wrap=$foo2 -Xlinker --wrap=$foo3"

mkdir -p results
rm -f results/chpl_comm* h.o* prog prog_real

chpl prog.chpl -o prog $name --ldflags "$options"

qsub h.job
