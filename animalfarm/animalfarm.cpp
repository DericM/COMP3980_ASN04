// animalfarm.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "globalvar.h"
#include "animalfarm.h"
#include "tx_wait_connect.h"
#include "idle.h"
#include "tx_get_data.h"
#include "session.h"




#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
COMMCONFIG	cc;
BOOL connected;
HANDLE comHandle;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	MainDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL                Connect(HWND& hWnd, LPCWSTR commName);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ANIMALFARM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ANIMALFARM));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANIMALFARM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ANIMALFARM);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

  GlobalVar::g_hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 515, 380, nullptr, nullptr, hInstance, nullptr);

   if (!GlobalVar::g_hWnd)
   {
      return FALSE;
   }

   GlobalVar::g_hMainDlg = CreateDialogW(hInst, MAKEINTRESOURCE(IDD_MAIN_DLG), GlobalVar::g_hWnd, MainDlg);
   ShowWindow(GlobalVar::g_hMainDlg, SW_SHOW);
   ShowWindow(GlobalVar::g_hWnd, nCmdShow);
   UpdateWindow(GlobalVar::g_hWnd);

   GlobalVar::g_hSendBox = GetDlgItem(GlobalVar::g_hMainDlg, IDC_EDIT2);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPCWSTR cName = NULL;
	comHandle = NULL;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_CONFIG_COM1:
				cName = L"Com1";
				idle_setup(cName);
				break;
			case ID_CONFIG_COM2:
				cName = L"Com2";
				idle_setup(cName);
				break;
			case ID_CONFIG_COM3:
				cName = L"Com3";
				idle_setup(cName);
				break;
			case ID_CONFIG_COM4:
				cName = L"Com4";
				idle_setup(cName);
				break;
			case ID_CONFIG_COM5:
				cName = L"Com5";
				idle_setup(cName);
				break;
			case ID_CONFIG_COM6:
				cName = L"Com6";
				idle_setup(cName);
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
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
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		is_close_port();
		idle_terminate_thread();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

	//*****if Connect Button is clicked******
	//idle_setup(hWnd, cName);
	//Connect(hWnd, cName);

    return 0;
}

// Message handler for main dlg box.
INT_PTR CALLBACK MainDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			SendMessage(GetParent(hDlg), WM_DESTROY, 0, 0);
			//PostQuitMessage(0);
			return (INT_PTR)TRUE;
		}
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDC_CONNECT:
			idle_connect();
			break;
		case IDC_SENDFILE:
			wchar_t filenameBuff[128];
			GetDlgItemTextW(hDlg, IDC_EDIT_SENDFILE, filenameBuff, 128);
			LPCWSTR filename(filenameBuff);
			GlobalVar::g_bWannaSendFile = true;
			//idle_go_to_sendfile(filename);
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for about box.
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

BOOL Connect(HWND& hWnd, LPCWSTR commName) {
	if (commName == NULL) {
		MessageBox(NULL, L"Select a Com under the Config Menu", L"", MB_OK);
		return FALSE;
	}

	return true;
}
