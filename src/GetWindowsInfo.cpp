#include "GetWindowsInfo.h"

WindowInfo::WindowInfo(HWND hwnd) : m_hwnd(hwnd)
{
    m_windowRect = {0, 0, 0, 0};

    const int titleLength = GetWindowTextLengthW(m_hwnd);
    if (titleLength > 0)
    {
        std::vector<wchar_t> buffer(titleLength + 1);
        GetWindowTextW(m_hwnd, buffer.data(), titleLength + 1);
        m_windowTitle = buffer.data();
    }
    else
    {
        m_windowTitle = L"";
    }

    GetWindowRect(m_hwnd, &m_windowRect);
}

HWND WindowInfo::GetHandle() const
{
    return m_hwnd;
}

std::wstring WindowInfo::GetTitle() const
{
    return m_windowTitle;
}

RECT WindowInfo::GetRect() const
{
    return m_windowRect;
}

/****************************************************** */

WindowInfoManager::WindowInfoManager()
{
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
}

BOOL CALLBACK WindowInfoManager::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    WindowInfoManager* pManager = reinterpret_cast<WindowInfoManager*>(lParam);

    if (pManager)
    {
        const int titleLength = GetWindowTextLengthW(hwnd);
        if (IsWindowVisible(hwnd) && titleLength > 0)
        {
            pManager->m_windows.emplace_back(hwnd);
        }
    }

    return TRUE;
}

const std::vector<WindowInfo>& WindowInfoManager::GetAllWindows() const
{
    return m_windows;
}
