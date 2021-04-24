#!/bin/bash
BASEDIR=`dirname $0`
SCRIPTDIR=`cd $BASEDIR ; pwd`
CONFIGFILE='./config.txt'
TMPFILE='/tmp/summator_msg'

echo "Running test system..."
if [ -f "${SCRIPTDIR}/summator" ] ; then

echo -e -n "Incorrect number of arguments checking ...\t"
"${SCRIPTDIR}/summator" >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || "${SCRIPTDIR}/summator" 42 42 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"

echo -e -n "Incorrect value of arguments checking ...\t"
"${SCRIPTDIR}/summator" -100 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"


echo -e -n "Not found config file checking ...\t\t"
rm "${CONFIGFILE}" >/dev/null 2>/dev/null
"${SCRIPTDIR}/summator" 10 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"

echo -e -n "Empty config file checking ...\t\t\t"
> "${CONFIGFILE}"
"${SCRIPTDIR}/summator" 10 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"

echo -e -n "Acces denied temp file checking ...\t\t"
echo "10" > "${CONFIGFILE}"
> "${TMPFILE}"
chmod 000 "${TMPFILE}"
"${SCRIPTDIR}/summator" 10 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"
chmod 777 "${TMPFILE}"
rm "${TMPFILE}"

echo -e -n "Normal test #1 checking ...\t\t\t"
echo "3" > "${CONFIGFILE}"
if [ `"${SCRIPTDIR}/summator" 1 2>/dev/null | wc -l` -ne 2 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"

echo -e -n "Normal test #2 checking ...\t\t\t"
echo "1000" > "${CONFIGFILE}"
if [ `"${SCRIPTDIR}/summator" 100 2>/dev/null | wc -l` -ne 101 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"

else
echo -e "Program not builded.\t\t\t\t[FAIL]"
exit 1
fi

