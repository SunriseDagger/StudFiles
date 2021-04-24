#!/bin/sh

CXX="g++"
FLAGS="-O2 -Wall"
SRC="sources"
SRV="server"
N=5

for i in `seq 1 $N`
do
    if [ -e "$SRV$i" ]; then
	:
    else
	mkdir "$SRV$i"
    fi
done

$CXX "$SRC/client.cpp" $FALGS -o client
$CXX "$SRC/server.cpp" $FLAGS -o server
$CXX "$SRC/main_server.cpp" $FALGS -o main_server

for i in `seq 1 $N`
do
    if [ -e "$SRV$i/server" ]; then
	rm "$SRV$i/server"
    fi
    cp server "$SRV$i/server"
done

rm server
