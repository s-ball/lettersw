#pragma once
#include <Windows.h>
#include "Args.h"
#include "AboutDlg.h"
#include "resource.h"
#include "Control.h"
#include "Dico.h"
#include "WordDisplayer.h"
#include "Scroller.h"
#include <tchar.h>

class MainWindow: public ScrollOwner
{
	static const int APPM_REDRAW = WM_APP + 2;

	Args& args;
	HINSTANCE hInst;
	HWND hWnd = NULL;
	AboutDlg about;

	Dico dico;

	LONG minWidth, minHeight;
	LONG x, y, width, height;

	LONG yScroll;

	LONG dimWidth, dimHeight;

	bool inUpdateLetters = false;

	void Clear(DWORD id) {
		SetDlgItemText(hWnd, id, _T(""));
		SetFocus(GetDlgItem(hWnd, id));
	}

	void Valid(BOOL enable);
	void UpdateLetters(int id, bool letters);
	void FilterSearch(std::vector<WCHAR>& str);
	void Search();


	Control lettersGroup = Control(IDC_GROUP_LETTERS, 255, 0 ), searchGroup = Control(IDC_GROUP_SEARCH, 255, 0);
	Edit lettersEdit{ IDC_EDIT_LETTERS, 255, 0 }, searchEdit{ IDC_EDIT_SEARCH, 255, 0 };
	Control lettersClear{ IDC_CLEAR_LETTERS, 0, 255 },
		searchClear{ IDC_CLEAR_SEARCH, 0,255 },
		lettersValid{ IDC_VALID_LETTERS,0,255 }, searchValid{ IDC_VALID_SEARCH,0,255 };
	WordDisplayer wd{ IDC_WORDS, 255, 0 };

	Control* children[9] = {
		&lettersGroup, &lettersEdit, &lettersClear, &lettersValid,
		&searchGroup, &searchEdit, &searchClear, &searchValid, &wd
	};

	
	INT ctrlId = 0;

	std::vector<WCHAR> letters;
	std::vector<WCHAR> mask;

	static INT_PTR StaticProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	INT_PTR Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	INT_PTR OnSizing(WPARAM, LPARAM);
	INT_PTR OnInitDialog(WPARAM, LPARAM);
	INT_PTR OnWindowPosChanged(WINDOWPOS* wp);
	INT_PTR OnRedraw();

	void initEditMenu(bool inEdit);
	bool editCopy();
	void editCut();
	void editPaste();

	Scroller scroller;
	void redraw();
	void adjustScroll(int scroll);

	HWND getHwnd() const {
		return hWnd;
	}
	int lineSize() const {
		static int line = 0;

		if (line == 0) {
			line = wd.line();
			if (line == 0) return 10;
		}
		return line;
	}
	int pageSize() const {
		int page = height / 3;
		if (page < 4 * lineSize()) {
			page = 4 * lineSize();
		}
		return page;
	}

	void EnsureVisibleEdit(bool letters, bool focus);


public:
	struct Dims {
		LONG x, y, w, h;
	};
	MainWindow(Args& args, HINSTANCE hInst) : args(args), hInst(hInst),
		about(args, hInst), dico(hInst), scroller(*this) {}

	HWND Create();
	Dims getDims() const {
		return Dims{ x, y, dimWidth, dimHeight };
	}
};

