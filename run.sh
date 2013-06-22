#!/bin/bash
prog_name="prog.chpl"
name="wrap.c"
foo1="chpl_comm_get"
foo2="chpl_comm_put"
foo3="main"
foo4="chpl_exit_all"
foo5="chpl_comm_init"
options="-Xlinker --wrap=$foo1 -Xlinker --wrap=$foo2 \
         -Xlinker --wrap=$foo3 -Xlinker --wrap=$foo4 \
         -Xlinker --wrap=$foo5"

mkdir -p results
rm -f results/chpl_comm* h.o* prog prog_real results/info

chpl $prog_name -o prog $name --ldflags "$options"

echo $prog_name >> results/info
