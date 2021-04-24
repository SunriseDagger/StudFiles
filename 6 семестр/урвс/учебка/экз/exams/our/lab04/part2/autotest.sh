#!/bin/bash
BASEDIR=`dirname $0`
SCRIPTDIR=`cd $BASEDIR ; pwd`
CONFIGFILE='./config.txt'
TMPFILE='/tmp/summator_msg'

echo "Running test system..."
if [ -f "${SCRIPTDIR}/summator_parent" ] ; then

echo -e -n "Incorrect number of arguments checking ...\t"
"${SCRIPTDIR}/summator_parent" >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || "${SCRIPTDIR}/summator_parent" 42 42 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"

echo -e -n "Incorrect value of arguments checking ...\t"
"${SCRIPTDIR}/summator_parent" -100 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"

echo -e -n "Not found config file checking ...\t\t"
rm "${CONFIGFILE}" >/dev/null 2>/dev/null
"${SCRIPTDIR}/summator_parent" 10 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"

echo -e -n "Empty config file checking ...\t\t\t"
> "${CONFIGFILE}"
"${SCRIPTDIR}/summator_parent" 10 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"

echo -e -n "Acces denied summator_children checking ...\t"
echo "10" > "${CONFIGFILE}"
chmod -x "${SCRIPTDIR}/summator_children"
"${SCRIPTDIR}/summator_parent" 10 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"
chmod +x "${SCRIPTDIR}/summator_children"

echo -e -n "Normal test #1 checking ...\t\t\t"
echo "3" > "${CONFIGFILE}"
"${SCRIPTDIR}/summator_parent" 1 2>/dev/null >/dev/null && RES=0 || RES=1
if [ ${RES} -ne 0 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"

echo -e -n "Normal test #2 checking ...\t\t\t"
echo "1000" > "${CONFIGFILE}"
"${SCRIPTDIR}/summator_parent" 100 2>/dev/null >/dev/null && RES=0 || RES=1
if [ ${RES} -ne 0 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"

else
echo -e "Program not builded.\t\t\t\t[FAIL]"
exit 1
fi

