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
    LPVOID old = SelectObject(hDC, hFont);
    WCHAR fontName[256];
    GetTextFaceW(hDC, sizeof(fontName) / sizeof(*fontName), fontName);

    int deltaH = addHeight;
    int y = 0, x = 0;
    for (const auto& words : wordlist) {
        if (!words.empty()) {
            int len = words[0].size();
            int deltaW = singleWidth + (len -1) * addWidth + spaceWidth;
            int count = (width + spaceWidth) / (deltaW + spaceWidth);
            int j = 0;
            x = 0;
            for (const auto& word : words) {
                TextOut(hDC, x, y, word.data(), word.size());
                if (++j == count) {
                    j = 0;
                    x = 0;
                    y += deltaH;
                }
                else x += deltaW;
            }
        }
        if (x == 0) y += deltaH / 2;
        else y += deltaH * 3 / 2;
    }
    SelectObject(hDC, old);
}


WordDisplayer::WordDisplayer(UINT id, unsigned char sizing, unsigned char moving)
    : Control(id, sizing, moving)
{
    registerClass();
    NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
    ncm.lfCaptionFont.lfWeight = 500;
    hFont = CreateFontIndirectW(&ncm.lfCaptionFont);
    if (NULL == hFont) hFont = (HFONT)GetStockObject(SYSTEM_FONT);
    else hFontDel = true;
}

void WordDisplayer::onInit(HWND parent, LPRECT parentRect) {
    Control::onInit(parent, parentRect);
    hNoScroll = height;
    hwnd = GetDlgItem(parent, id);
    SetWindowLongPtr(hwnd, 0, (LONG_PTR)this);

    HDC dc = GetDC(hwnd);
    auto old = SelectObject(dc, hFont);

    TEXTMETRIC metric;
    GetTextMetrics(dc, &metric);
    SIZE sz;
    GetTextExtentPoint32(dc, _T("W"), 1, &sz);
    singleWidth = sz.cx;
    singleHeight = sz.cy;
    GetTextExtentPoint32(dc, _T("WW"), 2, &sz);
    addWidth = sz.cx - singleWidth;
    addHeight = sz.cy + metric.tmExternalLeading;
    GetTextExtentPoint32(dc, _T("W W"), 3, &sz);
    spaceWidth = sz.cx - 2 * singleWidth;
    WCHAR fontName[256];
    GetTextFaceW(dc, sizeof(fontName) / sizeof(*fontName), fontName);
    SelectObject(dc, old);
    ReleaseDC(hwnd, dc);

}

void WordDisplayer::onVSize(DWORD oldH, DWORD newH, DWORD yScroll) {
    int delta = newH - oldH;
    hNoScroll += delta;
    height = hNoScroll + yScroll;
}
