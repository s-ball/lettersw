#pragma once
#include <Windows.h>
#include <exception>
#include <stdexcept>

class ArgError;

class Args
{
	LCID id = 0;
	UINT debug = 0;
	HINSTANCE hInst;

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
	Args& parent;

	DWORD FormatMessage(UINT cause, HINSTANCE hInst, va_list& args);
public:
	ArgError(Args& args, UINT cause, ...): parent(args), id(cause) {
		va_list argList;
		va_start(argList, cause);
		FormatMessage(cause, args.hInst, argList);
		va_end(argList);
	}

	const char* what() const {
		return "incorrect command line";
	}

	LPCWSTR wcsWhat() const {
		return msg;
	}

	UINT cause() const {
		return id;
	}
	static LPWSTR LoadLangString(UINT id, DWORD dwLang, HMODULE mod = NULL);
};

