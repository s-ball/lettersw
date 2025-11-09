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
    SendMessage(hWnd, DM_SETDEFID, enable ? IDC_VALID_SEARCH : IDC_VALID_LETTERS, 0);
}

void MainWindow::UpdateLetters(int id = IDC_EDIT_LETTERS, bool letters = true) {
    if (!inUpdateLetters) {
        DWORD start, end;
        SendDlgItemMessage(hWnd, id, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
        std::vector<WCHAR> text;
        unsigned int sz = 32;
        for (;;) {
            text.resize(sz);
            UINT cr = GetDlgItemTextW(hWnd, id, text.data(), sz);
            if (cr >= sz - 1) {
                sz *= 2;
            }
            else break;
        }
        std::vector<WCHAR> str = dico.updateLetters(text.data(), start, end, letters);
        if (!letters) {
            FilterSearch(str);
        }
        inUpdateLetters = true;
        SetDlgItemTextW(hWnd, id, str.data());
        inUpdateLetters = false;
        SendDlgItemMessage(hWnd, id, EM_SETSEL, start, end);
        if (letters) Valid(FALSE);
    }
}

void MainWindow::FilterSearch(std::vector<WCHAR>& str) {
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
    text.resize(1 + lstrlenW(text.data()));
    auto dest = str.begin();
    for (auto src = str.cbegin(); src != str.end(); ++src) {
        if (0 == *src) {
            *dest++ = 0;
            break;
        }
        if ('*' == *src) {
            str = std::vector<WCHAR>{ '*', 0 };
            return;
        }
        if ('_' == *src) {
            *dest++ = *src;
            continue;
        }
        bool found = false;
        for (WCHAR& letter : text) {
            if (letter == *src) {
                letter = 0;
                found = true;
                break;
            }
        }
        if (found) *dest++ = *src;
    }
    str.resize(dest - str.begin());
    if (str.size() > text.size()) {
        str[text.size() - 1] = 0;
    }
}

void MainWindow::Search() {
    std::vector<std::vector<WCHAR>> wordlist;
    std::vector<WCHAR> letters(32);
    std::vector<WCHAR> mask(32);
    for (;;) {
        UINT cr = GetDlgItemTextW(hWnd, IDC_EDIT_LETTERS,
            letters.data(), (int) letters.size());
        if (cr >= letters.size() - 1) letters.resize(2 * letters.size());
        else break;
    }
    for (;;) {
        UINT cr = GetDlgItemTextW(hWnd, IDC_EDIT_SEARCH,
            mask.data(), (int) mask.size());
        if (cr >= mask.size() - 1) mask.resize(2 * mask.size());
        else break;
    }
    wordlist = dico.findMatch(mask.data(), letters.data());
    wd.display1(std::move(wordlist)
);
    return;
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
            case IDC_CLEAR_SEARCH:
                Clear(IDC_EDIT_SEARCH);
                return TRUE;
            case IDC_VALID_SEARCH:
                Search();
                return TRUE;
            }
            break;
        case EN_UPDATE:
            switch (LOWORD(wp)) {
            case IDC_EDIT_LETTERS:
                UpdateLetters(IDC_EDIT_LETTERS, true);
                break;
            case IDC_EDIT_SEARCH:
                UpdateLetters(IDC_EDIT_SEARCH, false);
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
        return FALSE; // OnSize(wp, lp);
        break;
    case WM_MOVE:
        return FALSE; // OnMove(wp, lp);
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
    dimHeight = height = minHeight = rect.bottom - rect.top;
    dimWidth = width = minWidth = rect.right - rect.left;
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
    LONG w = LOWORD(lp);
    LONG h = HIWORD(lp);
    wd.onVSize(height, h);
    for (auto child : children) {
        child->onSize(width, w);
    }
    width = w;
    height = h;
    return TRUE;
}

INT_PTR MainWindow::OnMove(WPARAM, LPARAM lp)
{
    x = LOWORD(lp);
    y = HIWORD(lp);
    return TRUE;
}

INT_PTR MainWindow::OnWindowPosChanged(WINDOWPOS* wp) {
    wd.onVSize(height, wp->cy);
    for (auto child : children) {
        child->onSize(width, wp->cx);
    }

    width = wp->cx;
    height = wp->cy;

    if (!IsIconic(hWnd) && (!IsZoomed(hWnd))) {
        x = wp->x;
        y = wp->y;
        dimHeight = height;
        dimWidth = width;
    }
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
