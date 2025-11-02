#pragma once
#include <Windows.h>

class Control {
	LONG x, width, y, height;
	UINT id;
	HWND hwnd;
	unsigned char sizing, moving;

public:
	Control(UINT id, unsigned char sizing, unsigned char moving) {
		this->id = id;
		this->moving = moving;
		this->sizing = sizing;
	}
	virtual ~Control() = default;

	void onSize(DWORD oldW, DWORD newW);
	void onInit(HWND parent, LPRECT parentRect);
};

