#include <stdio.h>
#include <windows.h>
#include <time.h>

// ���������� ������ ������
extern "C"
__declspec(dllexport)
WORD getDayOfWeek()
{
	SYSTEMTIME SystemTime;
	GetSystemTime(&SystemTime);
	return SystemTime.wDayOfWeek;
}


// ���������� ������� SSE2
extern "C"
__declspec(dllexport)
void cpuid(int regs[4], int func)
{
	int ieax, iebx, iecx, iedx;
 	__asm
	{
   		mov eax, func
		mov ecx, 2	//����� �������� ��� 2 ������
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