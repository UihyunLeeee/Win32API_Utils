#pragma once
#include <windows.h>
#include <string>

namespace uhConsole
{
    // These are declared here with 'extern' and defined in uhConsole.cpp
    extern HWND hConsoleOutput;
    extern HWND hConsoleInput;
    extern WNDPROC pfnOrigInputProc;

    void CreateConsoleTab(HWND hPage, HINSTANCE ghInst);
    void AppendTextToConsole(const wchar_t *text);
}   // namespace uhConsole