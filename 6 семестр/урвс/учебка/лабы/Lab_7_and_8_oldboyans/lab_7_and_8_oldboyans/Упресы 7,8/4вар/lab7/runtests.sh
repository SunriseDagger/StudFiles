#!/bin/sh

PRG="lab7"
RUN="../$PRG"
TSTDIR="tests"

#***************************************************************
if [ -e $PRG -a -f $PRG  ]; then
    :
else
    echo "Error: $PRG file not found"
    exit 1
fi

if [ -e $TSTDIR -a -d $TSTDIR ]; then
    :
else
    echo "Error: $TSTDIR directory not found"
    exit 1
fi
#***************************************************************

cd $TSTDIR

testnum=0
testpassed=0

for file in input.*.txt 
do
    if [ -f $file ]; then
	curtest=$(echo $file | awk -F. '{print $2}')

	$RUN $file 1>"res.$curtest.txt" 2>"err.$curtest.txt"
	runres=$?

	if [ $runres -eq 0 ]; then
	    diff -q "answr.$curtest.txt" "res.$curtest.txt" > /dev/null	
	    diffresult=$?
	else
	    diff -q "answr.$curtest.txt" "err.$curtest.txt" > /dev/null
	    diffresult=$?
	fi

	if [ $diffresult -eq 0 ]; then
	    testpassed=`expr $testpassed + 1`
	    testres="success"
	else
	    testres="fail"
	fi
	testnum=`expr $testnum + 1`
	echo "Passing test $testnum ........................... $testres"
	rm "res.$curtest.txt"
	rm "err.$curtest.txt"
    fi
done

if [ $testnum -gt 0 ]; then
    passp=`expr 100 \* $testpassed / $testnum`
    failp=`expr 100 - $passp`
    testfailed=`expr $testnum - $testpassed`

    echo "$testpassed ($passp%) tests passed, $testfailed ($failp%) tests failed"
else
    echo "No tests found"
fi

cd ..





