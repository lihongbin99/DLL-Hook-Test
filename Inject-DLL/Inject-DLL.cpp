#include <iostream>
#include <windows.h>

BOOL injectDLL(PCWCH className, PCWCH windowName, PCWCH DLLPath);

int main() {
    setlocale(LC_ALL, "chs");
    BOOL isSuccess = injectDLL(L"DLL-Hook", L"Dll-Hook-WinMain", L".\\DLL.dll");
    if (isSuccess) {
        wprintf(L"注入成功\n");
    } else {
        wprintf(L"注入失败\n");
    }

    wprintf(L"请输入任意字符退出程序\n");
    std::cin.get();
}

BOOL injectDLL(PCWCH className, PCWCH windowName, PCWCH DLLPath) {
    HMODULE moudleHandleAddr = GetModuleHandleA("Kernel32");
    if (NULL == moudleHandleAddr) {
        wprintf(L"获取 Kernel32 地址失败\n");
        return FALSE;
    }
    wprintf(L"Kernel32地址: 0x%p\n", moudleHandleAddr);

    FARPROC loadLibraryAddr = GetProcAddress(moudleHandleAddr, "LoadLibraryW");
    if (NULL == loadLibraryAddr) {
        wprintf(L"获取 LoadLibraryW 函数地址失败\n");
        return FALSE;
    }
    wprintf(L"LoadLibraryW函数地址: 0x%p\n", loadLibraryAddr);

    HWND hwnd = FindWindowW(className, windowName);
    if (NULL == hwnd) {
        wprintf(L"未找到窗口\n");
        return FALSE;
    }
    wprintf(L"窗口句柄: 0x%p\n", hwnd);

    DWORD pId;
    DWORD tId = GetWindowThreadProcessId(hwnd, &pId);
    wprintf(L"进程句柄: 0x%08X\n", pId);
    wprintf(L"线程句柄: 0x%08X\n", tId);

    HANDLE remoteProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);
    if (NULL == remoteProcess) {
        wprintf(L"未获取到远程句柄\n");
        return FALSE;
    }
    wprintf(L"远程句柄: 0x%p\n", remoteProcess);

    int len = (lstrlenW(DLLPath) + 1) * 2;
    LPVOID remtoeAddr = VirtualAllocEx(remoteProcess, NULL, len, MEM_COMMIT, PAGE_READWRITE);
    if (NULL == remtoeAddr) {
        wprintf(L"分配内存空间失败\n");
        return FALSE;
    }
    wprintf(L"分配内存空间地址为: 0x%p\n", remtoeAddr);
    
    SIZE_T writeLen;
    BOOL writeResult = WriteProcessMemory(remoteProcess, remtoeAddr, DLLPath, len, &writeLen);
    if (!writeResult) {
        wprintf(L"写入内存失败\n");
        return FALSE;
    }
    wprintf(L"写入路径: %ls\n", DLLPath);
    wprintf(L"写入长度: %d\n", writeLen);

    HANDLE thread = CreateRemoteThread(remoteProcess, NULL, NULL, LPTHREAD_START_ROUTINE(loadLibraryAddr), remtoeAddr, NULL, NULL);
    if (NULL == thread) {
        wprintf(L"创建线程失败\n");
        return FALSE;
    }
    wprintf(L"线程地址: 0x%p\n", thread);

    return TRUE;
}
