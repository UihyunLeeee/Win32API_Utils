#include "Util.h"
#include "GetWindowsInfo.h"
#include "psapi.h"
#include <fstream>
#include <codecvt>
#include <algorithm>
#include <locale>
#include <sstream>  

uh_WindowInfo::uh_WindowInfo(HWND hwnd) : 
    m_hwnd(hwnd), m_executeFile(L""), 
    m_windowRect({0, 0, 0, 0}), m_windowTitle(L"")
{
    if (!IsWindow(hwnd))
    {
        return;
    }

    /*** m_windowaTitle ***/
    const int titleLength = GetWindowTextLengthW(hwnd);
    if(titleLength > 0)
    {
        vector<wchar_t> titleBuffer(titleLength + 1);
        GetWindowTextW(hwnd, titleBuffer.data(), titleLength + 1);
        m_windowTitle = std::wstring(titleBuffer.data());
    }

    /*** m_executeFile ***/
    
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    if (processId != 0)
    {
        HANDLE hProcess = 
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);

        if (hProcess != NULL)
        {
            wchar_t processFilePath[MAX_PATH] = {0};
            if (GetModuleFileNameExW(hProcess, NULL, processFilePath, MAX_PATH) > 0)
            {
                m_executeFile = processFilePath;
            }
            CloseHandle(hProcess);
        }
    }
    
    /*** m_windowRect ***/  

    GetWindowRect(hwnd, &m_windowRect);
}

void uh_WindowInfo::SetPosition(int x, int y, int width, int height)
{
    // It's good practice to check if the handle is still valid before using it.
    if (m_hwnd && IsWindow(m_hwnd))
    {
        // Use SWP_NOZORDER to keep the window's z-order and SWP_NOACTIVATE to not steal focus.
        SetWindowPos(m_hwnd, NULL, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

std::wstring uh_WindowInfo::GetWindowTitle() const
{
    return m_windowTitle;
}

std::wstring uh_WindowInfo::GetExecuteFile() const
{
    return m_executeFile;
}

RECT uh_WindowInfo::GetRect() const
{
    return m_windowRect;
}

HWND uh_WindowInfo::GetHwnd() const
{
    return m_hwnd;
}

/****************************************************** */

/********Function for Saving Windows List to File *********/
BOOL CALLBACK uh_WindowInfoManager::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    vector<uh_WindowInfo>* pWindows = 
        reinterpret_cast<vector<uh_WindowInfo>*>(lParam); 

    if (pWindows == nullptr)
    {
        return FALSE;
    }

    if (!IsWindowVisible(hwnd) || GetWindowTextLengthW(hwnd) == 0 ||
        (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) ||
        (GetWindow(hwnd, GW_OWNER) != (HWND)0))
    {
        return TRUE;
    }

    uh_WindowInfo newWindow(hwnd);
    const std::wstring &exePath = newWindow.GetExecuteFile();

    if (exePath.empty())
    {
        return TRUE; 
    }

    if (std::any_of(pWindows->cbegin(), pWindows->cend(),
                    [&exePath](const uh_WindowInfo &w)
                    { return w.GetExecuteFile() == exePath; }))
    {
        return TRUE;
    }

    pWindows->push_back(std::move(newWindow));

    return TRUE;
}

size_t uh_WindowInfoManager::RefreshWindowsList()
{
    /**** Get Window Information ****/
    m_windows.clear();
    EnumWindows(EnumWindowsProc, (LPARAM)(&m_windows));
    return m_windows.size();
}

bool uh_WindowInfoManager::SaveWindowsListToFile() 
{
    /*******  First, get the path to the application's directory. *******/

    wchar_t exePathBuf[MAX_PATH] = {0};
    GetModuleFileNameW(NULL, exePathBuf, MAX_PATH);
 
    std::wstring exePath(exePathBuf);
    size_t last_slash_idx = exePath.find_last_of(L"\\/");
    std::wstring dirPath;
    if (std::wstring::npos != last_slash_idx)
    {
        dirPath = exePath.substr(0, last_slash_idx + 1);
    }
    std::wstring filepath = dirPath + L"windows_list.txt";

    /**** Write Window Information to File ****/

    std::wofstream outFile(filepath.c_str(), std::ios::out | std::ios::binary);
    if (outFile.is_open())
    {
        outFile.imbue(std::locale(outFile.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));

        for (size_t i = 0; i < GetWindowCount(); ++i)
        {
            const std::wstring windowInfo = GetwindowInfo(i);
            outFile << windowInfo;
            debugger.PrintErrorMsg(windowInfo.c_str());
        }
        debugger.PrintErrorMsg((L"Windows list saved to: " + filepath).c_str());
    }
    else
    {
        debugger.PrintErrorMsg((L"Failed to open file for writing: " + filepath).c_str());
        return false;
    }
    outFile.close();

    /**** Clear Window Informaion ****/
    m_windows.clear();

    return true;
}

/************* Functions for Arranging Windows ***************/ 

struct SavedWindowLayout
{
    std::wstring exePath;
    std::wstring title;
    RECT rect;
};

size_t uh_WindowInfoManager::ArrangeWindows()
{
     debugger.PrintErrorMsg(L"--- Starting Window Arrangement ---");

    /******  Part 1: Find and Read the Layout File ******/
    // First, get the path to the application's directory, 
    // same as in RefreshWindowsList.
    wchar_t exePathBuf[MAX_PATH] = {0};
    GetModuleFileNameW(NULL, exePathBuf, MAX_PATH);
    std::wstring exePath(exePathBuf);
    size_t last_slash_idx = exePath.find_last_of(L"\\/");
    std::wstring dirPath;
    if (std::wstring::npos != last_slash_idx)
    {
        dirPath = exePath.substr(0, last_slash_idx + 1);
    }
    std::wstring filepath = dirPath + L"windows_list.txt";

    /**** Part 2: Parse the File and Store Layouts ****/
    std::vector<SavedWindowLayout> savedLayouts;
    std::wifstream inFile(filepath.c_str());
    if (!inFile.is_open())
    {
        debugger.PrintErrorMsg((L"ArrangeWindows: Could not open file: " + filepath).c_str());
        return 0;
    }

    // Set the locale to correctly read the UTF-8 file we wrote.
    inFile.imbue(std::locale(inFile.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>));

    std::wstring line;
    SavedWindowLayout currentLayout = {};
    while (std::getline(inFile, line))
    {
        // Skip empty lines, which can happen between entries.
        if (line.empty() || line == L"\r") continue;

        std::wstringstream ss(line);
        std::wstring key;
        ss >> key;

        debugger.PrintErrorMsg((L"ArrangeWindows: Processing line: " + line).c_str());
        debugger.PrintErrorMsg(ss.str().c_str());
        debugger.PrintErrorMsg(L"\n");


        if (key == L"Executable:") {
            std::getline(ss, currentLayout.exePath);
            debugger.PrintErrorMsg((L"ArrangeWindows: Found Executable: " + currentLayout.exePath).c_str()); 
            currentLayout.exePath.erase(0, currentLayout.exePath.find_first_not_of(L" \t"));
            debugger.PrintErrorMsg((L"ArrangeWindows: Executable after trim: " + currentLayout.exePath).c_str());
        }
        else if (key == L"Title:") {
            std::getline(ss, currentLayout.title);
            currentLayout.title.erase(0, currentLayout.title.find_first_not_of(L" \t"));
        }
        else if (key == L"Position:") {
            wchar_t dummy;
            ss >> dummy >> currentLayout.rect.left >> dummy >> currentLayout.rect.top >> dummy;
        }
        else if (key == L"Size:") {
            long width = 0, height = 0;
            wchar_t dummy;
            ss >> dummy >> width >> dummy >> height >> dummy;
            currentLayout.rect.right = currentLayout.rect.left + width;
            currentLayout.rect.bottom = currentLayout.rect.top + height;

            // This marks the end of a block, so we save the layout.
            savedLayouts.push_back(currentLayout);
            currentLayout = {}; // Reset for the next entry.
        }
    }

    inFile.close();
    debugger.PrintErrorMsg((L"ArrangeWindows: Loaded " + std::to_wstring(savedLayouts.size()) + L" layouts from file.").c_str());

    /*check data*/
    for (const auto& layout : savedLayouts)
    {
        debugger.PrintErrorMsg((L"Executable: " + layout.exePath + L", Title: " + layout.title +
                               L", Rect: (" + std::to_wstring(layout.rect.left) + L", " +
                               std::to_wstring(layout.rect.top) + L", " +
                               std::to_wstring(layout.rect.right) + L", " +
                               std::to_wstring(layout.rect.bottom) + L")").c_str());
    }   
    debugger.PrintErrorMsg(L"--- Finished Loading Layouts ---");   

    for(const auto& layout : m_windows)
    {
        debugger.PrintErrorMsg((L"Current Window: " + layout.GetExecuteFile() + L", Title: " + layout.GetWindowTitle() +
                               L", Rect: (" + std::to_wstring(layout.GetRect().left) + L", " +
                               std::to_wstring(layout.GetRect().top) + L", " +
                               std::to_wstring(layout.GetRect().right) + L", " +
                               std::to_wstring(layout.GetRect().bottom) + L")").c_str());
    }
    debugger.PrintErrorMsg(L"--- Finished Loading Windows ---");


    // --- Part 3: Find Currently Running Windows and Move Them ---
    // Get a fresh list of all currently visible windows.

    size_t arrangedCount = 0;
    // Loop through each currently running window.
    for (auto& window : m_windows)
    {
        // Loop through each layout we loaded from the file.
        for (auto& layout : savedLayouts)
        {
            // Match based on BOTH executable path (case-insensitive) AND window title (case-sensitive).
            bool exeMatch = (_wcsicmp(window.GetExecuteFile().c_str(), layout.exePath.c_str()) == 0);
            bool titleMatch = (window.GetWindowTitle() == layout.title);
#if 0
            debugger.PrintErrorMsg(window.GetExecuteFile().c_str());
            debugger.PrintErrorMsg(L" / ");
            debugger.PrintErrorMsg(layout.exePath.c_str());
            debugger.PrintErrorMsg(L"\n");
            debugger.PrintErrorMsg(window.GetWindowTitle().c_str());
            debugger.PrintErrorMsg(L" / ");
            debugger.PrintErrorMsg(layout.title.c_str());
            debugger.PrintErrorMsg(L"\n");
#endif

            //if (exeMatch && titleMatch)
            if (exeMatch && titleMatch)
            {
                debugger.PrintErrorMsg((L"Arranging window for: " + layout.exePath).c_str());
                
                const RECT& rect = layout.rect;
                int width = rect.right - rect.left;
                int height = rect.bottom - rect.top;

                window.SetPosition(rect.left, rect.top, width, height);
                arrangedCount++;

                // We've arranged this window, so we can break the inner loop and move to the next live window.
                break;
            }
        }
    }
    debugger.PrintErrorMsg((L"ArrangeWindows: Finished. Arranged " + std::to_wstring(arrangedCount) + L" windows.").c_str());
    return arrangedCount;

    return 0;
}

/************* Functions Print Window Information ***************/

std::wstring uh_WindowInfoManager::GetwindowInfo(size_t index) const
{
    // size_t is unsigned, so a check for < 0 is redundant and can be removed.
    if (index >= m_windows.size())
    {
        return L"Invalid index";
    }

    const uh_WindowInfo& window = m_windows[index];
    std::wstring info = L"\nExecutable: " + window.GetExecuteFile() +
                        L"\nTitle: " + window.GetWindowTitle() +
                        L"\nPosition: (" + std::to_wstring(window.GetRect().left) + L", " +
                        std::to_wstring(window.GetRect().top) + L")" +
                        L"\nSize: (" + std::to_wstring(window.GetRect().right - window.GetRect().left) + L", " +
                        std::to_wstring(window.GetRect().bottom - window.GetRect().top) + L")" + L"\n";
    return info;
}

size_t uh_WindowInfoManager::GetWindowCount() const
{
    return m_windows.size();
}