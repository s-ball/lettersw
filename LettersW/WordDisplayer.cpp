#include "WordDisplayer.h"
#include <crtdbg.h>

ATOM WordDisplayer::registerClass()
{
    WNDCLASS wndClass = {
        CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
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
    case WM_SETFOCUS:
        SendMessage(parent, WM_COMMAND, (EN_SETFOCUS << 16) | id, (LPARAM) hwnd);
        break;
    case WM_COPY:
        return doCopy();
    case WM_LBUTTONDOWN: {
            POINT pt{ LOWORD(lp), HIWORD(lp) };

            SetFocus(hwnd);
            if (start.x != -1) {
                HDC hDC = GetDC(hwnd);
                drawSelRect(hDC);
                ReleaseDC(hwnd, hDC);
            }
            if (DragDetect(hwnd, pt)) {
                inDrag = true;
                end = start = pt;
                firstSel = lastSel = -1;
                RECT rect;
                GetClientRect(hwnd, &rect);
                pt = POINT{ 0, 0 };
                ClientToScreen(hwnd, &pt);
                OffsetRect(&rect, pt.x + 1, pt.y + 1);
                rect.right -= 2;
                rect.bottom -= 2;
                ClipCursor(&rect);
            }
            else {
                inDrag = false;
                if (start.x != -1) {
                    start.x = -1;
                    firstSel = lastSel = -1;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
        }
        break;
    case WM_LBUTTONUP:
        inDrag = false;
        ClipCursor(NULL);
        InvalidateRect(hwnd, NULL, TRUE);
        if (start.y <= end.y) {
            first = &start;
            last = &end;
        }
        else {
            first = &end;
            last = &start;
        }
        break;
    case WM_MOUSEMOVE:
        if (inDrag) {
            HDC hDC = GetDC(hwnd);
            if (start.x != end.x || start.y != end.y) {
                drawSelRect(hDC);
            }
            end = POINT{ LOWORD(lp), HIWORD(lp) };
            _RPT4(_CRT_WARN, "Select from %d-%d to %d-%d\n",
                start.x, start.y, end.x, end.y);
            drawSelRect(hDC);
            ReleaseDC(hwnd, hDC);
        }
        break;
    case WM_LBUTTONDBLCLK:
        start = end = POINT{ LOWORD(lp), HIWORD(lp) };
        first = &start;
        last = &end;
        firstSel = lastSel = -1;
        break;
    case WM_GETDLGCODE:
        _RPT2(_CRT_WARN, "GETDLGCODE %x - %x\n", wp, lp);
        return DLGC_WANTCHARS;
    case WM_CHAR:
        if (wp == 0x03) return doCopy();
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

void WordDisplayer::paint(HDC hDC)
{
    LPVOID old = SelectObject(hDC, hFont);

    int deltaH = addHeight;
    int y = 0, x = 0, index = 0;
    for (const auto& words : wordlist) {
        if (!words.empty()) {
            int len = (int) words[0].size();
            int deltaW = singleWidth + (len -1) * addWidth + spaceWidth;
            int count = (width + spaceWidth) / deltaW;
            if (count == 0) count = 1;
            int j = 0;
            x = 0;
            for (const auto& word : words) {
                if (start.x != -1 && firstSel == -1) {
                    if (y + deltaH > first->y) {
                        if (x + deltaW > first->x) {
                            firstSel = index;
                        }
                        else if (y + deltaH > last->y && x + deltaW > last->x) {
                            std::swap(first, last);
                            firstSel = index;
                        }
                    }
               }
                if (firstSel != -1 && lastSel == -1) {
                    if (y > last->y || (y + deltaH > last->y && x > last->x)) {
                        lastSel = index;
                    }
                }
                if ((firstSel != -1 && index >= firstSel) && (lastSel == -1 || index < lastSel)) {
                    SetBkColor(hDC, GetSysColor(COLOR_HIGHLIGHT));
                    SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
                }
                else {
                    SetBkColor(hDC, GetDCBrushColor(hDC));
                    SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
                }
                TextOut(hDC, x, y, word.data(), (int) word.size());
                index += 1;
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
    if (firstSel != -1 && lastSel == -1) lastSel = index;
    /*if (start.x != -1) {
        drawSelRect(hDC);
    }*/
    _RPT1(_CRT_WARN, "required height: %d\n", y + singleHeight);
    SelectObject(hDC, old);
}

BOOL WordDisplayer::doCopy()
{
    _RPT0(_CRT_WARN, "Should copy to clipboard\n");
    return 0;
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
    this->parent = parent;
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

int WordDisplayer::onVSize(DWORD oldH, DWORD newH, DWORD yScroll) {
    int delta = newH - oldH;
    if (delta != 0) _RPT2(_CRT_WARN, "height: %d -> %d\n", oldH, newH);
    hNoScroll += delta;
    height = hNoScroll + yScroll;


    int total = -height;
    for (const auto& words : wordlist) {
        if (!words.empty()) {
            int len = (int) words[0].size();
            int deltaW = singleWidth + (len - 1) * addWidth + spaceWidth;
            int count = (width + spaceWidth) / deltaW;
            if (count == 0) count = 1;
            int addLines = ((int) words.size() - 1) / count;
            total += singleHeight + addLines * addHeight;
        }
    }
    if (!wordlist.empty()) {
        total += (addHeight / 2) * (wordlist.size() - 1);
    }
    MoveWindow(hwnd, x, y - yScroll, width, height, TRUE);
    _RPT1(_CRT_WARN, "vSize: %d\n", total + height);
    return (total > 0) ? total : 0;
}
