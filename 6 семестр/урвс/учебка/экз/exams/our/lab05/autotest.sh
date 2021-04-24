#!/bin/bash
PROGNAME='ur_lab05_var02'
echo "Running test system..."
echo -e -n "Program is builded ...\t\t\t"
if [ -f "./${PROGNAME}" ] ; then
echo "[OK]"
echo -e -n "Checking correct work ...\t\t"
NUMCORR=` ./${PROGNAME} | sed 's/[0-9]\{1,\}: [0-9]\{1,\}$/p1/g ; s/[0-9]\{1,\}: [0-9]\{1,\}\.[0-9]\{1,\}$/p2/g ; s/Child #P1: [0-9]\{1,\} created\.$/m1/g ; s/Child #P2: [0-9]\{1,\} created\.$/m2/g ; s/Child #P2: signal sended\.$/m3/g ; s/Child #P1: signal accepted\.$/m4/g ; s/Child #P2: work done\.$/m5/g ; s/Child #P1: work done\.$/m6/g' | uniq | wc -l`
if [ ${NUMCORR} -ne 8 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"
else
echo -e "Program not builded.\t\t\t[FAIL]"
exit 1
fi

