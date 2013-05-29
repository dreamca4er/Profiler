#!/bin/bash

rm -f results/m results/graph.gv
cat results/chpl_comm* > results/m
gcc -o graph graph.c

./graph $*

dot -Tsvg results/graph.gv -o results/graph.html
