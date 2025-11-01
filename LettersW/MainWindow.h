#pragma once
#include <Windows.h>
#include "Args.h"
#include "AboutDlg.h"

class MainWindow
{
	Args& args;
	HINSTANCE hInst;
	HWND hWnd = NULL;
	AboutDlg about;

	static INT_PTR StaticProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	INT_PTR Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

public:
	MainWindow(Args& args, HINSTANCE hInst) : args(args), hInst(hInst), about(args, hInst) {}

	HWND Create();
};

