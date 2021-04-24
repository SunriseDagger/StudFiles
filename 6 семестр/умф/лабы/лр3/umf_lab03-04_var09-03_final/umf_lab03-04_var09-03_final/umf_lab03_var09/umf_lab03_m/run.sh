#!/bin/bash
rm -r testdir 2>/dev/null >/dev/null
mkdir -p testdir
cd testdir
echo "compiling.."
g++ -O3 -I../ ../additional_classes.cpp ../grid_gen_rect.cpp ../solver_BCGStab.cpp ../grid_gen.cpp ../solver_LOS.cpp ../grid_gen_cube.cpp ../launch.cpp ../solver_LU.cpp -o umf_lab03_m
./umf_lab03_m

