#pragma once
#include <windows.h>

class HookManager
{
private:
    static HWND m_appWindow;
    static HHOOK m_keyboardHook;
    static LRESULT CALLBACK LowLevelKeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

public:
    HookManager();
    ~HookManager();

    void InstallHook(HWND hWnd);
    void UninstallHook();
};