#!/bin/sh
#clearing test files

filename="wrong_filename.txt"
if [ -f $filename ] 
then
	rm $filename
fi
