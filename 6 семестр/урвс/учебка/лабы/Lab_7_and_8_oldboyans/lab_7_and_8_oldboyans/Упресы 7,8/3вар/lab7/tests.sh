#!bin/sh
echo "Тест №1: "
./labka7 > test1.txt
if [ $? -ne 0 ];
	then echo "Провален"
else
	echo "Успешен"
fi
rm test1.txt
