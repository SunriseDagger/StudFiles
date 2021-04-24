#!/bin/sh
#clearing test files

# Procedure run script whose name is passed as a parameter
deletefile()
{	
	if [ -f $1 ]
	then	
		rm $1
	fi
}

deletefile "params.txt"
deletefile "result.txt"
