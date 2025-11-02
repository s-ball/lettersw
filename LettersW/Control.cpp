#include "Control.h"

void Control::onSize(DWORD oldW, DWORD newW) {
	if (oldW == newW) return;
	int delta = newW - oldW;
	x += delta * moving / 255;
	width += delta * sizing / 255;
	MoveWindow(hwnd, x, y, width, height, TRUE);
}

void Control::onInit(HWND parent, LPRECT parentRect) {
	hwnd = GetDlgItem(parent, id);
	RECT rect;
	GetWindowRect(hwnd, &rect);
	x = rect.left - parentRect->left;
	y = rect.top - parentRect->top;
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}
