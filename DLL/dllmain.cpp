// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    WCHAR* text = (WCHAR*)0x00632180;
    DWORD OldProtext = 0;
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:

        // 测试读取
        // MessageBoxW(NULL, text, L"提示2", MB_OK);

        // 测试写入
        VirtualProtect((LPVOID)text, 6, PAGE_EXECUTE_READWRITE, &OldProtext);
        memcpy((void*)text, L"456", 6);
        VirtualProtect((LPVOID)text, 6, OldProtext, &OldProtext);
        // MessageBoxW(NULL, text, L"提示3", MB_OK);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

