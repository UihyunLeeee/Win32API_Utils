#include <windows.h>
#include <vector>
#include <string>

using namespace std;

class uh_WindowInfo
{
private:
    HWND m_hwnd;                // The window's unique handle
    std::wstring m_executeFile; // The path to the executable file
    RECT m_windowRect;          // The window's position and size

public:
    uh_WindowInfo(HWND hwnd);

    HWND GetHandle() const;
    std::wstring GetExecuteFile() const;
    RECT GetRect() const;
};

class uh_WindowInfoManager
{
private:
    std::vector<uh_WindowInfo> m_windows;
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

public:
    size_t SetAllWindowsInfo()
    {
        m_windows.clear();
        EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(this));
        return m_windows.size();
    }
    std::vector<uh_WindowInfo> GetAllWindows() const { return m_windows; }
    std::wstring GetwindowInfo(size_t index) const;
    size_t GetWindowCount() const { return m_windows.size(); }
};
