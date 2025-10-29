#include <Windows.h>
#include "resource.h"
#include "Args.h"


INT_PTR AboutProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM) {
    switch (msg) {
    case WM_INITDIALOG:
        break;
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDOK:
        case IDCANCEL:
            EndDialog(hwnd, LOWORD(wp));
            break;
        default:
            return FALSE;
        }
    default:
        return FALSE;
    }
    return TRUE;
}

INT_PTR MainProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case ID_FILE_EXIT:
            DestroyWindow(hWnd);
            return TRUE;
        case ID_HELP_ABOUT:
            DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutProc);
            return TRUE;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return TRUE;
    case WM_NCDESTROY:
        PostQuitMessage(0);
        return TRUE;
    }
    return FALSE;
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInst,
    _In_opt_ HINSTANCE /* hPrevInstance */,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
) {
    Args args;
    try {
        args = Args (lpCmdLine);
    }
    catch (ArgError& err) {
        MessageBoxW(NULL, err.wcsWhat(), NULL, MB_OK | MB_ICONERROR);
        return 1;
    }
    HWND hWnd = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, MainProc, 0);
    if (NULL == hWnd) {
    //if (TRUE) {
        LPWSTR msg;
        ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
            NULL, GetLastError(), 0, (LPWSTR)&msg, 0, NULL);
        ArgError err(args, IDS_CREATE_WINDOW, msg);
        MessageBoxW(NULL, err.wcsWhat(), NULL, MB_OK | MB_ICONERROR);
        return 1;
    }

    HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
    HICON hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));
    if (hIcon || hIcon2) {
        if (NULL == hIcon) hIcon = hIcon2;
        if (NULL == hIcon2) hIcon2 = hIcon;
        SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon2);
    }


    ShowWindow(hWnd, nShowCmd);

    HACCEL hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));

    // Application message loop
    MSG msg;
    BOOL cr;
    while ((cr = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (cr == -1) {
            LPWSTR msg;
            ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                NULL, GetLastError(), 0, (LPWSTR)&msg, 0, NULL);
            ArgError err(args, IDS_GETMESSAGE, msg);
            MessageBoxW(NULL, err.wcsWhat(), NULL, MB_OK | MB_ICONERROR);
            return 1;
        }
        if (TranslateAccelerator(hWnd, hAccel, &msg)) {
            continue;
        }
        if (IsDialogMessage(hWnd, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    DestroyAcceleratorTable(hAccel);

    return (int) msg.wParam;
}