#include "MainWindow.h"
#include "resource.h"


HWND MainWindow::Create()
{
    hWnd = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, StaticProc, (LPARAM) this);

    if (NULL != hWnd) {
        HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
        HICON hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));
        if (hIcon || hIcon2) {
            if (NULL == hIcon) hIcon = hIcon2;
            if (NULL == hIcon2) hIcon2 = hIcon;
            SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon2);
        }
    }
    return hWnd;
}

INT_PTR MainWindow::StaticProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    MainWindow* wnd;
    if (msg == WM_INITDIALOG) {
        SetWindowLongPtr(hWnd, DWLP_USER, lp);
        wnd = (MainWindow*)lp;
    }
    else {
        wnd = (MainWindow*)GetWindowLongPtr(hWnd, DWLP_USER);
    }
    return wnd->Proc(hWnd, msg, wp, lp);
}

INT_PTR MainWindow::Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case ID_FILE_EXIT:
            DestroyWindow(hWnd);
            return TRUE;
        case ID_HELP_ABOUT:
            about.Exec(hWnd);
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
