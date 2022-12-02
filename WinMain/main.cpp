#include <windows.h>

int GetNum(int base);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define IDS_BUTTON 1

HINSTANCE hInstance;
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    ::hInstance = hInstance;
    const wchar_t CLASS_NAME[] = L"Dll-Hook";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Dll-Hook-WinMain",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WCHAR buf[MAX_PATH];
    switch (uMsg) {
    case WM_CREATE:
        CreateWindowW(L"BUTTON", L"button", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 0, 0, 100, 50, hwnd, (HMENU) IDS_BUTTON, hInstance, NULL);
        return 0;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDS_BUTTON) {
            if (HIWORD(wParam) == BN_CLICKED) {
                int num = GetNum(456);
                wsprintfW(buf, L"num: %d", num);
                MessageBoxW(NULL, buf, L"123", MB_OK);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

ULONGLONG TimeMilliSecond() {
    FILETIME file_time;
    GetSystemTimeAsFileTime(&file_time);
    ULONGLONG time = ((ULONGLONG)file_time.dwLowDateTime) + ((ULONGLONG)file_time.dwHighDateTime << 32);
    static const ULONGLONG EPOCH = ((ULONGLONG)116444736000000000ULL);
    return (ULONGLONG)((time - EPOCH) / 10000LL);
}
int GetNum(int base) {
    if (base > 0) {
        return base + GetNum(base - 1);
    }
    return base;
}
