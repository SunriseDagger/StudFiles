echo off
cls

cd /d %~dp0
RMDIR Project\Project\Debug\ /S /Q 
RMDIR Project\Project\Release\ /S /Q 
RMDIR Project\Debug\ /S /Q 
RMDIR Project\Release\ /S /Q 
cd ./Project
if exist *.ncb del *.ncb
cd ./Project
if exist *.user del *.user
cd ../../

RMDIR DynamicLibrary\DynamicLibrary\Debug\ /S /Q 
RMDIR DynamicLibrary\DynamicLibrary\Release\ /S /Q 
RMDIR DynamicLibrary\Debug\ /S /Q 
RMDIR DynamicLibrary\Release\ /S /Q 
cd ./DynamicLibrary
if exist *.ncb del *.ncb
cd ./DynamicLibrary
if exist *.user del *.user
cd ../../