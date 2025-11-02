#include "Preferences.h"
#include <Windows.h>
#include <tchar.h>


HKEY Preferences::iniReg() {
	HKEY key;
	LSTATUS cr =RegCreateKey(HKEY_CURRENT_USER, _T("SOFTWARE\\s-ball\\LettersW"), &key);
	return ((ERROR_SUCCESS == cr) ? key : NULL);
}

template<typename T>
static void readValue(HKEY key, LPCWSTR name, T& var) {
	DWORD size = sizeof(var);
	RegQueryValueEx(key, name, NULL, NULL, (LPBYTE) & var, &size);
}

void Preferences::load() {
	HKEY key = iniReg();
	if (!key) return;
	HKEY wKey;
	LSTATUS cr = RegOpenKey(key, _T("MainWindow"), &wKey);
	if (ERROR_SUCCESS == cr) {
		readValue(wKey, _T("X"), mainWnd.x);
		readValue(wKey, _T("Y"), mainWnd.y);
		readValue(wKey, _T("W"), mainWnd.w);
		readValue(wKey, _T("H"), mainWnd.h);
		mainWnd.ok = true;
		RegCloseKey(wKey);
	}
	RegCloseKey(key);
}

void Preferences::save() {
	HKEY key = iniReg();
	if (!key) return;
	if (mainWnd.ok) {
		RegSetKeyValue(key, _T("MainWindow"), _T("X"), REG_DWORD, &mainWnd.x, sizeof(mainWnd.x));
		RegSetKeyValue(key, _T("MainWindow"), _T("Y"), REG_DWORD, &mainWnd.y, sizeof(mainWnd.y));
		RegSetKeyValue(key, _T("MainWindow"), _T("W"), REG_DWORD, &mainWnd.w, sizeof(mainWnd.w));
		RegSetKeyValue(key, _T("MainWindow"), _T("H"), REG_DWORD, &mainWnd.h, sizeof(mainWnd.h));
	}
	RegCloseKey(key);
}

void Preferences::loadWnd(const MainWindow& wnd) {
	mainWnd = wnd.getDims();
	mainWnd.ok = true;
}
