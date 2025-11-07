#include "Dico.h"
#include "Args.h"
#include <memory>
#include "resource.h"
#include <cctype>

Dico::Dico(HINSTANCE hInst) : up(nullptr, FreeResource) {
	HRSRC hRsrc = FindResourceEx(hInst, RT_RCDATA, MAKEINTRESOURCE(IDR_WORDS),
		MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH));
	if (NULL != hRsrc) {
		sz = SizeofResource(hInst, hRsrc);
		HGLOBAL hGlob = LoadResource(hInst, hRsrc);
		if (NULL != hGlob) {
			LPVOID data = LockResource(hGlob);
			if (nullptr != data) {
				words = static_cast<LPSTR>(data);
				up.reset(hGlob);
			}
		}
	}
	// load transliterators
	UErrorCode err = U_ZERO_ERROR;

	availableTrans = utrans_openU(
		(UChar*)L"Any-Latin; NFKD; [:Nonspacing Mark:] Remove; NFC; Upper; [^A-Z] Remove",
		-1, UTRANS_FORWARD,
		NULL, -1, nullptr, &err);
	searcherTrans = utrans_openU(
		(UChar*)L"Any-Latin; NFKD; [:Nonspacing Mark:] Remove; NFC; Upper; [^A-Z_*] Remove",
		-1, UTRANS_FORWARD,
		NULL, -1, nullptr, &err);
	conv = ucnv_open(nullptr, &err);
}

bool Dico::match(LPCSTR word, LPCWSTR mask, LPCWSTR letters)
{
	const char* ix = word;
	while (std::toupper(*ix) == *mask || *mask == '_') {
		if (0 == *ix) break;
		ix++;
		mask++;
	}
	if (0 != *ix || 0 != *mask) return false;
	std::vector<WCHAR> w(1 + lstrlenW(letters));
	lstrcpy(w.data(), letters);
	for (ix = word; *ix != 0; ix++) {
		bool ok = false;
		for (auto& c : w) {
			if (c == std::toupper(*ix)) {
				c = '\0';
				ok = true;
				break;
			}
		}
		if (!ok) return false;
	}
	return true;
}

std::vector<std::vector<WCHAR>> Dico::findMatch(LPCWSTR mask, LPCWSTR letters) {
	std::istringstream in(std::string(words, sz));
	std::vector<std::vector<WCHAR>> ret;

	UErrorCode err = U_ZERO_ERROR;

	int count = 0;
	for (;;) {
		std::string w;
		if (std::getline(in, w)) {
			if (match(w.c_str(), mask, letters)) {
				std::vector<WCHAR> word(w.size());
				ucnv_toUChars(conv, (UChar*)word.data(), (int32_t) word.size(),
					w.c_str(), (int32_t)w.length(), &err);
				for (auto& c : word) {
					c = std::toupper(c);
				}
				ret.push_back(word);
			}
		}
		else break;
	}
	return ret;
}

std::vector<WCHAR> Dico::updateLetters(LPCWSTR text, DWORD& start, DWORD& end, bool letters = true)
{
	int32_t sz = lstrlenW(text);
	int32_t limit = sz;
	std::vector<WCHAR> data(1 + sz);
	lstrcpyW(data.data(), text);
	UErrorCode err = U_ZERO_ERROR;
	utrans_transUChars((letters) ? availableTrans : searcherTrans,
		(UChar *) data.data(), &sz, 1 + sz,
		0, &limit, &err);
	data.resize(1 + sz);
	if (start > (unsigned)sz) start = sz;
	if (end > (unsigned)sz) end = sz;
	return data;
}
