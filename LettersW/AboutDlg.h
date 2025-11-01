#pragma once
#include "Args.h"

class AboutDlg
{
	Args& args;
	HINSTANCE hInst;
	LPCWSTR version = NULL;
	HWND parent = NULL;

	static INT_PTR StaticProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	INT_PTR Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	LPCWSTR LoadVersionString();

public:
	AboutDlg(Args& args, HINSTANCE hInst) : args(args), hInst(hInst) {}
	void Exec(HWND hwnd);
	LPCWSTR getVersion();
};

