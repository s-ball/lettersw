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
    case WM_INITDIALOG:
        this->hWnd = hWnd;
        return OnInitDialog(wp, lp);
    case WM_SIZING:
        return OnSizing(wp, lp);
        break;
    case WM_SIZE:
        return OnSize(wp, lp);
        break;
    case WM_MOVE:
        return OnMove(wp, lp);
        break;
    case WM_WINDOWPOSCHANGED:
        return OnWindowPosChanged((WINDOWPOS*)lp);
        break;
    }
    return FALSE;
}

INT_PTR MainWindow::OnSizing(WPARAM wp, LPARAM lp) {
    RECT* rect = (RECT*)lp;
    if (rect->bottom - rect->top < minHeight) {
        switch (wp) {
        case WMSZ_BOTTOM:
        case WMSZ_BOTTOMLEFT:
        case WMSZ_BOTTOMRIGHT:
            rect->bottom = rect->top + minHeight;
            break;
        default:
            rect->top = rect->bottom - minHeight;
        }
    }
    if (rect->right - rect->left < minWidth) {
        switch (wp) {
        case WMSZ_LEFT:
        case WMSZ_BOTTOMLEFT:
        case WMSZ_TOPLEFT:
            rect->left = rect->right - minWidth;
            break;
        default:
            rect->right = rect->left + minWidth;
        }
    }
    return TRUE;
}

INT_PTR MainWindow::OnInitDialog(WPARAM, LPARAM) {
    RECT rect;
    GetWindowRect(GetDlgItem(hWnd, IDC_TOPLEFT), &rect);
    DWORD top = rect.top, left = rect.left;
    GetWindowRect(hWnd, &rect);
    height = minHeight = rect.bottom - rect.top;
    oldWidth = width = minWidth = rect.right - rect.left;
    GetClientRect(hWnd, &rect);
    rect.top += top;
    rect.bottom += top;
    rect.left += left;
    rect.right += left;
    for (auto child : children) {
        child->onInit(hWnd, &rect);
    }
    return TRUE;
}

INT_PTR MainWindow::OnSize(WPARAM wp, LPARAM lp) {
    if (wp == SIZE_RESTORED) {
        width = LOWORD(lp);
        height = HIWORD(lp);
    }
    for (auto child : children) {
        child->onSize(oldWidth, LOWORD(lp));
    }
    oldWidth = LOWORD(lp);
    return TRUE;
}

INT_PTR MainWindow::OnMove(WPARAM, LPARAM lp)
{
    x = LOWORD(lp);
    y = HIWORD(lp);
    return TRUE;
}

INT_PTR MainWindow::OnWindowPosChanged(WINDOWPOS* wp) {
    x = wp->x;
    y = wp->y;
    width = wp->cx;
    height = wp->cy;
    for (auto child : children) {
        child->onSize(oldWidth, wp->cx);
    }
    oldWidth = wp->cx;
    return TRUE;
}
