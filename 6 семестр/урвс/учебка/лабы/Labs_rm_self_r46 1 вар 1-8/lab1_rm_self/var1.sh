#!/bin/sh
# Copyleft (c) Str1ker, 2016

STARTDIR=$1
if [ "$1" = "" ]; then
echo "No directory specified; using current directory"
STARTDIR="."
#STARTDIR=$(pwd) # var=$(foo) reads output of external program foo to var
fi

cd "$STARTDIR" 2> /dev/null || (echo "Directory '$STARTDIR' does not exists" && exit 1)
# this echo is for debug and may be cutted
echo "Going through $STARTDIR"

declare -i COUNTER=0
# list subdirs, do not halts if none found
# for each file in pwd
for D in *; do
	# check whether it is a directory or not
	if [ -d "${D}" ]; then
		# if yes dive in, check subdirs with ls, print, increase counter, float.
		cd $D 2> /dev/null &&	ls -d1 */ > /dev/null 2> /dev/null && echo $D && COUNTER=$COUNTER+1 && cd ..
	fi
done

# if none subdirs match print it
if [ $COUNTER -eq 0 ]; then
	echo "The given directory does not contains subdirectories with subdirectories."
	exit 1
fi

# print count of passed directories
echo $COUNTER
# good tone to return 0 code
exit 0