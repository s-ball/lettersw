#include "Args.h"
#include "resource.h"
#include <stdlib.h>


UINT Args::parseInt(LPCWSTR arg) {
    LPWSTR end;
    UINT id = wcstoul(arg, &end, 10);
    if (*end != 0) {
        throw ArgError(*this, IDS_WRONG_DEBUG, arg);
    }
    return id;
}

void Args::setLanguage(LPCWSTR arg)
{
    ULONG nLangs;
    ULONG sz = 0;
    BOOL cr;
    cr = GetThreadPreferredUILanguages(MUI_MERGE_SYSTEM_FALLBACK | MUI_MERGE_USER_FALLBACK | MUI_LANGUAGE_NAME,
        &nLangs, NULL, &sz);

    sz += lstrlenW(arg) + 1;
    LPWSTR langs = new WCHAR[sz];
    lstrcpy(langs, arg);
    cr = GetThreadPreferredUILanguages(MUI_MERGE_SYSTEM_FALLBACK | MUI_MERGE_USER_FALLBACK | MUI_LANGUAGE_NAME,
        &nLangs, langs + lstrlenW(arg) + 1, &sz);
    nLangs += 1;
    cr = SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, langs, &nLangs);
    delete[] langs;
}


Args::Args(LPCWSTR cmdLine, HINSTANCE hInst): hInst(hInst) {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(cmdLine, &argc);
    if (**argv == 0) {  // handle a possibly empty first argument
        argv++;
        argc--;
    }
    if (NULL == this->hInst) {
        this->hInst = GetModuleHandle(NULL);
    }
    switch (argc) {
    case 2:
        id = parseInt(argv[1]);
    case 1:
        setLanguage(argv[0]);
    case 0:
        break;
    default:
        throw ArgError(*this, IDS_TOO_MANY_ARGS);
    }
}

DWORD ArgError::FormatMessage(UINT cause, HINSTANCE hInst, va_list& args)
{
    WCHAR tmp[SZ];

    DWORD cr = LoadString(hInst, cause, tmp, SZ);
    if (cr == 0) {

    }
    ::FormatMessage(FORMAT_MESSAGE_FROM_STRING, tmp, 0, 0,
        msg, SZ, &args);
    return cr;
}

LPWSTR ArgError::LoadLangString(UINT id, DWORD dwLang, HMODULE mod)
{
    UINT offset = id & 0x0F;
    UINT tableId = (id >> 4) + 1;
    LPWSTR msg = NULL;
    HRSRC hRsrc = FindResourceExW(mod, RT_STRING, MAKEINTRESOURCE(tableId), dwLang);
    if (hRsrc) {
        HGLOBAL hGlob = LoadResource(mod, hRsrc);
        if (hGlob) {
            UINT len = 0;
            LPCWSTR block = (LPCWSTR)LockResource(hGlob);
            DWORD blockSize = SizeofResource(mod, hRsrc);
            for (LPCWSTR end = block + blockSize; block < end;) {
                len = *block++;
                if (offset-- == 0) {
                    if (0 == len) {
                        WCHAR txt[SZ];
                        wsprintfW(txt, L"Inexistant string resource: %d", id);
                        msg = new WCHAR[lstrlenW(txt) + 1];
                        lstrcpyW(msg, txt);
                    }
                    else {
                        msg = new WCHAR[len + 1];
                        lstrcpynW(msg, block, len + 1);
                        msg[len] = 0;
                    }
                    break;
                }
                else {
                    block += len;
                }
            }
            FreeResource(hGlob);
            return msg;
        }
    }
    WCHAR buf[SZ], tmp[SZ] = L"unknown error";
    ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0,
        tmp, sizeof(tmp), NULL);
    wsprintfW(buf, L"Cannot load resource for string %d\n", id);
    msg = new WCHAR[lstrlenW(buf) + lstrlenW(tmp) + 1];
    lstrcpyW(msg, buf);
    lstrcatW(msg, tmp);
    return msg;
}
