#include <windows.h>
#include <vector>
#include <string>

using namespace std;

class uh_WindowInfo
{
private:
    HWND m_hwnd;                
    RECT m_windowRect;          
    std::wstring m_executeFile; 
    std::wstring m_windowTitle; 

public:
    uh_WindowInfo(HWND hwnd);

    void SetPosition(int x, int y, int width, int height);
    HWND GetHwnd() const;
    RECT GetRect() const;
    std::wstring GetExecuteFile() const;
    std::wstring GetWindowTitle() const;
};

class uh_WindowInfoManager
{
private:
    std::vector<uh_WindowInfo> m_windows;
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

public:
    /*** Functions for Saving Window Information ***/
    size_t RefreshWindowsList();
    bool SaveWindowsListToFile();

    /*** Function for Arranging Windows ***/    
    size_t ArrangeWindows();

    /*** Function for Print window information ****/
    std::wstring GetwindowInfo(size_t index) const;
    size_t GetWindowCount() const;
};
