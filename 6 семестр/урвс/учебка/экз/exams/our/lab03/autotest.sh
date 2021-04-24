#!/bin/bash
PROGNAME='tablegen'
echo "Running test system..."
echo -e "Program is builded ...\t\t\t\t[OK]"
if [ -f "./${PROGNAME}" ] ; then
echo -e -n "Checking correct work ...\t\t\t"
NUMCORR=` ./${PROGNAME} | sed ':a;N;$!ba;s/Implicit opening of standard files\.\.\.\ndescr\tname\t\tperm\tinode\tnlink\tUID\tGID\tsize\n---------------------------------------------------------------------\n0\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n1\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n2\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n\nOpening of the user file\.\.\.\ndescr\tname\t\tperm\tinode\tnlink\tUID\tGID\tsize\n---------------------------------------------------------------------\n0\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n1\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n2\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n[0-9]\{1,\}\tfile\.txt\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\n\nClosing of a standart file of input\.\.\.\ndescr\tname\t\tperm\tinode\tnlink\tUID\tGID\tsize\n---------------------------------------------------------------------\n1\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n2\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n[0-9]\{1,\}\tfile\.txt\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\n[0-9]\{1,\}\tfile1\.txt\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\n\nReception of a copy of a descriptor of the user file\.\.\.\ndescr\tname\t\tperm\tinode\tnlink\tUID\tGID\tsize\n---------------------------------------------------------------------\n1\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n2\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n[0-9]\{1,\}\tfile\.txt\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\n[0-9]\{1,\}\tfile1\.txt\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\n[0-9]\{1,\}\tfile\.txt\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\n\nClose file1\.txt\ndescr\tname\t\tperm\tinode\tnlink\tUID\tGID\tsize\n---------------------------------------------------------------------\n1\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n2\t\t\t\t[0-9]\{1,\}\t[0-9]\{1,\}\n[0-9]\{1,\}\tfile\.txt\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\n[0-9]\{1,\}\tfile\.txt\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}\t[0-9]\{1,\}/correct/g' | grep 'correct' | wc -l `
if [ ${NUMCORR} -ne 1 ] ; then echo "[FAIL]" && exit 1 ; fi
echo "[OK]"
else
echo -e "Program not builded.\t\t\t\t[FAIL]"
exit 1
fi

