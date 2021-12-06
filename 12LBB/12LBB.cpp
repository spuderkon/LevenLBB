// 12LBB.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "12LBB.h"
#include "commdlg.h"
#include "Resource.h"
#include "windows.h"
#include "richedit.h"
#include "winuser.h"
#include "string.h"
#include "iostream"
#include "fileapi.h"

#define MAX_LOADSTRING 100
#define IDC_BUTTON1 2001
#define IDC_BUTTON2 2003
#define IDC_BUTTON3 2004
#define IDC_BUTTON4 2005
#define IDC_BUTTON5 2006
#define IDC_HEDIT 2011
#define IDC_HEDIT1 2012

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hButton1, hButton2, hButton3, hButton4, hButton5, hEdit, hEdit1, dlg;
HANDLE File;
OPENFILENAME cr, re;
WIN32_FIND_DATA inf;

DWORD dwOffset;
wchar_t buf[255];
TCHAR Text[MAX_PATH];

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL ReadAsync(HANDLE file, LPVOID buf, DWORD offset, DWORD size, LPOVERLAPPED overlap)
{
    ZeroMemory(overlap, sizeof(OVERLAPPED));
    overlap->Offset = offset;
    overlap->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    BOOL ret = ReadFile(file, buf, size, NULL, overlap);
    if (ret == FALSE && GetLastError() != ERROR_IO_PENDING)
        CloseHandle(overlap->hEvent), overlap->hEvent = NULL;
    return ret;
}

BOOL WriteAsync(HANDLE file, LPCVOID buf, DWORD offset, DWORD size, LPOVERLAPPED overlap)
{
    ZeroMemory(overlap, sizeof(OVERLAPPED));
    overlap->Offset = offset;
    overlap->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    BOOL ret = WriteFile(file, buf, size, NULL, overlap);
    if (ret == FALSE && GetLastError() != ERROR_IO_PENDING)
        CloseHandle(overlap->hEvent), overlap->hEvent = NULL;
    return ret;
}

BOOL FinishIo(LPOVERLAPPED overlap)
{
    if (overlap->hEvent != NULL) {
        DWORD res = WaitForSingleObject(overlap->hEvent, INFINITE);
        if (res == WAIT_OBJECT_0) {
            CloseHandle(overlap->hEvent), overlap->hEvent = NULL;
            return TRUE;
        }
    }
    return FALSE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY12LBB, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY12LBB));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY12LBB));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY12LBB);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }
    else
    {
        hButton1 = CreateWindowEx(0, L"Button", L"Создать", BS_CENTER | WS_CHILD | WS_VISIBLE, 30, 30, 100, 30, hWnd, (HMENU)IDC_BUTTON1, hInst, NULL);

        hButton2 = CreateWindowEx(0, L"Button", L"Добавить в файл", BS_CENTER | WS_CHILD | WS_VISIBLE, 15, 70, 150, 30, hWnd, (HMENU)IDC_BUTTON2, hInst, NULL);

        hButton3 = CreateWindowEx(0, L"Button", L"Прочитать", BS_CENTER | WS_CHILD | WS_VISIBLE, 30, 110, 100, 30, hWnd, (HMENU)IDC_BUTTON3, hInst, NULL);

        hButton4 = CreateWindowEx(0, L"Button", L"Открыть", BS_CENTER | WS_CHILD | WS_VISIBLE, 30, 140, 100, 30, hWnd, (HMENU)IDC_BUTTON4, hInst, NULL);

        hButton5 = CreateWindowEx(0, L"Button", L"Узнать", BS_CENTER | WS_CHILD | WS_VISIBLE, 30, 170, 100, 30, hWnd, (HMENU)IDC_BUTTON5, hInst, NULL);

        hEdit = CreateWindowEx(0, L"Edit", L"", WS_CHILD | WS_BORDER | WS_VISIBLE | ES_MULTILINE | LBS_MULTIPLESEL, 170, 30, 300, 400, hWnd, (HMENU)IDC_HEDIT, hInst, NULL);
    }


    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DWORD dwFlags = FILE_FLAG_OVERLAPPED;
    TCHAR path[250] = L"text.txt";
    OVERLAPPED overlap;

    inf.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    // inf.ftCreationTime = FindFirstFile(buf, &inf);
  
    switch (message)
    {
    case WM_MOVE:
        /*TCHAR s3[100];
        TCHAR s4[100];
        _stprintf_s(s3, _T("%i"), LOWORD(lParam));
        WritePrivateProfileString(TEXT("General"), TEXT("WINX"), s3, buf);
        _stprintf_s(s4, _T("%i"), HIWORD(lParam));
        WritePrivateProfileString(TEXT("General"), TEXT("WINY"), s4, buf);*/
        break;

    case WM_SIZE:
        /*TCHAR s[100];
        TCHAR s2[100];
        _stprintf_s(s, _T("%i"), LOWORD(lParam));
        WritePrivateProfileString(TEXT("General"), TEXT("WINW"), s, buf);
        _stprintf_s(s2, _T("%i"), HIWORD(lParam));
        WritePrivateProfileString(TEXT("General"), TEXT("WINH"), s2, buf);*/
        break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDC_BUTTON1:
        {
            ZeroMemory(&re, sizeof(cr));
            wchar_t filter[] = L"Data Files(*.txt)\0*.txt\0All Files\0*.*\0\0";
            char filterExt[][6] = { ".txt" };
            wchar_t cCustomFilter[256] = L"\0\0";
            int nFilterIndex = 0;
            cr.lStructSize = sizeof(OPENFILENAME);
            cr.hwndOwner = dlg;
            cr.hInstance = hInst;
            cr.lpstrFilter = filter;
            cr.lpstrCustomFilter = cCustomFilter;
            cr.nMaxCustFilter = 256;
            cr.nFilterIndex = nFilterIndex;
            cr.lpstrFile = buf;
            cr.lpstrFileTitle = NULL;
            cr.nMaxFile = 255;
            cr.nMaxFileTitle = 255;
            cr.lpstrInitialDir = NULL;
            cr.lpstrTitle = 0;
            cr.Flags = OFN_FILEMUSTEXIST;
            cr.nFileOffset = 0;
            cr.nFileExtension = 0;
            cr.lpstrDefExt = NULL;
            cr.lCustData = NULL;
            cr.lpfnHook = NULL;
            cr.lpTemplateName = NULL;

            if (GetSaveFileName(&cr))
            {
                HANDLE hFile = CreateFile(buf, GENERIC_READ |
                    GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_NEW,
                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);



                if (INVALID_HANDLE_VALUE != hFile)
                {
                    MessageBox(0, L"Файл создан", L"Сообщение", MB_OK);
                }
                else
                {
                    MessageBox(0, L"Ошибка при создании файла", L"Сообщение", MB_OK);
                }
                CloseHandle(hFile);
            }
        }
        break;

        case IDC_BUTTON2:
        {

            int n = GetWindowTextLength(hEdit);
            WCHAR* buf4 = new WCHAR[n + 1];
            char* output_buf2 = new char[n + 1];
            GetWindowText(hEdit, buf4, n + 1);

            WideCharToMultiByte(0, 0, buf4, -1, output_buf2, n + 1, 0, 0);
            File = CreateFile(buf, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
            WriteAsync(File, output_buf2, 0, n, &overlap);

            /*char buf3[999];
            WCHAR buf4[999];

            File = CreateFile(buf, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, dwFlags, NULL);

            GetWindowText(hEdit, buf4, 999);
            WideCharToMultiByte(0, 0, buf4, -1, buf3, 999, 0, 0);
            WriteAsync(File, buf3, 0, 999, &overlap);*/
            FinishIo(&overlap);
            CloseHandle(File);

        }
        break;

        case IDC_BUTTON3:
        {
            char* buf2 = new char[30]{};
            WCHAR buf1[256];
            File = CreateFile(buf, GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
            ReadAsync(File, buf2, 0, 256, &overlap);
            MultiByteToWideChar(0, 0, buf2, 256, buf1, 256);
            SetWindowText(hEdit, buf1);
            FinishIo(&overlap);
            CloseHandle(File);
            //char buf2[999];
            //WCHAR buf4[999];
            ////GetWindowText(hEdit, buf4, 999);
            //File = CreateFile(buf, GENERIC_READ, 0, NULL, OPEN_ALWAYS, dwFlags, NULL);
            ////WritePrivateProfileString(TEXT("General"), TEXT("File"), buf, TEXT("C:\\ProgramData\\app.ini"));
            //ReadAsync(File, buf2, dwOffset, 999, &overlap);
            //FinishIo(&overlap);
            //CloseHandle(File);
            //MultiByteToWideChar(0, 0, buf2, 999, buf4, 999);
            //SetWindowTextA(hEdit, buf2);                 
        }
        break;

        case IDC_BUTTON4:
        {
            ZeroMemory(&cr, sizeof(re));
            re.lStructSize = sizeof(OPENFILENAME);
            re.hwndOwner = dlg;
            re.hInstance = hInst;
            re.nMaxCustFilter = 256;
            re.lpstrFile = buf;
            re.lpstrFileTitle = NULL;
            re.nMaxFile = 255;
            re.nMaxFileTitle = 255;
            re.lpstrInitialDir = NULL;
            re.lpstrTitle = 0;
            re.Flags = OFN_FILEMUSTEXIST;
            re.nFileOffset = 0;
            re.nFileExtension = 0;
            re.lpstrDefExt = NULL;
            re.lCustData = NULL;
            re.lpfnHook = NULL;
            re.lpTemplateName = NULL;

            if (GetOpenFileName(&re))
            {

            }
        }
        break;

        case IDC_BUTTON5:
        {
            ZeroMemory(&cr, sizeof(re));
            re.lStructSize = sizeof(OPENFILENAME);
            re.hwndOwner = dlg;
            re.hInstance = hInst;
            re.nMaxCustFilter = 256;
            re.lpstrFile = buf;
            re.lpstrFileTitle = NULL;
            re.nMaxFile = 255;
            re.nMaxFileTitle = 255;
            re.lpstrInitialDir = NULL;
            re.lpstrTitle = 0;
            re.Flags = OFN_FILEMUSTEXIST;
            re.nFileOffset = 0;
            re.nFileExtension = 0;
            re.lpstrDefExt = NULL;
            re.lCustData = NULL;
            re.lpfnHook = NULL;
            re.lpTemplateName = NULL;
            if (GetOpenFileName(&re) == TRUE)
            {
                SYSTEMTIME time;
                SYSTEMTIME time2;
                SYSTEMTIME time3;
                WIN32_FIND_DATAW wfd;
                HANDLE const hFind = FindFirstFileW(buf, &wfd);

                if (INVALID_HANDLE_VALUE != hFind)
                {
                    do
                    {
                        FileTimeToSystemTime(&wfd.ftCreationTime, &time);

                    } while (NULL != FindNextFileW(hFind, &wfd));

                    FindClose(hFind);
                }
                FileTimeToSystemTime(&wfd.ftLastWriteTime, &time2);
                FileTimeToSystemTime(&wfd.ftLastAccessTime, &time3);

                TCHAR bufx[256] = { 0 };
                _stprintf_s(bufx, L"Имя файла: %s\nДата создания: %u.%u.%u\nАтрибуты файла: %i\nРазмер файла: %u байт\nДата последнего изменения: %u.%u.%u в %u:%u\nДата последнего обращения к файлу: %u.%u.%u в %u:%u", &wfd.cFileName, time.wDay, time.wMonth, time.wYear, wfd.dwFileAttributes, wfd.nFileSizeLow, time2.wDay, time2.wMonth, time2.wYear, time2.wHour, time2.wMinute, time3.wDay, time3.wMonth, time3.wYear, time3.wHour, time3.wMinute);
                MessageBox(0, bufx, L"MessageBox caption", MB_OK);
            }
        }
        break;

        case IDM_ABOUT:
        {
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
        }
        break;

        case IDM_EXIT:
        {
            DestroyWindow(hWnd);
        }
        break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
    break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
