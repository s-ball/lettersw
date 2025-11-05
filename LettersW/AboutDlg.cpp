#include "AboutDlg.h"
#include "resource.h"
#include "MessageBuilder.h"
#include <vector>
#include <ImageHlp.h>
#include <ctime>


void AboutDlg::Exec(HWND hWnd) {
    parent = hWnd;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC) StaticProc, (LPARAM) this);
}

LPCWSTR AboutDlg::getVersion()
{
    if (!version) {
        version = LoadVersionString();
    }
    return version;
}

INT_PTR AboutDlg::StaticProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    AboutDlg* wnd;
    if (msg == WM_INITDIALOG) {
        SetWindowLongPtr(hWnd, DWLP_USER, lp);
        wnd = (AboutDlg*)lp;
    }
    else {
        wnd = (AboutDlg*)GetWindowLongPtr(hWnd, DWLP_USER);
    }
    return wnd->Proc(hWnd, msg, wp, lp);
}

INT_PTR AboutDlg::Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM) {
    switch (msg) {
    case WM_INITDIALOG:
        {
        LPCWSTR version = getVersion();
        SetDlgItemText(hwnd, IDC_VERSION, version);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDOK:
        case IDCANCEL:
            EndDialog(hwnd, LOWORD(wp));
            break;
        default:
            return FALSE;
        }
    default:
        return FALSE;
    }
    return TRUE;
}

LPCWSTR AboutDlg::LoadVersionString()
{
    LPWSTR name;
    DWORD len;
    DWORD handle;
    UINT ids = IDS_VERSION;
    try {
        _get_wpgmptr(&name);
        len = GetFileVersionInfoSizeW(name, &handle);
        std::vector<char> data;
        data.resize(len);
        BOOL cr = GetFileVersionInfoW(name, NULL, len, data.data());
        if (!cr) throw ArgError(hInst, IDS_VERSION_ERROR);
        UINT l2;
        VS_FIXEDFILEINFO* fixedInfo;
        cr = VerQueryValueW(data.data(), L"\\", (LPVOID*)&fixedInfo, &l2);
        if (!cr) throw ArgError(hInst, IDS_VERSION_ERROR);
        BOOL preRelease = fixedInfo->dwFileFlags & VS_FF_PRERELEASE;
        int year = 0, month = 0, day = 0;
        struct {
            WORD lang;
            WORD encoding;
        } *bloc;
        cr = VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation",
            (LPVOID*)&bloc, &l2);
        if (!cr) throw ArgError(hInst, IDS_VERSION_ERROR);
        WCHAR key[64];
        wsprintf(key, L"\\StringFileInfo\\%04x%04x\\FileVersion", bloc->lang, bloc->encoding);
        cr = VerQueryValueW(data.data(), key, (LPVOID*)&version, &l2);
        if (!cr) throw ArgError(hInst, IDS_VERSION_ERROR);
        if (preRelease) {
            DWORD sz = 32;
            std::vector<char> v_name;
            for (;;) {
                v_name.resize(sz);
                DWORD len = GetModuleFileNameA(NULL, v_name.data(), sz);
                if (len < sz - 1) break;
                sz *= 2;
            }
            LPSTR name = v_name.data();

            PLOADED_IMAGE pi = ImageLoad(name, name);
            if (0 != pi) {
                IMAGE_NT_HEADERS* ih = pi->FileHeader;
                IMAGE_FILE_HEADER* ifh = &(ih->FileHeader);

                time_t ts = ifh->TimeDateStamp;
                tm dt;

                localtime_s(&dt, &ts);

                year = dt.tm_year + 1900;
                month = dt.tm_mon + 1;
                day = dt.tm_mday;

                ImageUnload(pi);

                ids = IDS_VERSION_DAT;
            }
        }
        version = MessageBuilder(hInst).getStringMsg(ids, NULL, 0, version, year, month, day);
    }
    catch (ArgError& err) {
        MessageBox(parent, err.wcsWhat(), NULL, MB_OK | MB_ICONERROR);
        version = ArgError::CopyString(L"0.0.0");
    }
    return version;
}


