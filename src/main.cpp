#include "config.h"
#include "Util.h"
#include "GetWindowsInfo.h"
#include <shellapi.h>
#include "resource.h"

#define  WM_TRAYICON (WM_APP + 1)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
uh_WindowInfoManager g_windowManager; // Make the manager global to persist state
NOTIFYICONDATAW nid = {};
HINSTANCE g_hInst; 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nShowCmd)
{
    g_hInst = hInstance; 

#ifdef MY_APP_DEBUG_MODE
    if (!debugger.IsValid())
    {
        debugger.PrintErrorMsg(L"Failed to initialize console debugger.");
        return -1;
    }
    debugger.PrintErrorMsg(L"Console debugger initialized successfully.");
#endif

    const wchar_t CLASS_NAME[] = L"UH_Tools";

    WNDCLASSW wc = { };
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassW(&wc))
    {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    HWND hWnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"UH Tools",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hWnd == NULL)
    {
        return 0;
    }

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void ShowContextMenu(HWND hWnd)
{
    POINT pt;
    GetCursorPos(&pt); 

    HMENU hMenu = CreatePopupMenu();
    if (hMenu) 
    {
        InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, ID_TRAY_GETINFO, L"Save Window Layout");
        InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, ID_TRAY_OPEN_LIST, L"Open Window List");
        InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, ID_TRAY_SETSIZE, L"Restore Window Layout");
        InsertMenuW(hMenu, -1, MF_SEPARATOR, 0, NULL);
        InsertMenuW(hMenu, -1, MF_SEPARATOR, 0, NULL);
        InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT, L"Exit");

        SetMenuDefaultItem(hMenu, ID_TRAY_GETINFO, FALSE);

        TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
        DestroyMenu(hMenu);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hWnd;
            nid.uID = 100; 
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid.uCallbackMessage = WM_TRAYICON; 
            nid.hIcon = (HICON)LoadImageW(g_hInst, MAKEINTRESOURCEW(IDI_MYICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
            if (nid.hIcon == NULL)
            {
                nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
                debugger.PrintErrorMsg(L"Failed to load custom icon from resources. Falling back to default icon.");
            }
            wcscpy_s(nid.szTip, L"UH Tools");

            Shell_NotifyIconW(NIM_ADD, &nid);
            break;
        }
        case WM_TRAYICON:
        {
            switch (lParam)
            {
            case WM_RBUTTONUP:
                ShowContextMenu(hWnd);
                break;
            }
            break;
        }
    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case ID_TRAY_GETINFO:
                {
                    debugger.PrintErrorMsg(L"--- Saving Current Window Layout ---");
                    const size_t windowCount = g_windowManager.RefreshWindowsList();
                    if(g_windowManager.SaveWindowsListToFile())
                    {
                        debugger.PrintErrorMsg((L"Saved " + std::to_wstring(windowCount) + L" windows.").c_str());
                    }
                    else
                    {
                        debugger.PrintErrorMsg(L"Failed to save window layout.");
                    }
                    debugger.PrintErrorMsg(L"--- Layout Saved ---");
                    break;
                }
            case ID_TRAY_OPEN_LIST:
                {
                    debugger.PrintErrorMsg(L"--- Opening window_list.txt ---");

                    wchar_t exePathBuf[MAX_PATH] = { 0 };
                    GetModuleFileNameW(NULL, exePathBuf, MAX_PATH);

                    std::wstring exePath(exePathBuf);
                    size_t last_slash_idx = exePath.find_last_of(L"\\/");
                    std::wstring dirPath;
                    if (std::wstring::npos != last_slash_idx)
                    {
                        dirPath = exePath.substr(0, last_slash_idx + 1);
                    }
                    std::wstring filePath = dirPath + L"windows_list.txt";

                    // Use ShellExecuteW to open the file with the default associated program.
                    HINSTANCE result = ShellExecuteW(NULL, L"open", filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    if ((INT_PTR)result <= 32) {
                        debugger.PrintErrorMsg((L"Failed to open file: " + filePath).c_str());
                    }
                    break;
                }
            case ID_TRAY_SETSIZE:
                {
                    debugger.PrintErrorMsg(L"--- Restoring Window Layout ---");
                    g_windowManager.RefreshWindowsList();
                    g_windowManager.ArrangeWindows();
                    debugger.PrintErrorMsg(L"--- Layout Restored ---");
                    break;
                }
            case ID_TRAY_EXIT:
                {
                    DestroyWindow(hWnd);
                    break;
                }
            }
            break;
        }
        case WM_DESTROY:
            Shell_NotifyIconW(NIM_DELETE, &nid); 
            PostQuitMessage(0);
            break;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}