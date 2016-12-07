/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: animalfarm.cpp - An application that will create a window with
-- modifiable settings allowing us to connect to a comm port and send and receive
-- text files using the Animalfarm itteration of the Stall'in protocall.
--
-- PROGRAM: animalfarm.cpp
--
-- FUNCTIONS:
-- int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
--                     _In_opt_ HINSTANCE hPrevInstance,
--                   _In_ LPWSTR    lpCmdLine,
--                   _In_ int       nCmdShow);
-- ATOM MyRegisterClass(HINSTANCE hInstance);
-- BOOL InitInstance(HINSTANCE, int);
-- LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
-- INT_PTR CALLBACK MainDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
-- INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
-- BOOL Connect(HWND& hWnd, LPCWSTR commName);
--
--
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- NOTES:
-- An application that will create a window with
-- modifiable settings allowing us to connect to a comm port and send and receive
-- text files using the Animalfarm itteration of the Stall'in protocall.
----------------------------------------------------------------------------------------------------------------------*/
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



/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- ATOM MyRegisterClass(HINSTANCE hInstance)
--
-- RETURNS: ATOM.
--
-- NOTES:
-- This function registers the class.
----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
--
-- RETURNS: BOOL.
--
-- NOTES:
-- This function inits an  instance of the program.
----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
--
-- RETURNS: LRESULT.
--
-- NOTES:
-- This function handles events from the user.
----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- INT_PTR CALLBACK MainDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
--
-- RETURNS: INT_PTR.
--
-- NOTES:
-- This function handles the main dialog.
----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
--
-- RETURNS: INT_PTR.
--
-- NOTES:
-- This function handles the about box.
----------------------------------------------------------------------------------------------------------------------*/
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


/*------------------------------------------------------------------------------------------------------------------
-- DATE: December 06, 2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Maitiu Morton, Yao Shu, Deric Mccadden, Jamie Lee
--
-- PROGRAMMER: Jamie Lee, Deric Mccadden, Maitiu Morton, Yao Shu,
--
-- BOOL Connect(HWND& hWnd, LPCWSTR commName) 
--
-- RETURNS: BOOL.
--
-- NOTES:
-- This function checks if the comm is connected.
----------------------------------------------------------------------------------------------------------------------*/
BOOL Connect(HWND& hWnd, LPCWSTR commName) {
	if (commName == NULL) {
		MessageBox(NULL, L"Select a Com under the Config Menu", L"", MB_OK);
		return FALSE;
	}

	return true;
}
