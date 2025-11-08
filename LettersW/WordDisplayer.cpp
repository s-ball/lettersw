#include "WordDisplayer.h"

ATOM WordDisplayer::registerClass()
{
    WNDCLASS wndClass = {
        CS_HREDRAW | CS_VREDRAW,
        staticProc,
        0,
        sizeof(WordDisplayer*),
        0,
        NULL,
        LoadCursor(nullptr, IDC_ARROW),
        (HBRUSH)(1 + COLOR_WINDOW),
        NULL,
        _T("WORDDISPLAY"),
    };
    return RegisterClass(&wndClass);
}

void WordDisplayer::unregister()
{
}

LRESULT WordDisplayer::staticProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    WordDisplayer * wd = (WordDisplayer *) GetWindowLongPtr(hWnd, 0);
    return wd ? wd->Proc(Msg, wParam, lParam) : DefWindowProc(hWnd, Msg, wParam, lParam);
}

LRESULT WordDisplayer::Proc(UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            // All painting occurs here, between BeginPaint and EndPaint.
            paint(hdc);
            EndPaint(hwnd, &ps);
        }
        break;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

void WordDisplayer::paint(HDC hDC)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    DrawText(hDC, _T("Foo"), -1, &rect, DT_LEFT | DT_TOP);
}

WordDisplayer::WordDisplayer(UINT id, unsigned char sizing, unsigned char moving)
    : Control(id, sizing, moving)
{
    registerClass();
}

void WordDisplayer::onInit(HWND parent, LPRECT parentRect) {
    Control::onInit(parent, parentRect);
    hwnd = GetDlgItem(parent, id);
    SetWindowLongPtr(hwnd, 0, (LONG_PTR)this);
}

void WordDisplayer::onVSize(DWORD oldH, DWORD newH) {
    if (oldH == newH) return;
    int delta = newH - oldH;
    height += delta;
}
