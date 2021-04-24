#!/bin/sh
#clearing test files

filename="params.txt"
if [ -f $filename ] 
then
	rm $filename
fi
