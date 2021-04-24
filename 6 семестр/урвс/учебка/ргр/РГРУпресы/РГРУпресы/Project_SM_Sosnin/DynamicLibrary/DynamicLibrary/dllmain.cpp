#include <stdio.h>
#include <windows.h>
#include <time.h>

// Определить директорию Windows
extern "C"
__declspec(dllexport)
void getWindowsDirectory(TCHAR* windir)
{
	GetWindowsDirectory(windir, 256);
}


// Определить размер кэша команд первого уровня
extern "C"
__declspec(dllexport)
void cpuid(int regs[4], int func)
{
	int ieax, iebx, iecx, iedx;
 	__asm
	{
   		mov eax, func
		mov ecx, 1	//Чтобы получать кэш первого уровня
		cpuid
   		mov ieax, eax
    	mov iebx, ebx
   		mov iecx, ecx
    	mov iedx, edx
	}
	regs[0] = ieax;
	regs[1] = iebx;
  	regs[2] = iecx;
  	regs[3] = iedx;
}