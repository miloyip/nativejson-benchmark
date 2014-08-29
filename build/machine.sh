#!/bin/sh
grep -m 1 "model name" /proc/cpuinfo | sed 's/model name.*: //' | sed -r 's/\s+//g' | sed -e 's/Intel(R)//' -e 's/(TM)//' -e 's/CPU//' | sed 's/.*/#define MACHINE "&"/' > ../src/machine.h
cat ../src/machine.h
