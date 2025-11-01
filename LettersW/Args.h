#pragma once
#include <Windows.h>
#include <exception>
#include <stdexcept>

class ArgError;

class Args
{
	LCID id = 0;
	UINT debug = 0;
	HINSTANCE hInst = 0;

	UINT parseInt(LPCWSTR arg);
	void setLanguage(LPCWSTR arg);

public:
	Args(LPCWSTR cmdLine, HINSTANCE hInst = NULL);
	Args() {};

	friend class ArgError;
};

class ArgError : public std::exception {
	static constexpr int SZ = 256;
	const UINT id;
	WCHAR msg[SZ];

public:
	ArgError(HINSTANCE hInst, UINT cause, ...);

	const char* what() const {
		return "incorrect command line";
	}

	LPCWSTR wcsWhat() const {
		return msg;
	}

	UINT cause() const {
		return id;
	}
	static LPWSTR LoadLangString(UINT id, WORD dwLang, HMODULE mod = NULL);
	static LPWSTR CopyString(LPCWSTR src);
};

