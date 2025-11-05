#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <memory>
#include <sstream>

class Dico
{
	std::unique_ptr<void, decltype(&FreeResource)> up;
	LPCSTR words = nullptr;
	size_t sz = 0;

public:
	Dico(HINSTANCE hInst);

	bool match(LPCSTR word, LPCSTR mask);
	std::vector<std::string> findMatch(LPCSTR mask);

	std::wstring updateLetters(LPCTSTR text, DWORD & start, DWORD & end);
};

