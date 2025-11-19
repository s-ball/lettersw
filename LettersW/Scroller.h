#pragma once
#include <Windows.h>

class ScrollOwner {
public:
	static const DWORD APPM_SCROLL = WM_APP + 1;
	virtual HWND getHwnd() const = 0;
	virtual int lineSize() const = 0;
	virtual int pageSize() const = 0;
};

class Scroller {

	int line = 10;
	int page = 40;

	ScrollOwner& parent;

	SCROLLINFO si = { sizeof(si), SIF_ALL, 0, 0, 0, 0 };

	void setScrollbar(bool set);
	void adjustPos(UINT newPos);

public:
	Scroller(ScrollOwner& parent) : parent(parent) {}
	void setMaxPos(int pos);
	bool scrollMsg(WPARAM wp);
	void setCurPos(int pos);
};

