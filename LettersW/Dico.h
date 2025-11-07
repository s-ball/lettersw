#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <memory>
#include <sstream>
#include <icu.h>

class Dico
{
	std::unique_ptr<void, decltype(&FreeResource)> up;
	LPCSTR words = nullptr;
	size_t sz = 0;

	UTransliterator* availableTrans = nullptr, * searcherTrans = nullptr;
	UConverter *conv = nullptr;

	// neither copyable nor moveable
	Dico(const Dico&) = delete;
	Dico(Dico&&) = delete;
	const Dico& operator = (const Dico&) = delete;
	Dico&& operator = (Dico&&) = delete;


public:
	Dico(HINSTANCE hInst);

	bool match(LPCSTR word, LPCWSTR mask, LPCWSTR letters);
	std::vector<std::vector<WCHAR>> findMatch(LPCWSTR mask, LPCWSTR letters);

	std::vector<WCHAR> updateLetters(LPCWSTR text, DWORD& start, DWORD& end, bool letters);

	~Dico() {
		if (availableTrans) {
			utrans_close(availableTrans);
		}
		if (searcherTrans) {
			utrans_close(searcherTrans);
		}
	}
};

