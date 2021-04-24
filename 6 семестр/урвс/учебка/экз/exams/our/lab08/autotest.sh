#!/bin/bash
echo "Running test system..."
echo -e -n "Program is builded ...\t\t\t"
if [ -f "./chat_server" ] ; then
if [ -f "./chat_client" ] ; then
echo "[OK]"
rm -r testdir >/dev/null 2>/dev/null || true
mkdir -p testdir
echo -e -n "Checking work ...\t\t\t"
PORT=`seq 2000 65000 | sort -R | head -n 1`
./chat_server ${PORT} >testdir/out_srv 2>testdir/err_srv &
sleep 0.1
./chat_client "localhost" ${PORT} "user1" >testdir/out_cl1 2>testdir/err_cl1 </dev/null &
sleep 0.1
#echo -e "lalala\nuuuu\nlalala\n"'\\'"cmd exit\n" > testdir/in_cl2
#./chat_client "localhost" ${PORT} "user2" >/dev/null 2>testdir/err_cl2 <testdir/in_cl2
(sleep 0.1 && echo -e "lalala\n" && sleep 0.1 && echo -e "uuuu\n" && sleep 0.1 && echo -e "lalala\n" && sleep 0.1 && echo -e '\\'"cmd exit\n") | ./chat_client "localhost" ${PORT} "user2" >/dev/null 2>testdir/err_cl2
sleep 0.1
kill -TERM `ps ux | grep chat_server | cut -c 9-14` >/dev/null 2>/dev/null
sleep 0.1
if [ `cat testdir/err_cl1 | wc -c` -eq 0 ] ; then echo "[FAIL1]" && exit 1 ; fi
if [ `cat testdir/err_cl2 | wc -c` -ne 0 ] ; then echo "[FAIL2]" && exit 1 ; fi
if [ `cat testdir/err_srv | wc -c` -ne 0 ] ; then echo "[FAIL3]" && exit 1 ; fi
if [ `cat testdir/out_cl1 | sed ':a;N;$!ba;s/Join:user2\nuser2:lalala\nuser2:uuuu\nuser2:lalala\nLeft:user2\n\nConnection lost.../correct/g' | grep 'correct' | wc -l` -ne 1 ] ; then echo "[FAIL4]" && exit 1 ; fi
if [ `cat testdir/out_srv | sed 's/Starting server.../m1/g ; s/Server start at port:[0-9]\{1,\}/m2/g ; s/+localhost \[127\.0\.0\.1\] new connect!/m3/g ; s/New user: user1/m4/g ; s/New user: user2/m5/g ; s/+ user2/m6/g ; s/- user2/m7/g ; s/= lalala/m8/g ; s/= uuuu/m9/g ; s/lalala/m10/g ; s/uuuu/m11/g ; /^$/d' | sort | uniq | wc -l` -ne 11 ] ; then echo "[FAIL5]" && exit 1 ; fi
echo "[OK]"
rm -r testdir >/dev/null 2>/dev/null || true
else
echo -e "Program not builded.\t\t\t[FAIL]"
exit 1
fi
else
echo -e "Program not builded.\t\t\t[FAIL]"
exit 1
fi

