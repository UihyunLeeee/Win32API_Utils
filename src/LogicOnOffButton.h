#pragma once
#include <windows.h>

class LogicOnOffButton
{
private:
    HWND m_hWnd;
    bool m_bIsOn;
    int m_nControlId;
    static int s_nNextControlId;

public:
    static HFONT s_hFont;
    static void InitFont();
    static void CleanupFont();

    LogicOnOffButton();

    void Create(HWND hParent, HINSTANCE hInst, const wchar_t* text);
    void Resize(int x, int y, int w, int h);

    void ToggleState();
    bool IsOn() const;
    int GetId() const;
    HWND GetHwnd() const;

    LRESULT HandleCtlColor(HDC hdc);
};
