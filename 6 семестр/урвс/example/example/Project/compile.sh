#!/bin/sh

# Show all compiler errors
CFLAGS=-Wall
# Include math library (used by function sin(x))
LIBS=-lm
SRCDIR="sources"

cd ${SRCDIR}
g++ $CFLAGS $LIBS calculate.cpp utils.cpp -o ../calculate
