#include "MainWindow.h"
#include "resource.h"


HWND MainWindow::Create()
{
    hWnd = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC) StaticProc, (LPARAM) this);

    if (NULL != hWnd) {
        HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
        HICON hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));
        if (hIcon || hIcon2) {
            if (NULL == hIcon) hIcon = hIcon2;
            if (NULL == hIcon2) hIcon2 = hIcon;
            SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon2);
        }
    }
    return hWnd;
}

void MainWindow::Valid(BOOL enable = TRUE) {
    DWORD search[] = { IDC_CLEAR_SEARCH, IDC_VALID_SEARCH, IDC_EDIT_SEARCH };
    HWND child = 0;
    for (DWORD id : search) {
        child = GetDlgItem(hWnd, id);
        EnableWindow(child, enable);
    }
    if (enable) SetFocus(child);
}

void MainWindow::UpdateLetters() {
    if (!inUpdateLetters) {
        DWORD start, end;
        SendDlgItemMessage(hWnd, IDC_EDIT_LETTERS, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
        std::vector<WCHAR> text;
        unsigned int sz = 32;
        for (;;) {
            text.resize(sz);
            UINT cr = GetDlgItemTextW(hWnd, IDC_EDIT_LETTERS, text.data(), sz);
            if (cr >= sz - 1) {
                sz *= 2;
            }
            else break;
        }
        std::wstring str = dico.updateLetters(text.data(), start, end);
        inUpdateLetters = true;
        SetDlgItemTextW(hWnd, IDC_EDIT_LETTERS, str.c_str());
        inUpdateLetters = false;
        SendDlgItemMessage(hWnd, IDC_EDIT_LETTERS, EM_SETSEL, start, end);
        Valid(FALSE);
    }
}

INT_PTR MainWindow::StaticProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    MainWindow* wnd;
    if (msg == WM_INITDIALOG) {
        SetWindowLongPtr(hWnd, DWLP_USER, lp);
        wnd = (MainWindow*)lp;
    }
    else {
        wnd = (MainWindow*)GetWindowLongPtr(hWnd, DWLP_USER);
    }
    return wnd->Proc(hWnd, msg, wp, lp);
}

INT_PTR MainWindow::Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case ID_FILE_EXIT:
            DestroyWindow(hWnd);
            return TRUE;
        case ID_HELP_ABOUT:
            about.Exec(hWnd);
            return TRUE;
        case ID_EDIT_COPY:
            editCopy();
            return TRUE;
        case ID_EDIT_CUT:
            editCut();
            return TRUE;
        case ID_EDIT_PASTE:
            editPaste();
            return TRUE;
        case ID_EDIT_CLEARALL:
            Clear(ctrlId);
            return TRUE;
        }
        switch (HIWORD(wp)) {
        case BN_CLICKED:
            switch (LOWORD(wp)) {
            case IDC_CLEAR_LETTERS:
                Clear(IDC_EDIT_LETTERS);
                return TRUE;
            case IDC_VALID_LETTERS:
                Valid(TRUE);
                return TRUE;
            }
            break;
        case EN_UPDATE:
            switch (LOWORD(wp)) {
            case IDC_EDIT_LETTERS:
                UpdateLetters();
                break;
            }
            break;
        case EN_SETFOCUS:
            switch (LOWORD(wp)) {
            case IDC_EDIT_LETTERS:
                lettersEdit.restoreSel();
                break;
            case IDC_EDIT_SEARCH:
                searchEdit.restoreSel();
                break;
            }
        case BN_SETFOCUS:
            ctrlId = LOWORD(wp);
            break;
        case EN_KILLFOCUS:
            switch (LOWORD(wp)) {
            case IDC_EDIT_LETTERS:
                lettersEdit.saveSel();
                break;
            case IDC_EDIT_SEARCH:
                searchEdit.saveSel();
                break;
            }
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return TRUE;
    case WM_NCDESTROY:
        PostQuitMessage(0);
        return TRUE;
    case WM_INITDIALOG:
        this->hWnd = hWnd;
        return OnInitDialog(wp, lp);
    case WM_SIZING:
        return OnSizing(wp, lp);
        break;
    case WM_SIZE:
        return OnSize(wp, lp);
        break;
    case WM_MOVE:
        return OnMove(wp, lp);
        break;
    case WM_WINDOWPOSCHANGED:
        return OnWindowPosChanged((WINDOWPOS*)lp);
        break;
    case WM_INITMENUPOPUP:
        if (1 == LOWORD(lp)) { // Edit menu
            initEditMenu((ctrlId == IDC_EDIT_LETTERS) || (ctrlId == IDC_EDIT_SEARCH));
        }
        break;
    }
    return FALSE;
}

INT_PTR MainWindow::OnSizing(WPARAM wp, LPARAM lp) {
    RECT* rect = (RECT*)lp;
    if (rect->bottom - rect->top < minHeight) {
        switch (wp) {
        case WMSZ_BOTTOM:
        case WMSZ_BOTTOMLEFT:
        case WMSZ_BOTTOMRIGHT:
            rect->bottom = rect->top + minHeight;
            break;
        default:
            rect->top = rect->bottom - minHeight;
        }
    }
    if (rect->right - rect->left < minWidth) {
        switch (wp) {
        case WMSZ_LEFT:
        case WMSZ_BOTTOMLEFT:
        case WMSZ_TOPLEFT:
            rect->left = rect->right - minWidth;
            break;
        default:
            rect->right = rect->left + minWidth;
        }
    }
    return TRUE;
}

INT_PTR MainWindow::OnInitDialog(WPARAM, LPARAM) {
    RECT rect;
    GetWindowRect(GetDlgItem(hWnd, IDC_TOPLEFT), &rect);
    DWORD top = rect.top, left = rect.left;
    GetWindowRect(hWnd, &rect);
    height = minHeight = rect.bottom - rect.top;
    oldWidth = width = minWidth = rect.right - rect.left;
    GetClientRect(hWnd, &rect);
    rect.top += top;
    rect.bottom += top;
    rect.left += left;
    rect.right += left;
    for (auto child : children) {
        child->onInit(hWnd, &rect);
    }
    return TRUE;
}

INT_PTR MainWindow::OnSize(WPARAM wp, LPARAM lp) {
    if (wp == SIZE_RESTORED) {
        width = LOWORD(lp);
        height = HIWORD(lp);
    }
    for (auto child : children) {
        child->onSize(oldWidth, LOWORD(lp));
    }
    oldWidth = LOWORD(lp);
    return TRUE;
}

INT_PTR MainWindow::OnMove(WPARAM, LPARAM lp)
{
    x = LOWORD(lp);
    y = HIWORD(lp);
    return TRUE;
}

INT_PTR MainWindow::OnWindowPosChanged(WINDOWPOS* wp) {
    x = wp->x;
    y = wp->y;
    width = wp->cx;
    height = wp->cy;
    for (auto child : children) {
        child->onSize(oldWidth, wp->cx);
    }
    oldWidth = wp->cx;
    return TRUE;
}

void MainWindow::initEditMenu(bool inEdit) {
    HMENU menu = GetMenu(hWnd);
    if (!inEdit) {
        UINT items[] = { ID_EDIT_COPY, ID_EDIT_CUT, ID_EDIT_PASTE, ID_EDIT_CLEARALL };
        for (auto item : items) {
            EnableMenuItem(menu, item, MF_BYCOMMAND | MF_GRAYED);
        }
    }
    else {
        HWND child = GetDlgItem(hWnd, ctrlId);
        WCHAR txt[8];
        GetWindowTextW(child, txt, sizeof(txt) / sizeof(*txt));
        EnableMenuItem(menu, ID_EDIT_CLEARALL,
            (0 == txt[0]) ? MF_GRAYED : MF_ENABLED);
        DWORD sel = (DWORD) SendMessage(child, EM_GETSEL, 0, 0);
        EnableMenuItem(menu, ID_EDIT_COPY,
            (LOWORD(sel) == HIWORD(sel)) ? MF_GRAYED : MF_ENABLED);
        EnableMenuItem(menu, ID_EDIT_CUT,
            (LOWORD(sel) == HIWORD(sel)) ? MF_GRAYED : MF_ENABLED);
        EnableMenuItem(menu, ID_EDIT_PASTE,
            IsClipboardFormatAvailable(CF_UNICODETEXT) ? MF_ENABLED : MF_GRAYED);
    }
}

bool MainWindow::editCopy() {
    DWORD start, end;
    HWND child = GetDlgItem(hWnd, ctrlId);
    DWORD sel = (DWORD)SendMessageW(child, EM_GETSEL, (WPARAM) & start, (WPARAM) & end);
    if (start == end) return false;
    if (start > end) std::swap(start, end);
    std::vector<WCHAR> temp(1 + end);
    GetWindowTextW(child, temp.data(), 1 + end);
    temp[end] = 0;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, sizeof(WCHAR) * (1 + end - start));
    if (hMem) {
        LPWSTR ix = (LPWSTR)GlobalLock(hMem);
        lstrcpyW(ix, temp.data() + start);
        GlobalUnlock(hMem);

        OpenClipboard(hWnd);
        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, hMem);
        CloseClipboard();
        return true;
    }
    return false;
}

void MainWindow::editCut() {
    if (editCopy()) {
        SendDlgItemMessageW(hWnd, ctrlId, EM_REPLACESEL, TRUE, (LPARAM)L"");
    }
}

void MainWindow::editPaste() {
    OpenClipboard(hWnd);
    HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);
    if (hMem) {
        HWND child = GetDlgItem(hWnd, ctrlId);
        LPCWSTR ix = (LPCWSTR)GlobalLock(hMem);
        int sz = (int) (GlobalSize(hMem) + 1) / sizeof(WCHAR);
        std::vector<WCHAR> temp(sz + 1);
        lstrcpynW(temp.data(), ix, sz);
        temp[sz] = 0;
        GlobalUnlock(hMem);
        SendMessageW(child, EM_REPLACESEL,
            TRUE, (LPARAM) temp.data());
        DWORD start, end;
        SendMessage(child, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
        if (end < start) std::swap(start, end);
        SendMessage(child, EM_SETSEL, end, end);
    }
}
