#pragma once
#include <Windows.h>
#include "MainWindow.h"

class Preferences
{
	struct WndDims: public MainWindow::Dims {
		WndDims() {}
		WndDims(const MainWindow::Dims& src) : MainWindow::Dims(src) {}
		bool ok = false;
	} mainWnd;
	HKEY iniReg();
public:
	void load();
	void save();
	void loadWnd(const MainWindow& wnd);
	const WndDims& getDims() {
		return mainWnd;
	}
};

