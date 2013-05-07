#!/bin/bash

rm -f results/m
cat results/chpl_comm_get results/chpl_comm_put > results/m

gcc -o graph graph.c

./graph results/m

