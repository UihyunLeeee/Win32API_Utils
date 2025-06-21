#include <windows.h>
#include <vector>
#include <string>

using namespace std;

class GetWindowsInfo
{
private:
    HWND hWnd;
    wstring windowTitle;
    RECT windowRect;
public:
    GetWindowsInfo(HWND hwnd) : hWnd(hwnd)
    {
        const int titleLength = GetWindowTextLengthW(hWnd);
        if (titleLength > 0)
        {
            vector<wchar_t> buffer(titleLength + 1);
            GetWindowTextW(hWnd, buffer.data(), titleLength + 1);
            windowTitle = wstring(buffer.data());
        }
        else
        {
            windowTitle = L"";
        }

        GetWindowRect(hWnd, &windowRect);
    }

    wstring GetTitle() const { return windowTitle; }
    RECT GetRect() const { return windowRect; }
};

class GetAllWindowsInfo
{
private:
    vector<GetWindowsInfo> windowsInfo;
public:
    GetAllWindowsInfo()
    {
        EnumWindows(EnumWindowsProc, NULL);
    }

    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
    {   
        GetWindowsInfo windowInfo(hwnd);
        windowsInfo.push_back(windowInfo);
        return TRUE;
    }   

    const vector<GetWindowsInfo>& GetWindows() const { return windowsInfo; }
};