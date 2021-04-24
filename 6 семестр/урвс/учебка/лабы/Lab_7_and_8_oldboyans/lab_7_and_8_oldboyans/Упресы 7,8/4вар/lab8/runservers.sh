#!/bin/sh

#количество серверов (кроме главного)
N=5
FILEDIR="tests/files"

if [ $# -ne 1 ]; then
    echo "Неверное число параметров $#"
    echo "должен быть указан номер порта для главного сервера"
    exit 1
fi

if [ -e serverlist.txt ]; then
    rm serverlist.txt
fi

touch serverlist.txt

if [ -e pids.txt ]; then
    rm pids.txt
fi

touch pids.txt


for i in `seq 1 $N`
do
    port=`expr $1 + $i`
    cd "server$i"
    ./server $port &
    cd ..
    echo $! >> pids.txt
    echo "127.0.0.1:$port" >> serverlist.txt
    
    for f in $(ls $FILEDIR)
    do
	if [ -e "server$i/$f" ]; then
	    :
	else
	    cp "$FILEDIR/$f" "server$i/$f"
	fi
    done
done

./main_server $1 &

echo $! >> pids.txt
