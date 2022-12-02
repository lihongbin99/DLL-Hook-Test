#include <windows.h>
#include <psapi.h>

void injectDLL();
void freeDLL();

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define IDS_BUTTON_INJECT      1
#define IDS_BUTTON_FREE        2
#define IDS_EDIT_CLASS_NAME    3
#define IDS_EDIT_WINDOWS_NAME  4
#define IDS_EDIT_DLL_NAME      5

int fontHeight = 20;

HINSTANCE hInstance;
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    ::hInstance = hInstance;
    const wchar_t CLASS_NAME[] = L"Inject-Tool";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(0x00FFFFFF);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"DLL注入工具",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 280, 4 * (fontHeight + 4) + 38,
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
    HACCEL hAccel = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        if (!TranslateAcceleratorW(hwnd, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return 0;
}


HWND hwndClassName;
HWND hwndWindwosName;
HWND hwndDllName;

PCWSTR defaultClassName = L"请输入 class name";
PCWSTR defaultWindowName = L"请输入 window name";
PCWSTR defaultDllName = L"请输入 dll name";
void setFocus(HWND hwnd, int focusFlag, PCWSTR defaultText) {
    WCHAR buf[1024];
    int textLen = GetWindowTextW(hwnd, buf, sizeof(buf) / sizeof(WCHAR));
    if (focusFlag == 1) {
        if (textLen == lstrlenW(defaultText)) {
            if (lstrcmpW(buf, defaultText) == 0) {
                SetWindowTextW(hwnd, L"");
            }
        }
    }
    else if (focusFlag == 0) {
        if (textLen == 0) {
            SetWindowTextW(hwnd, defaultText);
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    TEXTMETRICW tm;
    PAINTSTRUCT ps;
    static HFONT hFont;

    static HWND hwndInject;
    static HWND hwndFree;

    WCHAR buf[MAX_PATH];
    switch (uMsg) {
    case WM_CREATE:
        // 创建输入框
        hwndClassName = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0, hwnd, (HMENU)IDS_EDIT_CLASS_NAME, hInstance, NULL);
        hwndWindwosName = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0, hwnd, (HMENU)IDS_EDIT_WINDOWS_NAME, hInstance, NULL);
        hwndDllName = CreateWindowW(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0, hwnd, (HMENU)IDS_EDIT_DLL_NAME, hInstance, NULL);

        //创建字体
        hFont = CreateFont(fontHeight, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("微软雅黑"));
        //设置编辑框的字体
        SendMessage(hwndClassName, WM_SETFONT, (WPARAM)hFont, NULL);
        SendMessage(hwndWindwosName, WM_SETFONT, (WPARAM)hFont, NULL);
        SendMessage(hwndDllName, WM_SETFONT, (WPARAM)hFont, NULL);

        // 创建按钮
        hwndInject = CreateWindowW(L"BUTTON", L"注入", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 0, 0, 0, 0, hwnd, (HMENU)IDS_BUTTON_INJECT, hInstance, NULL);
        hwndFree = CreateWindowW(L"BUTTON", L"释放", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 0, 0, 0, 0, hwnd, (HMENU)IDS_BUTTON_FREE, hInstance, NULL);

        // 设置默认文本
        setFocus(hwndClassName, 0, defaultClassName);
        setFocus(hwndWindwosName, 0, defaultWindowName);
        setFocus(hwndDllName, 0, defaultDllName);

        return 0;
    case WM_SIZE:
        MoveWindow(hwndClassName, 0, 0, LOWORD(lParam), fontHeight + 4, TRUE);
        MoveWindow(hwndWindwosName, 0, fontHeight + 4, LOWORD(lParam), fontHeight + 4, TRUE);
        MoveWindow(hwndDllName, 0, 2 * (fontHeight + 4), LOWORD(lParam), fontHeight + 4, TRUE);

        MoveWindow(hwndInject, 0, 3 * (fontHeight + 4), LOWORD(lParam) / 2, fontHeight + 4, TRUE);
        MoveWindow(hwndFree, LOWORD(lParam) / 2, 3 * (fontHeight + 4), LOWORD(lParam) / 2, fontHeight + 4, TRUE);
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDS_EDIT_CLASS_NAME:
            if (HIWORD(wParam) == EN_SETFOCUS) {
                setFocus(hwndClassName, 1, defaultClassName);
            }
            else if (HIWORD(wParam) == EN_KILLFOCUS) {
                setFocus(hwndClassName, 0, defaultClassName);
            }
            break;
        case IDS_EDIT_WINDOWS_NAME:
            if (HIWORD(wParam) == EN_SETFOCUS) {
                setFocus(hwndWindwosName, 1, defaultWindowName);
            }
            else if (HIWORD(wParam) == EN_KILLFOCUS) {
                setFocus(hwndWindwosName, 0, defaultWindowName);
            }
            break;
        case IDS_EDIT_DLL_NAME:
            if (HIWORD(wParam) == EN_SETFOCUS) {
                setFocus(hwndDllName, 1, defaultDllName);
            }
            else if (HIWORD(wParam) == EN_KILLFOCUS) {
                setFocus(hwndDllName, 0, defaultDllName);
            }
            break;
        case IDS_BUTTON_INJECT:
            if (HIWORD(wParam) == BN_CLICKED) {
                injectDLL();
            }
            break;
        case IDS_BUTTON_FREE:
            if (HIWORD(wParam) == BN_CLICKED) {
                freeDLL();
            }
            break;
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        return 0;
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

void injectDLL() {
    WCHAR errBuf[MAX_PATH];

    WCHAR className[MAX_PATH];
    if (GetWindowText(hwndClassName, className, sizeof(className) / sizeof(WCHAR)) == 0) {
        MessageBoxW(NULL, L"请输入 class name", L"错误", MB_OK);
        return;
    }
    WCHAR windowName[MAX_PATH];
    if (GetWindowText(hwndWindwosName, windowName, sizeof(windowName) / sizeof(WCHAR)) == 0) {
        MessageBoxW(NULL, L"请输入 window name", L"错误", MB_OK);
        return;
    }
    WCHAR dllName[MAX_PATH];
    if (GetWindowText(hwndDllName, dllName, sizeof(dllName) / sizeof(WCHAR)) == 0) {
        MessageBoxW(NULL, L"请输入 dll name", L"错误", MB_OK);
        return;
    }

    // 获取 DLL 全路径
    WCHAR dllPathBuf[MAX_PATH];
    DWORD pathLen = GetCurrentDirectoryW(sizeof(dllPathBuf) / sizeof(WCHAR) / 2, dllPathBuf);
    if (!pathLen) {
        wsprintfW(errBuf, L"获取当前路径失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }
    wsprintfW(dllPathBuf, L"%ls\\%ls", dllPathBuf, dllName);

    // 判断 DLL 文件是否存在
    WIN32_FIND_DATAW fileInfo;
    HANDLE findFile = FindFirstFileW(dllPathBuf, &fileInfo);
    if (findFile == INVALID_HANDLE_VALUE) {
        wsprintfW(errBuf, L"查找 \"%ls\" 文件失败: %d", dllPathBuf, GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }
    else {
        FindClose(findFile);
    }

    HMODULE moudleHandleAddr = GetModuleHandleA("Kernel32");
    if (NULL == moudleHandleAddr) {
        wsprintfW(errBuf, L"获取 Kernel32 模块失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    FARPROC loadLibraryAddr = GetProcAddress(moudleHandleAddr, "LoadLibraryW");
    if (NULL == loadLibraryAddr) {
        wsprintfW(errBuf, L"获取 LoadLibraryW 函数地址失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    HWND hwnd = FindWindowW(className, windowName);
    if (NULL == hwnd) {
        wsprintfW(errBuf, L"查找 \"%ls\" - \"%ls\" 窗口句柄失败: %d", className, windowName, GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    DWORD pId;
    DWORD tId = GetWindowThreadProcessId(hwnd, &pId);

    HANDLE remoteProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
    if (NULL == remoteProcess) {
        wsprintfW(errBuf, L"获取远程句柄失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    int len = lstrlenW(dllPathBuf) * 2 + sizeof(dllPathBuf);
    LPVOID remtoeAddr = VirtualAllocEx(remoteProcess, NULL, len, MEM_COMMIT, PAGE_READWRITE);
    if (NULL == remtoeAddr) {
        wsprintfW(errBuf, L"分配内存空间失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    SIZE_T writeLen;
    BOOL writeResult = WriteProcessMemory(remoteProcess, remtoeAddr, dllPathBuf, len, &writeLen);
    if (!writeResult) {
        wsprintfW(errBuf, L"写入内存数据失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    HANDLE thread = CreateRemoteThread(remoteProcess, NULL, NULL, LPTHREAD_START_ROUTINE(loadLibraryAddr), remtoeAddr, NULL, NULL);
    if (NULL == thread) {
        wsprintfW(errBuf, L"创建线程失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }
}

void freeDLL() {
    WCHAR errBuf[MAX_PATH];

    WCHAR className[MAX_PATH];
    if (GetWindowText(hwndClassName, className, sizeof(className) / sizeof(WCHAR)) == 0) {
        MessageBoxW(NULL, L"请输入 class name", L"错误", MB_OK);
        return;
    }
    WCHAR windowName[MAX_PATH];
    if (GetWindowText(hwndWindwosName, windowName, sizeof(windowName) / sizeof(WCHAR)) == 0) {
        MessageBoxW(NULL, L"请输入 window name", L"错误", MB_OK);
        return;
    }
    WCHAR dllName[MAX_PATH];
    if (GetWindowText(hwndDllName, dllName, sizeof(dllName) / sizeof(WCHAR)) == 0) {
        MessageBoxW(NULL, L"请输入 dll name", L"错误", MB_OK);
        return;
    }

    // 获取 DLL 全路径
    WCHAR dllPathBuf[MAX_PATH];
    DWORD pathLen = GetCurrentDirectoryW(sizeof(dllPathBuf) / sizeof(WCHAR) / 2, dllPathBuf);
    if (!pathLen) {
        wsprintfW(errBuf, L"获取当前路径失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }
    wsprintfW(dllPathBuf, L"%ls\\%ls", dllPathBuf, dllName);

    // 判断 DLL 文件是否存在
    WIN32_FIND_DATAW fileInfo;
    HANDLE findFile = FindFirstFileW(dllPathBuf, &fileInfo);
    if (findFile == INVALID_HANDLE_VALUE) {
        wsprintfW(errBuf, L"查找 \"%ls\" 文件失败: %d", dllPathBuf, GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }
    else {
        FindClose(findFile);
    }

    HMODULE moudleHandleAddr = GetModuleHandleA("Kernel32");
    if (NULL == moudleHandleAddr) {
        wsprintfW(errBuf, L"获取 Kernel32 模块失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    FARPROC freeLibraryAddr = GetProcAddress(moudleHandleAddr, "FreeLibrary");
    if (NULL == freeLibraryAddr) {
        wsprintfW(errBuf, L"获取 FreeLibrary 函数地址失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    HWND hwnd = FindWindowW(className, windowName);
    if (NULL == hwnd) {
        wsprintfW(errBuf, L"查找 \"%ls\" - \"%ls\" 窗口句柄失败: %d", className, windowName, GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    DWORD pId;
    DWORD tId = GetWindowThreadProcessId(hwnd, &pId);

    HANDLE remoteProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
    if (NULL == remoteProcess) {
        wsprintfW(errBuf, L"获取远程句柄失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    HMODULE hModules[1024];
    DWORD cbNeeded;
    BOOL result = EnumProcessModules(remoteProcess, hModules, sizeof(hModules), &cbNeeded);
    if (!result) {
        wsprintfW(errBuf, L"查找模块列表失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    WCHAR moduleNameBuf[MAX_PATH];
    HMODULE dllModule = NULL;
    for (int i = 0; i < cbNeeded / sizeof(HMODULE); ++i) {
        DWORD nameLen = GetModuleFileNameExW(remoteProcess, hModules[i], moduleNameBuf, sizeof(moduleNameBuf) / sizeof(WCHAR));
        if (nameLen == 0) {
            continue;
        }
        if (lstrcmpW(moduleNameBuf, dllPathBuf) == 0) {
            dllModule = hModules[i];
            break;
        }
    }

    if (NULL == dllModule) {
        wsprintfW(errBuf, L"未找到 \"%ls\" 模块", dllPathBuf);
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }

    HANDLE thread = CreateRemoteThread(remoteProcess, NULL, NULL, LPTHREAD_START_ROUTINE(freeLibraryAddr), dllModule, NULL, NULL);
    if (NULL == thread) {
        wsprintfW(errBuf, L"创建线程失败: %d", GetLastError());
        MessageBoxW(NULL, errBuf, L"错误", MB_OK);
        return;
    }
}
