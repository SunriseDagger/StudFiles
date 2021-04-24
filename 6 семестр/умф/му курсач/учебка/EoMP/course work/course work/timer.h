#pragma once

#if defined(_WIN32) || defined (_WIN64) 
#include <windows.h>

class timer{
 public:

	 //������ ������
	 void start(){
		QueryPerformanceFrequency(&fr);
		QueryPerformanceCounter(&start_time);
	 }

	 //�����, �������� � ������ � ��������
	 double time(){
		 LARGE_INTEGER stop, timetime;
		 QueryPerformanceCounter(&stop);
		 timetime.QuadPart = stop.QuadPart - start_time.QuadPart;

		 double timer_time = (double)timetime.QuadPart / (double)fr.QuadPart; //���������� ������� �������

		 return timer_time;
	 }

 private:

	 LARGE_INTEGER start_time, fr;
};

#endif