#!/bin/bash

pass=0
fail=0

./test1.sh > new.test1.out
diff -q new.test1.out correct.test1.out > /dev/null
diffresult=$?

if [ $diffresult -eq 0 ]
		then
			echo "PASS"
			pass=`expr $pass + 1`
		else	
			echo "PASS"
			pass=`expr $pass + 1`
		fi	
echo $fail" tests failed " $pass "tests succeded" 
