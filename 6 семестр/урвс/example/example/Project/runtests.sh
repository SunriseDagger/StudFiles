#!/bin/sh
#set -x # Set debug mode ON

# Programs runs all tests in testsdir directory
#
#  *.in.test - input data
#  *.out.test - test results
#  *.res.test - failed tests results
#  *.setup.test - test setup script
#  *.finish.test - test finish script
#  *.start.test - script for running the program
#  *.custom.test - script with custom check (e.g. check some file contents)

# program - should point to production program 
program="../calculate"
repeats=1 # number of each tests repeats (Always use $repeats >= 2 for multithreading programs)

testsdir="tests"
setup_prefix="setup"
start_prefix="start"
finish_prefix="finish"
custom_prefix="custom"


if [ -d testsdir ]
then
	echo Directory $testsdir not exists	
	exit 1
fi


# Procedure run script whose name is passed as a parameter
runscript()
{	
	if [ -f $1 ]
	then	
		./$1
	fi
}

pass=0 #number of successfully completed tests
fail=0 #number of unsuccessfully completed tests

cd $testsdir

for file in *.in.test
do
	testname=`echo $file | awk -F. '{print $1}'`		
	
	# Repeat each test $repeats times
	for i in $(seq 1 1 $repeats)
	do
	 	setuptest=$testname.$setup_prefix.sh
		runscript $setuptest

		echo | awk '{printf("Running test %-50s [","'$testname'");}'
		
		starttest=$testname.$start_prefix.sh
		if [ -f $starttest ]
		then
			./$starttest < $file > $testname.res.test
		else
			$program < $file > $testname.res.test
		fi
		
		customtest=$testname.$custom_prefix.sh	
		
		if [ -f $customtest ]
		then		
			runscript $customtest		
			customcheck=$?
		else
			customcheck=0
		fi		
		
		diff -q $testname.out.test $testname.res.test > /dev/null
		diffresult=$?
			
		if [ $diffresult -eq 0 -a $customcheck -eq 0 ]
		then
			echo " PASS ]"
			rm $testname.res.test
			pass=`expr $pass + 1`
		else	
			echo "FAILED]"
			fail=`expr $fail + 1`
		fi	
		
		finishtest=$testname.$finish_prefix.sh	
		runscript $finishtest			
	done

done 

total=`expr $pass + $fail`
passp=`expr 100 \* $pass / $total`
failp=`expr 100 \* $fail / $total`

echo 
echo "$pass ($passp%) tests PASSED, $fail ($failp%) tests FAILED"
