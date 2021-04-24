#!/bin/sh

for file in $(ls)
do
	if [ -f $file ]; then
		if [ $file == search.sh -o $file == server  ]; then
		    :
		else
		    if [ $(cat $file | grep -c $1) -eq 0 ]; then
			    :
		    else
			    echo $file
		    fi
		fi
	fi
done
