#include "Dico.h"
#include "Args.h"
#include <memory>
#include "resource.h"

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
}

bool Dico::match(LPCSTR word, LPCSTR mask)
{
	while (*word == *mask || *mask == '_') {
		if (0 == *word) return true;
		word++;
		mask++;
	}
	return false;
}

std::vector<std::string> Dico::findMatch(LPCSTR mask) {
	std::istringstream in(std::string(words, sz));
	std::vector<std::string> ret;
	int count = 0;
	for (;;) {
		std::string w;
		if (std::getline(in, w)) {
			if (++count % 10000 == 0) {
				std::ostringstream msg;
				msg << "Found " << count << "lines - pos=" << in.tellg() << " - continue ? ";
				if (MessageBoxA(NULL, msg.str().c_str(), "Words", MB_YESNO) == IDNO) break;
			}

			if (match(w.c_str(), mask)) {
				ret.push_back(w);
			}
		}
		if (! in) break;
		if (in.tellg() >= (int) sz) break;
	}
	return ret;
}

std::wstring Dico::updateLetters(LPCTSTR text, DWORD& start, DWORD& end)
{
	return std::wstring(text);
}
