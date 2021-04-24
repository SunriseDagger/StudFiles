#!/bin/sh
# checking contents of file "result.txt"
resfile="result.txt"
validationfile="result.correct.txt"

echo "sin(x) = 0.909297" > $validationfile

if [ -f $resfile ] 
then	
	if diff -q $resfile $validationfile > /dev/null
	then		
		status=0		
	else
		status=1		
	fi
else
	status=1	
fi

rm $validationfile
exit $status