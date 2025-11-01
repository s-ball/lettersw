#include "MessageBuilder.h"
#include "Args.h"
#include "resource.h"
#include <memory>
#include <vector>

const char* const MessageIdException::msg = "MessageIdException";

inline DWORD MessageBuilder::loadMessage(DWORD flags, DWORD msgId, 
	LPWSTR msg, DWORD sz, va_list& args) {
	DWORD cr;
	if (0 == (flags & FORMAT_MESSAGE_FROM_STRING)) {
		// Try current Thread language, then 0
		LCID lid = GetThreadLocale();
		WORD langs[] = { LANGIDFROMLCID(lid), 0 };
		for (WORD langId : langs) {
			cr = FormatMessageW(flags, NULL, msgId, langId, msg, sz, &args);
			if (cr != 0 || GetLastError() != ERROR_RESOURCE_LANG_NOT_FOUND) break;
		}
	}
	else {
		UINT bufsize = 64;
		std::vector<WCHAR> buf;

		for (;;) {
			buf.resize(bufsize);
			cr = LoadStringW(hInst, msgId, buf.data(), bufsize);
			if (cr == bufsize - 1) {
				bufsize *= 2;
			}
			else break;
		}
		if (cr == 0) {
			*buf.data() = 0;
			HRSRC hRsrc = FindResourceExW(hInst, RT_STRING, MAKEINTRESOURCE(msgId >> 4),
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
			WORD offset = msgId & 0x0f;
			if (hRsrc) {
				HGLOBAL hGlob = LoadResource(hInst, hRsrc);
				if (hGlob) {
					UINT len = 0;
					LPCWSTR block = (LPCWSTR)LockResource(hGlob);
					if (block) {
						std::unique_ptr<WCHAR, decltype(&FreeResource)>
							up(const_cast<LPWSTR>(block), &FreeResource);
						DWORD blockSize = SizeofResource(hInst, hRsrc);
						for (LPCWSTR end = block + blockSize; block < end;) {
							len = *block++;
							if (offset-- == 0) {
								if (0 == len) {
									throw MessageIdException(msgId);
								}
								else {
									if (bufsize <= len) {
										buf.resize(len + 1);
									}
									lstrcpynW(buf.data(), block, len + 1);
									msg[len] = 0;
								}
								break;
							}
							else {
								block += len;
							}
						}
					}
				}
			}
			if (*buf.data() == 0) throw MessageIdException(msgId);
		}
		// Ok buf contains the required string
		cr = FormatMessageW(flags, buf.data(), 0, 0, msg, sz, &args);
	}
	return cr;
}

LPCWSTR MessageBuilder::getErrMsg(UINT error, LPWSTR buf, UINT size, ...)
{
	va_list args;
	va_start(args, size);
	DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM;
	LPWSTR msg = buf;
	if (NULL == buf) {
		flags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
		msg = reinterpret_cast<LPWSTR>(&buf);
	}
	if (error == 0) error = GetLastError();
	DWORD cr = loadMessage(flags, error, msg, size, args);
	va_end(args);
	return (cr == 0) ? NULL : buf;
}

LPCWSTR MessageBuilder::getStringMsg(UINT id, LPWSTR buf, UINT size, ...) {
	va_list args;
	va_start(args, size);
	LPCWSTR cr = getStringMsgV(id, buf, size, args);
	va_end(args);
	return cr;
}

LPCWSTR MessageBuilder::getStringMsgV(UINT id, LPWSTR buf, UINT size, va_list& args)
{
	DWORD flags = FORMAT_MESSAGE_FROM_STRING;
	LPWSTR msg = buf;
	if (NULL == buf) {
		flags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
		msg = reinterpret_cast<LPWSTR>(&buf);
	}
	DWORD cr = loadMessage(flags, id, msg, size, args);
	return (cr == 0) ? NULL : buf;
}

LPCWSTR MessageIdException::wcsWhat() const {
	if (0 == w_msg[0]) {
		MessageBuilder builder(GetModuleHandleW(NULL));
		try {
			builder.getStringMsg(IDS_RSRC_STRING, w_msg, SZ);
		}
		catch (MessageIdException&) {
			wsprintfW(w_msg, L"Cannot find resource %d (%02x)", stringId, stringId);
		}
	}
	return w_msg;
}
