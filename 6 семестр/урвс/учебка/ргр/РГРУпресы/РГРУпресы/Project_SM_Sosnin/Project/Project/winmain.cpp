#pragma comment(linker,"/manifestdependency:\"type='win32' \
                        name='Microsoft.Windows.Common-Controls' \
                        version='6.0.0.0' processorArchitecture='*'\
                        publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <string>
#include <sstream>
#include <tchar.h>
#include "Resource.h"

#define TBColor RGB(255, 255, 180)
#define BGColor RGB(230, 255, 200)

HWND hwnd, button, button_exit, winDirLabel, cpuLabel;
static HBRUSH hBrush = CreateSolidBrush(TBColor);

// Функция обращения к DLL
void thread()
{
	HINSTANCE hinstLib = LoadLibrary(TEXT("DynamicLibrary.dll"));
	if (hinstLib != NULL)
	{
		// Импортируем функции
		typedef void (*getWindowsDirectory_t)(TCHAR*);
		getWindowsDirectory_t getWindowsDirectory = (getWindowsDirectory_t)GetProcAddress(hinstLib, "getWindowsDirectory");
		typedef void (*cpuid_t)(int[4], int);
		cpuid_t cpuid = (cpuid_t)GetProcAddress(hinstLib, "cpuid");
	
		// Получаем пусть к папке Windows
		TCHAR winDir[256];
		getWindowsDirectory(winDir);

		// Идентифицируем размер кэша 1 уровня (L1 cache)
		int CPUInfo[4];
		
		cpuid(CPUInfo, 4);
		int cache_type = CPUInfo[0] & 0x1F; 

		char * cache_type_string;
        switch (cache_type) {
            case 1: cache_type_string = "Кэш данных"; break;
            case 2: cache_type_string = "Кэш инструкций"; break;
            case 3: cache_type_string = "Универсальный кэш"; break;
            default: cache_type_string = "Неопределен"; break;
        }

		int cache_level = (CPUInfo[0] >>= 5) & 0x7;
		int cache_is_self_initializing = (CPUInfo[0] >>= 3) & 0x1;
        int cache_is_fully_associative = (CPUInfo[0] >>= 1) & 0x1;
        unsigned int cache_sets = CPUInfo[2] + 1;
        unsigned int cache_coherency_line_size = (CPUInfo[1] & 0xFFF) + 1;
        unsigned int cache_physical_line_partitions = ((CPUInfo[1] >>= 12) & 0x3FF) + 1;
        unsigned int cache_ways_of_associativity = ((CPUInfo[1] >>= 10) & 0x3FF) + 1;

		size_t cache_total_size = cache_ways_of_associativity * \
			cache_physical_line_partitions * cache_coherency_line_size * cache_sets;

		// Выгружаем DLL!
		FreeLibrary(hinstLib);
	
		// Формируем строки для вывода в окно
		char winDirString[1024];
		char tmpString[1024];

		int i = 0;
		while(winDir[i] != '\0')
		{
			tmpString[i] = char(winDir[i]);
			i++;
		}
		tmpString[i] = '\0';
		sprintf_s(winDirString, "Основное задание:\n\"Определить директорию Windows\"\
\n\nРасположение директории Windows:\n%s", tmpString);
		
		char cpuString[1024];
		sprintf_s(cpuString, "Дополнительное задание:\n\
\"Определить размер кэша команд первого уровня\"\n\n\
Кэш: %d уровня (L%d cache)\n\
Тип: %s\n\
Множеств: %d\n\
Путей: %d\n\
Физических линий: %d\n\
Связанных линий: %d\n\
Размер: %d байт (%d КБ)",\
		cache_level, cache_level,\
		cache_type_string,\
		cache_sets,\
		cache_ways_of_associativity,\
		cache_physical_line_partitions,\
		cache_coherency_line_size,\
		cache_total_size, cache_total_size >> 10);

		// Перекодируем их
		WCHAR winDirWString[1024];
		MultiByteToWideChar(CP_ACP, 0, winDirString, strlen(winDirString) + 1, (LPWSTR)winDirWString, sizeof(winDirWString) / sizeof(winDirWString[0]));
		WCHAR cpuWString[1024];
		MultiByteToWideChar(CP_ACP, 0, cpuString, strlen(cpuString) + 1, (LPWSTR)cpuWString, sizeof(cpuWString) / sizeof(cpuWString[0]));

		// И выведем в окно
		SetWindowText(winDirLabel, LPWSTR(winDirWString));
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
			   SetBkColor(hdcStatic, TBColor);
			   return (INT_PTR)hBrush;
			}
		}
		break;
		case WM_COMMAND:
			if (LOWORD(wparam) == 3000)
			{
				HANDLE hThread;
				DWORD IDThread;
				hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, NULL, 0, &IDThread); // Запускаем функцию в новом потоке
				CloseHandle(hThread);		
			}
			if (LOWORD(wparam) == 3003)
			{
				PostQuitMessage(0);	
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
	w.hbrBackground = CreateSolidBrush(BGColor);
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
	int Height = 240;
	int Width = 360;
	int ButtonHeight = 26;
	int ButtonWidth = 80;

	hwnd = CreateWindow(L"ResourceManagementProject", L"ПМ-01, Антон Соснин, Вариант 17", \
		WS_OVERLAPPED| WS_CAPTION| WS_SYSMENU | WS_MINIMIZEBOX, \
		(Horres-Width)/2, (Vertres-Height)/2, Width, Height, NULL, NULL, hInstance, NULL);
	
	button = CreateWindow(L"button", L"Выполнить", \
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_VCENTER | BS_CENTER, Width-2*ButtonWidth-16, Height-2*ButtonHeight-8,\
		ButtonWidth, ButtonHeight, hwnd, (HMENU)3000, hInstance, NULL);

	button_exit = CreateWindow(L"button", L"Выход", \
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_VCENTER | BS_CENTER, Width-ButtonWidth-10, Height-2*ButtonHeight-8,\
		ButtonWidth, ButtonHeight, hwnd, (HMENU)3003, hInstance, NULL);

	winDirLabel = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON,\
		5, 5, Width/2-8, Height-2*ButtonHeight-20, hwnd, (HMENU)3001, hInstance, NULL);
	
	cpuLabel = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHBUTTON, \
		Width/2+2, 5, Width/2-13, Height-2*ButtonHeight-20, hwnd, (HMENU)3002, hInstance, NULL);

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
