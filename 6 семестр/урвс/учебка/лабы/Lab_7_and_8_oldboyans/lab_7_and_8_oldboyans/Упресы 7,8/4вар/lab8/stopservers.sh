#!/bin/sh

if [ -e pids.txt ]; then
    :
else
    exit 1
fi

for pid in $(cat pids.txt)
do
    kill -9 $pid
done

rm pids.txt
