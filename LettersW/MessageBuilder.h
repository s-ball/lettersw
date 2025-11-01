#pragma once
#include <Windows.h>
#include <exception>

class MessageIdException : public std::exception {
	static constexpr UINT SZ = 256;
	UINT stringId;
	static const char* const msg;
	mutable WCHAR w_msg[SZ] = L"";
public:
	MessageIdException(UINT id) : stringId(id) {}
	UINT getId() const noexcept {
		return stringId;
	}
	const char* what() const noexcept {
		return msg;
	}
	LPCWSTR wcsWhat() const;
};


class MessageBuilder
{
	HINSTANCE hInst;

	DWORD loadMessage(DWORD flags, DWORD msgId, LPWSTR msg, DWORD sz, va_list& args);
public:
	MessageBuilder(HINSTANCE hInst) : hInst(hInst) {}
	LPCWSTR getErrMsg(UINT error, LPWSTR buf, UINT size, ...);
	LPCWSTR getStringMsg(UINT id, LPWSTR buf, UINT size, ...);
	LPCWSTR getStringMsgV(UINT id, LPWSTR buf, UINT size, va_list& args);
};

