#pragma once
#include <Windows.h>
#include "Args.h"
#include "AboutDlg.h"
#include "resource.h"
#include "Control.h"
#include "Dico.h"
#include <tchar.h>

class MainWindow
{
	Args& args;
	HINSTANCE hInst;
	HWND hWnd = NULL;
	AboutDlg about;

	Dico dico;

	LONG minWidth, minHeight;
	LONG x, y, width, height;

	LONG oldWidth;

	bool inUpdateLetters = false;

	void Clear(DWORD id) {
		SetDlgItemText(hWnd, id, _T(""));
		SetFocus(GetDlgItem(hWnd, id));
	}

	void Valid(BOOL enable);
	void UpdateLetters();

	Control lettersGroup = Control(IDC_GROUP_LETTERS, 255, 0 ), searchGroup = Control(IDC_GROUP_SEARCH, 255, 0);
	Edit lettersEdit{ IDC_EDIT_LETTERS, 255, 0 }, searchEdit{ IDC_EDIT_SEARCH, 255, 0 };
	Control lettersClear{ IDC_CLEAR_LETTERS, 0, 255 },
		searchClear{ IDC_CLEAR_SEARCH, 0,255 },
		lettersValid{ IDC_VALID_LETTERS,0,255 }, searchValid{ IDC_VALID_SEARCH,0,255 };

	Control* children[8] = {
		&lettersGroup, &lettersEdit, &lettersClear, &lettersValid,
		&searchGroup, &searchEdit, &searchClear, &searchValid
	};

	INT ctrlId = 0;

	std::vector<WCHAR> letters;
	std::vector<WCHAR> mask;

	static INT_PTR StaticProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	INT_PTR Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	INT_PTR OnSizing(WPARAM, LPARAM);
	INT_PTR OnInitDialog(WPARAM, LPARAM);
	INT_PTR OnSize(WPARAM, LPARAM);
	INT_PTR OnMove(WPARAM, LPARAM);
	INT_PTR OnWindowPosChanged(WINDOWPOS* wp);

	void initEditMenu(bool inEdit);
	bool editCopy();
	void editCut();
	void editPaste();


public:
	struct Dims {
		LONG x, y, w, h;
	};
	MainWindow(Args& args, HINSTANCE hInst) : args(args), hInst(hInst), about(args, hInst), dico(hInst) {}

	HWND Create();
	Dims getDims() const {
		return Dims{ x, y, width, height };
	}
};

