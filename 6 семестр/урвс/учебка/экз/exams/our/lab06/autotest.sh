#!/bin/bash
PROGNAME='ur_lab06_var02'
echo "Running test system..."
echo -e -n "Program is builded ...\t\t\t"
if [ -f "./${PROGNAME}" ] ; then
echo "[OK]"
echo -e -n "Checking no files ...\t\t\t"
rm a.txt b.txt >/dev/null 2>/dev/null || true
./${PROGNAME} >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1
echo "[OK]"
echo -e -n "Checking empty files ...\t\t"
>a.txt
>b.txt
if [ `./${PROGNAME} | grep -v '#' | grep '0' | wc -l` -ne 2 ] ; then echo "[FAIL]" && exit 1 ; fi
if [ `cat c.txt | grep '0' | wc -l` -ne 1 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"
echo -e -n "Checking full files ...\t\t\t"
echo "belka strelka ananas" >a.txt
echo "Some text must be here." >b.txt
OUTP=`./${PROGNAME} | grep -v '#' 2>/dev/null`
if [ `cat c.txt | grep '45' | wc -l` -ne 1 ] ; then echo "[FAIL]" && exit 1 ; fi
if [ `echo ${OUTP} | grep '21' | wc -l` -ne 1 ] ; then echo "[FAIL]" && exit 1 ; fi
if [ `echo ${OUTP} | grep '24' | wc -l` -ne 1 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"
else
echo -e "Program not builded.\t\t\t[FAIL]"
exit 1
fi

