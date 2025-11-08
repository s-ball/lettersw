#pragma once
#include <Windows.h>

class Control {
	unsigned char sizing, moving;
protected:
	UINT id;
	HWND hwnd;

	LONG x, width, y, height;

public:
	Control(UINT id, unsigned char sizing, unsigned char moving) {
		this->id = id;
		this->moving = moving;
		this->sizing = sizing;
	}
	virtual ~Control() = default;

	void onSize(DWORD oldW, DWORD newW);
	virtual void onInit(HWND parent, LPRECT parentRect);
};

class Edit : public Control {
	using Control::hwnd;
	unsigned selStart = 0, selEnd = 0;

public:
	Edit(UINT id, unsigned char sizing, unsigned char moving) :
		Control(id, sizing, moving) {}
	void saveSel();
	void restoreSel();
};

