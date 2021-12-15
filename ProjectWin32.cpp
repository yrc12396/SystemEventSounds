#ifndef UNICODE
#define UNICODE
#endif 

#include<Windows.h>
#include<mmsystem.h>
#include<WinUser.h>
#include<WtsApi32.h>
#include<fstream>
#pragma comment( lib, "Winmm.lib" )
#pragma comment( lib, "WtsApi32.lib" )
#pragma comment( lib, "User32.lib" )

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    std::fstream myfile("CloseStatus.txt", std::ios_base::in);
    int sts;
    myfile >> sts;
    if (sts == 1)//indicates user logon.
    {
        PlaySound(TEXT("WindowsLogon"), NULL, SND_ALIAS | SND_SYSTEM);
    }
    else
    {
        PlaySound(TEXT("SystemStart"), NULL, SND_ALIAS | SND_SYSTEM);
    }
    myfile.close();
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"System Sound Player",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, SW_HIDE);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL x;
    //wchar_t msg;
    switch (uMsg)
    {
    case WM_DESTROY:
        ShutdownBlockReasonDestroy(hwnd);
        WTSUnRegisterSessionNotification(hwnd);
        PostQuitMessage(0);
        break;

    case WM_CREATE:
        ShutdownBlockReasonCreate(hwnd, L"Playing System Sound");
        WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_THIS_SESSION);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);



        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_QUERYENDSESSION:
    {
        WTSUnRegisterSessionNotification(hwnd);
        std::fstream myfile("CloseStatus.txt", std::ios_base::out);
        switch (lParam)
        {
        case ENDSESSION_LOGOFF:
            x = PlaySound(TEXT("WindowsLogoff"), NULL, SND_ALIAS | SND_SYSTEM);
            //msg = "Logoff";
            myfile << 1;
            myfile.close();
            if (x)
            {
                ShutdownBlockReasonDestroy(hwnd);
                return TRUE;
            }
            break;
        case 0:
            x = PlaySound(TEXT("SystemExit"), NULL, SND_ALIAS | SND_SYSTEM);
            //msg = "Shutdown";
            myfile << 0;
            myfile.close();
            if (x)
            {
                ShutdownBlockReasonDestroy(hwnd);
                return TRUE;
            }
            break;
        default:
            break;
        }
        return FALSE;
        break;
    }

    }
    switch (wParam)
    {
    case WTS_SESSION_LOCK:
        PlaySound(TEXT("WindowsLogoff"), NULL, SND_ALIAS | SND_SYSTEM);
        break;
    case WTS_SESSION_UNLOCK:
        PlaySound(TEXT("WindowsUnlock"), NULL, SND_ALIAS | SND_SYSTEM);
        break;
    default:
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}