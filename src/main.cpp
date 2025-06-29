#include "config.h"
#include "Util.h"
#include "GetWindowsInfo.h"
#include <shellapi.h>
#include "resource.h"

#define  WM_TRAYICON (WM_APP + 1)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ConsoleDebugger debugger;
NOTIFYICONDATAW nid = {};
HINSTANCE g_hInst; // Global instance handle

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nShowCmd)
{
    g_hInst = hInstance; // Store instance handle in our global variable
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
    //wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    //wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);

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

    //ShowWindow(hWnd, nShowCmd);
    //UpdateWindow(hWnd);

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
    GetCursorPos(&pt); // Get the mouse cursor position

    // Create the popup menu programmatically
    HMENU hMenu = CreatePopupMenu();
    if (hMenu) {
        InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, ID_TRAY_GETINFO, L"Get Window Info");
        InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, ID_TRAY_SETSIZE, L"Set Window Size");
        InsertMenuW(hMenu, -1, MF_SEPARATOR, 0, NULL);
        InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT, L"Exit");

        // Make "Get Window Info" the default item (bold)
        SetMenuDefaultItem(hMenu, ID_TRAY_GETINFO, FALSE);

        // Display the menu at the cursor's position
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
            // This code runs once when the hidden window is created.
            // We set up the NOTIFYICONDATA structure here.
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hWnd;
            nid.uID = 100; // A unique ID for the icon
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid.uCallbackMessage = WM_TRAYICON; // Our custom message
            nid.hIcon = (HICON)LoadImageW(g_hInst, MAKEINTRESOURCEW(IDI_MYICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
            if (nid.hIcon == NULL)
            {
                // If the custom icon fails, fall back to a default system icon.
                nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
                debugger.PrintErrorMsg(L"Failed to load custom icon from resources. Falling back to default icon.");
            }
            wcscpy_s(nid.szTip, L"UH Tools");

            // Add the icon to the system tray
            Shell_NotifyIconW(NIM_ADD, &nid);
            break;
        }
        case WM_TRAYICON:
        {
            // This block handles messages from the tray icon.
            // The actual mouse message is in lParam.
            switch (lParam)
            {
            case WM_RBUTTONUP:
                // If the user right-clicks, show the context menu.
                ShowContextMenu(hWnd);
                break;
            }
            break;
        }
    case WM_COMMAND:
        {
            // This block handles clicks on our popup menu items.
            switch (LOWORD(wParam))
            {
            case ID_TRAY_GETINFO:
                {
                    debugger.PrintErrorMsg(L"--- Getting Window Info ---");
                    uh_WindowInfoManager manager;
                    manager.SetAllWindowsInfo();
                    debugger.PrintErrorMsg((L"Found " + std::to_wstring(manager.GetWindowCount()) + L" windows.").c_str());

                    for (size_t i = 0; i < manager.GetWindowCount(); ++i)
                    {
                        debugger.PrintErrorMsg(manager.GetwindowInfo(i).c_str());
                    }
                    debugger.PrintErrorMsg(L"--- Finished getting window info ---");
                    break;
                }
            case ID_TRAY_SETSIZE:
                {
                    // Placeholder for your "Set window size" logic
                    MessageBoxW(hWnd, L"\"Set window size\" was clicked.", L"Info", MB_OK);
                    break;
                }
            case ID_TRAY_EXIT:
                {
                    // If "Exit" is clicked, destroy the window, which will quit the app.
                    DestroyWindow(hWnd);
                    break;
                }
            }
            break;
        }
        case WM_DESTROY:
            Shell_NotifyIconW(NIM_DELETE, &nid); // Remove the icon from the system tray
            PostQuitMessage(0);
            break;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}