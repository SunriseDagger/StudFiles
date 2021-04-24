#pragma comment(linker,"/manifestdependency:\"type='win32' \
                        name='Microsoft.Windows.Common-Controls' \
                        version='6.0.0.0' processorArchitecture='*'\
                        publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <string>
#include <sstream>
#include <tchar.h>
#include "Resource.h"

HWND hwnd, button, button_exit, screenWidthLabel, cpuLabel;
static HBRUSH hBrush = CreateSolidBrush(RGB(255,255,255));

bool check_flag(int flag, int REG_EDX)
{
  int mask=1;
  mask <<= flag;
  
  if ( (REG_EDX & mask) != 0) {
    return true;
  } else {
    return false;
  }
}

// Функция обращения к DLL
void thread()
{
	HINSTANCE hinstLib = LoadLibrary(TEXT("DynamicLibrary.dll"));
	if (hinstLib != NULL)
	{
		// Импортируем функции
		typedef int (*getSystemMetrics_t)();
		getSystemMetrics_t getSystemMetrics = (getSystemMetrics_t)GetProcAddress(hinstLib, "getSystemMetrics");
		typedef void (*cpuid_t)(int[4], int);
		cpuid_t cpuid = (cpuid_t)GetProcAddress(hinstLib, "cpuid");
	
		// Получаем пусть к папке Windows
		int Width = getSystemMetrics();

		// Идентифицируем размер кэша 1 уровня (L1 cache)
		int CPUInfo[4];
		
		cpuid(CPUInfo, 1);

		bool SSE2 = check_flag(26, CPUInfo[3]); 
		

		// Выгружаем DLL!
		FreeLibrary(hinstLib);
	
		// Формируем строки для вывода в окно
		char screenWidth[1024];
		sprintf_s(screenWidth, "Основное задание:\n\"Определить ширину экрана\"\
\n\n Ширина экрана %d пикселей", Width);
		
		char cpuString[1024];
		sprintf_s(cpuString, "Дополнительное задание:\n\
\"Определить наличие поддержки технологии SSE2\"\n\n\
Технология SSE2 %s", SSE2 ? "поддерживается" : "не поддерживается");

		// Перекодируем их
		WCHAR screenWidthWString[1024];
		MultiByteToWideChar(CP_ACP, 0, screenWidth, strlen(screenWidth) + 1, (LPWSTR)screenWidthWString, sizeof(screenWidthWString) / sizeof(screenWidthWString[0]));
		WCHAR cpuWString[1024];
		MultiByteToWideChar(CP_ACP, 0, cpuString, strlen(cpuString) + 1, (LPWSTR)cpuWString, sizeof(cpuWString) / sizeof(cpuWString[0]));

		// И выведем в окно
		SetWindowText(screenWidthLabel, LPWSTR(screenWidthWString));
		SetWindowText(cpuLabel, LPWSTR(cpuWString));	
	}	
	else
	{
		MessageBox(hwnd, L"Ошибка загрузки DLL", L"Ошибка", 0);
	}	
}

// Обработчик сообщений
LONG WINAPI WndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
	switch (Message)
	{	
		case WM_CTLCOLORSTATIC:
		{
			DWORD CtrlID = GetDlgCtrlID((HWND)lparam); 
			if (CtrlID == 3001 || CtrlID == 3002)
			{
			   HDC hdcStatic = (HDC) wparam;
			   SetTextColor(hdcStatic, RGB(0,0,0));
			   SetBkColor(hdcStatic, RGB(255,255,255));
			   return (INT_PTR)hBrush;
			}
		}
		break;
		case WM_COMMAND:
		{
			if (LOWORD(wparam) == 3000)
			{
				// Нажата кнопка
				HANDLE hThread;
				DWORD IDThread;
				hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, NULL, 0, &IDThread); // Запускаем функцию в новом потоке
				CloseHandle(hThread);		
			}
			if (LOWORD(wparam) == 3003)
			{
				// Нажата кнопка
				PostQuitMessage(0);	
			}
		}
		break;
		case WM_DESTROY: 
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Message, wparam, lparam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR	 lpCmdLine,
				   int	   nCmdShow)
{
	WNDCLASS w;
	memset(&w, 0, sizeof(WNDCLASS));	
	w.style = CS_HREDRAW | CS_VREDRAW;	
	w.lpfnWndProc = WndProc;			// Обработчик сообщений
	w.hInstance = hInstance;			
	w.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
	w.lpszClassName = L"ResourceManagementProject";
	w.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RESFUND));
	RegisterClass(&w);

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	GetVersionEx(&osvi);
	
	if(osvi.dwMajorVersion < 6)
    ncm.cbSize -= sizeof(ncm.iPaddedBorderWidth);
	
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	
	HFONT hf = CreateFontIndirect(&ncm.lfMessageFont);

	HDC hDCScreen = GetDC(NULL);
	int Horres = GetDeviceCaps(hDCScreen, HORZRES);
	int Vertres = GetDeviceCaps(hDCScreen, VERTRES);
	int Height = 220;
	int Width = 360;
	int ButtonHeight = 26;
	int ButtonWidth = 80;

	hwnd = CreateWindow(L"ResourceManagementProject", L"ПМ-01, Александр Зубарев, Вариант 7", \
		WS_OVERLAPPED| WS_CAPTION| WS_SYSMENU | WS_MINIMIZEBOX, \
		(Horres-Width)/2, (Vertres-Height)/2, Width, Height, NULL, NULL, hInstance, NULL);
	
	button = CreateWindow(L"button", L"Выполнить", \
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_VCENTER | BS_CENTER, Width-2*ButtonWidth-16, Height-2*ButtonHeight-8,\
		ButtonWidth, ButtonHeight, hwnd, (HMENU)3000, hInstance, NULL);

	button_exit = CreateWindow(L"button", L"Выход", \
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_VCENTER | BS_CENTER, Width-ButtonWidth-10, Height-2*ButtonHeight-8,\
		ButtonWidth, ButtonHeight, hwnd, (HMENU)3003, hInstance, NULL);

	screenWidthLabel = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON,\
		5, 5, Width-15, Height/2-ButtonHeight-12, hwnd, (HMENU)3001, hInstance, NULL);
	
	cpuLabel = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON, \
		5, Height/2-ButtonHeight-2, Width-15, Height/2-ButtonHeight-12, hwnd, (HMENU)3002, hInstance, NULL);

	for(int i = 0; i < 4; i++)
		SendDlgItemMessage(hwnd, 3000+i, WM_SETFONT, (WPARAM)hf, TRUE);

	ShowWindow(hwnd, nCmdShow);

	UpdateWindow(hwnd);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
