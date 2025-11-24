#pragma once
#include "Control.h"
#include <Windows.h>
#include <vector>
#include <tchar.h>

class WordDisplayer: public Control
{
	unsigned short singleWidth, singleHeight, addWidth, addHeight, spaceWidth;

	std::vector<std::vector<std::vector<WCHAR>>> wordlist;


	HFONT hFont;
	bool hFontDel = false;

	LONG hNoScroll;

	ATOM registerClass();
	void unregister();

	static LRESULT staticProc(
		HWND   hWnd,
		UINT   Msg,
		WPARAM wParam,
		LPARAM lParam
	);

	LRESULT Proc(UINT msg, WPARAM wp, LPARAM lp);
	void paint(HDC hDC);
	BOOL doCopy();

	bool inDrag = false;
	POINT start{ -1 }, end;
	POINT* first, * last;
	int firstSel = -1, lastSel = -1;

	HWND parent;

public:
	WordDisplayer(UINT id, unsigned char sizing, unsigned char moving);
	~WordDisplayer() {
		unregister();
		if (hFontDel) DeleteObject(hFont);
	}
	void onInit(HWND parent, LPRECT parentRect);
	int onVSize(DWORD oldH, DWORD newH, DWORD yScroll);
	void display1(std::vector < std::vector<WCHAR>>&& words) {
		wordlist = std::vector<std::vector<std::vector<WCHAR>>>{ words };
		InvalidateRect(hwnd, NULL, TRUE);
	}
	void displayN(std::vector<std::vector<std::vector<WCHAR>>>&& list) {
		wordlist = list;
		InvalidateRect(hwnd, NULL, TRUE);
	}
	int line() const {
		return addHeight;
	}
	void drawSelRect(HDC hDC) {
		SetROP2(hDC, R2_NOT);
		MoveToEx(hDC, start.x, start.y, NULL);
		LineTo(hDC, end.x, start.y);
		LineTo(hDC, end.x, end.y);
		LineTo(hDC, start.x, end.y);
		LineTo(hDC, start.x, start.y);
	}
	bool hasSel() const {
		return (firstSel != -1 && firstSel != lastSel);
	}
};

