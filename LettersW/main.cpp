#include <Windows.h>
#include "resource.h"
#include "Args.h"
#include "MainWindow.h"
#include "Preferences.h"


int WINAPI wWinMain(
    _In_ HINSTANCE hInst,
    _In_opt_ HINSTANCE /* hPrevInstance */,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
) {
    Args args;
    try {
        args = Args (lpCmdLine);
    }
    catch (ArgError& err) {
        MessageBoxW(NULL, err.wcsWhat(), NULL, MB_OK | MB_ICONERROR);
        return 1;
    }
    Preferences pref;

    pref.load();

    MainWindow mainWindow(args, hInst);
    HWND hWnd = mainWindow.Create();
    if (NULL == hWnd) {
        //if (TRUE) {
        LPWSTR msg;
        ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
            NULL, GetLastError(), 0, (LPWSTR)&msg, 0, NULL);
        ArgError err(hInst, IDS_CREATE_WINDOW, msg);
        MessageBoxW(NULL, err.wcsWhat(), NULL, MB_OK | MB_ICONERROR);
        return 1;
    }

    if (pref.getDims().ok) {
        const MainWindow::Dims& dims = pref.getDims();
        MoveWindow(hWnd, dims.x, dims.y, dims.w, dims.h, TRUE);
    }
    ShowWindow(hWnd, nShowCmd);

    HACCEL hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));

    // Application message loop
    MSG msg;
    BOOL cr;
    while ((cr = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (cr == -1) {
            LPWSTR msg;
            ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                NULL, GetLastError(), 0, (LPWSTR)&msg, 0, NULL);
            ArgError err(hInst, IDS_GETMESSAGE, msg);
            MessageBoxW(NULL, err.wcsWhat(), NULL, MB_OK | MB_ICONERROR);
            return 1;
        }
        if (TranslateAccelerator(hWnd, hAccel, &msg)) {
            continue;
        }
        if (IsDialogMessage(hWnd, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    DestroyAcceleratorTable(hAccel);
    pref.loadWnd(mainWindow);
    pref.save();
    return (int) msg.wParam;
}