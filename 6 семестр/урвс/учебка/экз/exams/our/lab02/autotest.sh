#!/bin/bash
BASEDIR=`dirname $0`
SCRIPTDIR=`cd $BASEDIR ; pwd`
TMPFILE='/tmp/test.tmp'
TMPDIR='/tmp/test.tmpdir'
echo "Running test system..."
if [ -f "${SCRIPTDIR}/lsd" ] ; then

echo -e -n "Incorrect number of arguments checking ...\t"
"${SCRIPTDIR}/lsd" >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || "${SCRIPTDIR}/lsd" ext 12 >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"

echo -e -n "Not found directory detection checking ...\t"
mkdir -p "${TMPDIR}"
cd "${TMPDIR}"
touch "1.txt" "2.txt" "3.txt"
chmod 000 "${TMPDIR}"
"${SCRIPTDIR}/lsd" txt >/dev/null 2>/dev/null && echo "[FAIL]" && exit 1 || echo "[OK]"
chmod 755 "${TMPDIR}"
cd ..
rm -rf "${TMPDIR}"

echo -e -n "Standard checking ...\t\t\t\t"
cd /etc/modprobe.d
"${SCRIPTDIR}/lsd" conf >"$TMPFILE" 2>/dev/null || `echo "[FAIL]" && exit 1`
if [ `cat "${TMPFILE}" | grep 'resolv.conf' | wc -l` -ne 1 ] ; then echo "[FAIL]" && exit 1 ; fi
if [ `cat "${TMPFILE}" | grep 'sysctl.conf' | wc -l` -ne 1 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"
rm "${TMPFILE}"

echo -e -n "Empty result checking ...\t\t\t"
cd '/'
if [ `"${SCRIPTDIR}/lsd" tushenka 2>/dev/null | wc -l` -ne 0 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"

else
echo -e "Program not builded.\t\t\t\t[FAIL]"
exit 1
fi

