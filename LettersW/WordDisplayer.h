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


public:
	WordDisplayer(UINT id, unsigned char sizing, unsigned char moving);
	~WordDisplayer() {
		unregister();
		if (hFontDel) DeleteObject(hFont);
	}
	void onInit(HWND parent, LPRECT parentRect);
	void onVSize(DWORD oldH, DWORD newH);
	void display1(std::vector < std::vector<WCHAR>>&& words) {
		wordlist = std::vector<std::vector<std::vector<WCHAR>>>{ words };
		InvalidateRect(hwnd, NULL, TRUE);
	}
	void displayN(std::vector<std::vector<std::vector<WCHAR>>>&& list) {
		wordlist = list;
		InvalidateRect(hwnd, NULL, TRUE);
	}
};

