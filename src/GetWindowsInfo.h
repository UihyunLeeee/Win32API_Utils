#include <windows.h>
#include <vector>
#include <string>

using namespace std;

class WindowInfo
{
private:
    HWND m_hwnd;                // The window's unique handle
    std::wstring m_windowTitle; // The window's title text
    RECT m_windowRect;          // The window's position and size

public:
    // Constructor: Takes a window handle and gathers all information about it.
    WindowInfo(HWND hwnd);

    // Getter Functions: Safely access the private member variables.
    HWND GetHandle() const;
    std::wstring GetTitle() const;
    RECT GetRect() const;
};

class WindowInfoManager
{
private:
    std::vector<WindowInfo> m_windows; 
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

public:
    WindowInfoManager();
    const std::vector<WindowInfo>& GetAllWindows() const;
};
