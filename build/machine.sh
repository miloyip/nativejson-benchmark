#!/usr/bin/env bash
if [[ "$OSTYPE" == "darwin"* ]]; then
	sysctl -n machdep.cpu.brand_string   | sed 's/model name.*: //' | sed -E 's/[ ]*//g' | sed -e 's/Intel(R)//' -e 's/(TM)//'  -e 's/(R)//' -e 's/CPU//' | sed 's/.*/#define MACHINE "&"/' > ../src/machine.h
else
	grep -m 1 "model name" /proc/cpuinfo | sed 's/model name.*: //' | sed -r 's/\s+//g' | sed -e 's/Intel(R)//' -e 's/(TM)//' -e 's/(R)//' -e 's/CPU//' | sed 's/.*/#define MACHINE "&"/' > ../src/machine.h
fi
cat ../src/machine.h
