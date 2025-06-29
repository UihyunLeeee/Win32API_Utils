#include "GetWindowsInfo.h"
#include "psapi.h"

uh_WindowInfo::uh_WindowInfo(HWND hwnd) : m_hwnd(hwnd), m_executeFile(L""), m_windowRect({0, 0, 0, 0})
{
    // First, ensure the window handle is still valid to prevent race conditions.
    if (!IsWindow(m_hwnd))
    {
        return;
    }
    
    // Get the process ID associated with the window.
    DWORD processId = 0;
    GetWindowThreadProcessId(m_hwnd, &processId);

    // If we have a valid process ID, try to get the executable path.
    if (processId != 0)
    {
        // Use NULL with Win32 API functions for maximum compatibility.
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
        if (hProcess != NULL)
        {
            wchar_t processFilePath[MAX_PATH] = {0};
            if (GetModuleFileNameExW(hProcess, NULL, processFilePath, MAX_PATH) > 0)
            {
                m_executeFile = processFilePath;
            }
            CloseHandle(hProcess);
        }
        // If OpenProcess fails (e.g., for a protected system process), m_executeFile will remain empty.
    }
    // Always get the window rectangle, even if getting the path fails.
    GetWindowRect(m_hwnd, &m_windowRect);
}

HWND uh_WindowInfo::GetHandle() const
{
    return m_hwnd;
}

std::wstring uh_WindowInfo::GetExecuteFile() const
{
    return m_executeFile;
}

RECT uh_WindowInfo::GetRect() const
{
    return m_windowRect;
}

/****************************************************** */

BOOL CALLBACK uh_WindowInfoManager::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    uh_WindowInfoManager* pManager = reinterpret_cast<uh_WindowInfoManager*>(lParam);

    if (pManager == nullptr)
    {
        return FALSE;
    }

    if (!IsWindowVisible(hwnd) || GetWindowTextLengthW(hwnd) == 0 ||
        (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) ||
        (GetWindow(hwnd, GW_OWNER) != (HWND)0))
    {
        return TRUE;
    }

    pManager->m_windows.emplace_back(hwnd);

    return TRUE;
} 

std::wstring uh_WindowInfoManager::GetwindowInfo(size_t index) const
{
    // size_t is unsigned, so a check for < 0 is redundant and can be removed.
    if (index >= m_windows.size())
    {
        return L"Invalid index";
    }

    const uh_WindowInfo& window = m_windows[index];
    std::wstring info = L"Handle: " + std::to_wstring(reinterpret_cast<uintptr_t>(window.GetHandle())) +
                        L"\nExecutable: " + window.GetExecuteFile() +
                        L"\nPosition: (" + std::to_wstring(window.GetRect().left) + L", " +
                        std::to_wstring(window.GetRect().top) + L")" +
                        L"\nSize: (" + std::to_wstring(window.GetRect().right - window.GetRect().left) + L", " +
                        std::to_wstring(window.GetRect().bottom - window.GetRect().top) + L")" + L"\n";
    return info;
}