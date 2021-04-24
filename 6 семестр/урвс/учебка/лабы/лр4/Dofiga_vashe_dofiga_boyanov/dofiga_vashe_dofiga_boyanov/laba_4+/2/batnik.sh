#!/bin/bash
echo "--------------------Compiling...---------------------"
rm main.exe
rm pod1.exe
rm pod2.exe
g++ main.cpp -o main.exe
g++ pod1.cpp -o pod1.exe
g++ pod2.cpp -o pod2.exe
echo "--------------------O'k...---------------------------"
