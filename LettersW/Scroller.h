#pragma once
#include <Windows.h>

class ScrollOwner {
public:
	virtual void adjustClient(DWORD oldW, DWORD newW, DWORD yScroll) = 0;
	virtual HWND getHwnd() = 0;
};

class Scroller {
	ScrollOwner& parent;

	SCROLLINFO si = { sizeof(si), SIF_ALL, 0, 0, 0, 0 };

	void setScrollbar(bool set);
	void adjustPos(UINT newPos);

public:
	Scroller(ScrollOwner& parent) : parent(parent) {}
	void setMaxPos(UINT pos);
	bool scrollMsg(UINT msg, WPARAM wp);
};

