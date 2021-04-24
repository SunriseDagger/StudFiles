#!/bin/bash
PROGNAME='ur_lab07_var02'
echo "Running test system..."
echo -e -n "Program is builded ...\t\t\t"
if [ -f "./${PROGNAME}" ] ; then
echo "[OK]"
echo -e -n "Checking no arguments ...\t\t"
./${PROGNAME} >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1
echo "[OK]"
echo -e -n "Checking no file ...\t\t\t"
./${PROGNAME} ./gahsgsadfsdtyDRRDASGSJBXVZB >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1
echo "[OK]"
echo -e -n "Checking Tatyana mail ...\t\t"
rm out1.tmp out2.tmp || true
TMPRES=0
./${PROGNAME} ./tatyana_mail.txt 2>/dev/null | sed '/^$/d' >out1.tmp || TMPRES=1
if [ ${TMPRES} -ne 0 ] ; then echo "[FAIL]" && exit 1 ; fi
cat tatyana_mail.txt | sed '/^$/d' >out2.tmp
if [ `diff out2.tmp out1.tmp | wc -c` -ne 0 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"
rm out1.tmp out2.tmp || true
else
echo -e "Program not builded.\t\t\t[FAIL]"
exit 1
fi

