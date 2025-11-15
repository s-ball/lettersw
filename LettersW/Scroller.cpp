#include "Scroller.h"
#include <crtdbg.h>

void Scroller::setMaxPos(int pos) {
    if (pos == si.nMax) return;
    _RPT2(_CRT_WARN, "nMax=%d - newPos=%d\n", si.nMax, pos);

    if ((pos * si.nMax) == 0) {
        setScrollbar(pos);
	}
    si.nMax = pos;
    if (si.nPos > pos) {
        si.nPos = pos;
    }
    line = parent.lineSize();
    page = parent.pageSize();
    adjustPos(si.nPos);
}

bool Scroller::scrollMsg(WPARAM wp)
{
    _RPT2(_CRT_WARN, "scroll msg %d - %d\n", LOWORD(wp), HIWORD(wp));
    int pos = si.nPos;
    switch (LOWORD(wp)) {
    case SB_TOP:
        pos = 0;
        break;
    case SB_BOTTOM:
        pos = si.nMax;
        break;
    case SB_LINEDOWN:
        pos = si.nPos + line;
        if (pos > si.nMax) pos = si.nMax;
        break;
    case SB_LINEUP:
        pos = si.nPos - line;
        if (pos < 0) pos = 0;
        break;
    case SB_PAGEDOWN:
        pos = si.nPos + page;
        if (pos > si.nMax) pos = si.nMax;
        break;
    case SB_PAGEUP:
        pos = si.nPos - page;
        if (pos < 0) pos = 0;
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        pos = HIWORD(wp);
    case SB_ENDSCROLL:
        adjustPos(pos);
        return true;
    default:
        return false;
    }
    si.nPos = pos;
    return true;
}

void Scroller::setScrollbar(bool set) {
    _RPT1(_CRT_WARN, "set scrollbar %d\n", set);
    LONG_PTR style = GetWindowLongPtr(parent.getHwnd(), GWL_STYLE);
    if (set) style |= WS_VSCROLL;
    else style &= ~WS_VSCROLL;
    SetWindowLongPtrW(parent.getHwnd(), GWL_STYLE, style);
}

void Scroller::adjustPos(UINT newPos) {
    _RPT2(_CRT_WARN, "scroll %d -> %d\n", si.nPos, newPos);
    si.nPos = newPos;
    if (si.nMax != 0) SetScrollInfo(parent.getHwnd(), SB_VERT, &si, TRUE);
    PostMessage(parent.getHwnd(), parent.APPM_SCROLL, newPos, 0);
}

