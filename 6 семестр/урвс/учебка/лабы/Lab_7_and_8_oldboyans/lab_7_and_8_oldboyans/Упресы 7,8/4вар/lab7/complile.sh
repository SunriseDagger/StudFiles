#!/bin/sh

SRC="sources"
FILE1="main.cpp"
FILE2="table.cpp"
FILE3="table.h"
CXX="g++"
FLAGS="-Wall -O2"
OUT="lab7"

$CXX "$SRC/$FILE1" "$SRC/$FILE2" "$SRC/$FILE3" $FLAGS -o $OUT
