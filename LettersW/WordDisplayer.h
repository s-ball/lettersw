#pragma once
#include "Control.h"
#include <Windows.h>
#include <vector>
#include <tchar.h>

class WordDisplayer: public Control
{

	std::vector<std::vector<std::vector<WCHAR>>> wordlist;

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
	}
	void onInit(HWND parent, LPRECT parentRect);
	void onVSize(DWORD oldH, DWORD newH);
};

