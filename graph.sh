#!/bin/bash

rm -f results/m results/graph.gv
cat results/chpl_comm* > results/m
gcc -o graph graph.c

./graph -l 0 1 2 -w 1350

dot -Tsvg results/graph.gv -o results/graph.html
