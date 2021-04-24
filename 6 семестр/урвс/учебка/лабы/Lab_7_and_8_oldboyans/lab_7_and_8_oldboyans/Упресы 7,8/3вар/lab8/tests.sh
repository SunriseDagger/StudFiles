#!bin/sh
echo "Тест №1: "
./labka8 1.txt > test1.txt
if [ $? -ne 0 ];
	then echo "Провален"
else
	echo "Успешен"
fi
rm test1.txt

echo "Тест №2: "
./labka8 2.txt> test2.txt
if [ $? -ne 0 ];
	then echo "Провален"
else
	echo "Успешен"
fi
rm test2.txt

echo "Тест №3: "
./labka8 3.txt > test3.txt
if [ $? -ne 0 ];
	then echo "Провален"
else
	echo "Успешен"
fi
rm test3.txt

echo "Тест №4: "
./labka8 4.txt > test4.txt
if [ $? -ne 0 ];
	then echo "Провален"
else
	echo "Успешен"
fi
rm test4.txt

echo "Тест №5: "
./labka8 q.txt > test5.txt
if [ $? -ne 0 ];
	then echo "Провален"
else
	echo "Успешен"
fi
rm test5.txt

echo "Тест №6: "
./labka8 sd.txt > test6.txt
if [ $? -ne 0 ];
	then echo "Провален"
else
	echo "Успешен"
fi
rm test6.txt