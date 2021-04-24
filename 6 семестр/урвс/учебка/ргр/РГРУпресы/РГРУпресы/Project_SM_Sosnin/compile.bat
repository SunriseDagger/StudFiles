echo off
cls

set PFx86="C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"
set PF="C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"
set PFx86_1="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
set PF_1="C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
set PFx86_2="C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"
set PF_2="C:\Program Files\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"

if exist %PFx86% 	call %PFx86% 	x86
if exist %PF%    	call %PF%    	x86
if exist %PFx86_1%	call %PFx86_1% 	x86
if exist %PF_1%    	call %PF_1%    	x86
if exist %PFx86_2% 	call %PFx86_2% 	x86
if exist %PF_2%   	call %PF_2%    	x86

cd /d %~dp0
if exist *.exe del *.exe
if exist *.dll del *.dll
if exist *.obj del *.obj
if exist *.lib del *.lib
if exist *.exp del *.exp
if exist *.RES del *.RES
if exist *.manifest del *.manifest

echo. 
echo.
echo [�����⮢�� 䠩� ����ᮢ resource.RES]
rc ./Project\Project\resource.rc
move /Y ./Project\Project\resource.RES ./resource.RES
echo. 
echo.
echo [��������� �ᯮ��塞��� 䠩�� winmain.exe]
cl /EHsc /D "_UNICODE" /D "UNICODE" ./Project\Project\winmain.cpp kernel32.lib user32.lib gdi32.lib advapi32.lib ./resource.RES
echo. 
echo.
echo [��������� 䠩�� �������᪮� ������⥪� dllmain.dll]
cl /LD /D "_UNICODE" /D "UNICODE" ./DynamicLibrary\DynamicLibrary\dllmain.cpp kernel32.lib user32.lib gdi32.lib advapi32.lib
echo. 
echo.
echo [����७�� ������� � winmain.exe]
mt /manifest winmain.exe.manifest /outputresource:winmain.exe

echo. 
echo.
echo [�������� ����...]
del *.obj *.lib *.exp *.manifest *.RES
echo [��२��������� 䠩���...]
ren  winmain.exe Project.exe
ren  dllmain.dll DynamicLibrary.dll

PAUSE
EXIT