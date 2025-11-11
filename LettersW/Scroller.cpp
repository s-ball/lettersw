#include "Scroller.h"

void Scroller::setMaxPos(UINT pos) {
	if (pos == si.nMax) return;

	if ((pos * si.nMax) == 0) {
		setScrollbar(pos);
	}
	si.nMax = pos;
    if (si.nPos > pos) {
        si.nPos = pos;
    }
    adjustPos(si.nPos);
}

bool Scroller::scrollMsg(UINT msg, WPARAM wp)
{
    return false;
}

void Scroller::setScrollbar(bool set) {
    HWND hWnd = parent.getHwnd();
    LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
    if (set) style |= WS_VSCROLL;
    else style &= ~WS_VSCROLL;
    RECT rect;
    GetClientRect(hWnd, &rect);
    UINT old = rect.right;
    SetWindowLongPtrW(hWnd, GWL_STYLE, style);
    SetWindowPos(hWnd, 0, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    GetClientRect(hWnd, &rect);
    UINT w = rect.right;
    parent.adjustClient(old, w, 0);
    si.nPos = 0;
}

void Scroller::adjustPos(UINT newPos) {
    HWND hwnd = parent.getHwnd();
    si.nPos = newPos;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
    parent.adjustClient(0, 0, newPos);
}

