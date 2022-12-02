// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

DWORD textAddr = 0x00D92188;
DWORD hookAddr = 0x00D91167;
DWORD hookRet = hookAddr + 5;
DWORD hookOverFun = 0x00D91220;

void HookAnyAddress(DWORD dwHookAddr, LPVOID dwJmpAddress) {
    BYTE jmpCode[5] = { 0 };
    jmpCode[0] = 0xE9;
    *(DWORD*)&jmpCode[1] = (DWORD)dwJmpAddress - dwHookAddr - 5;

    DWORD OldProtext = 0;
    VirtualProtect((LPVOID)dwHookAddr, 5, PAGE_EXECUTE_READWRITE, &OldProtext);
    memcpy((void*)dwHookAddr, jmpCode, 5);
    VirtualProtect((LPVOID)dwHookAddr, 5, OldProtext, &OldProtext);
}

void WINAPI HookFun(DWORD num) {
    WCHAR buf[MAX_PATH];
    wsprintfW(buf, L"ecx: %d", num);
    MessageBoxW(NULL, buf, L"Hook", MB_OK);
}

__declspec(naked) void HookTest() {
    __asm
    {
        pushad;
        push ecx;
        call HookFun;
        popad;
        call hookOverFun;
        jmp hookRet;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    DWORD OldProtext = 0;
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        MessageBoxW(NULL, L"注入成功", L"提示", MB_OK);

        // 测试读取
        // MessageBoxW(NULL, text, L"提示2", MB_OK);

        // 测试写入
        // VirtualProtect((LPVOID)textAddr, 6, PAGE_EXECUTE_READWRITE, &OldProtext);
        // memcpy((void*)textAddr, L"abc", 6);
        // VirtualProtect((LPVOID)textAddr, 6, OldProtext, &OldProtext);
        // MessageBoxW(NULL, text, L"提示3", MB_OK);

        // 测试 Hook
        // HookAnyAddress(hookAddr, HookTest);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        MessageBoxW(NULL, L"卸载成功", L"提示", MB_OK);
        break;
    }
    return TRUE;
}

