#include "config.h"
#include <commctrl.h> // For Tab Controls

#include <string> // For std::wstring

#include "SpeedOffset.h"
#include "Monitoring.h"
#include "uhConsole.h"

// Main Winddow Function Declarations 
HINSTANCE ghInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnNotify(HWND hwnd, LPARAM lParam);

// Function to create the tab control and its pages
HWND CreateTabControl(HWND hWndParent, HINSTANCE hInst);

// Global Variables
HWND g_hTab; // Handle to the tab control

const int NUM_TABS = 5;
HWND hPageMontoring, hPageSpeedOffset, hPageAntiRoll;
HWND hPageAntiDive, hPageAntiSquat; 
HWND g_hCurrentPage; 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nShowCmd)
{
    ghInst = hInstance;

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);

    if (!RegisterClassW(&wc))
    {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error",
            MB_ICONERROR | MB_OK);
        return 0;
    }

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Tab Window Application", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, // Start with FHD size
        NULL, NULL, hInstance, NULL);

    if (hWnd == NULL)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error",
            MB_ICONERROR | MB_OK);
        return 0;
    }

    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        {
            // Create the tab control and pages
            g_hTab = CreateTabControl(hWnd, ghInst);
            if (!g_hTab) {
                MessageBoxW(hWnd, L"Tab Control Creation Failed!", L"Error",
                    MB_ICONERROR | MB_OK);
                return -1;
            }

            // Create page containers (simple STATIC windows)
            hPageMontoring = Monitoring::CreateControlTab(hPageMontoring, ghInst);
            hPageSpeedOffset = CreateWindowExW(0, L"STATIC", 
                NULL, WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWnd, NULL, ghInst, NULL);
            hPageAntiRoll   = CreateWindowExW(0, L"STATIC", 
                NULL, WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWnd, NULL, ghInst, NULL);
            hPageAntiDive   = CreateWindowExW(0, L"STATIC", 
                NULL, WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWnd, NULL, ghInst, NULL);
            hPageAntiSquat  = CreateWindowExW(0, L"STATIC", 
                NULL, WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWnd, NULL, ghInst, NULL);

            // Create the content for the pages.
            SpeedOffset::CreateControlTab(hPageSpeedOffset, ghInst);

            g_hCurrentPage = hPageMontoring; 
            ShowWindow(g_hCurrentPage, SW_SHOW);

            // Create the console window, parented to the main window.
            // It will be positioned in OnSize.
            uhConsole::CreateConsoleTab(hWnd, ghInst);
            if(!uhConsole::hConsoleOutput || !uhConsole::hConsoleInput )
            {
                MessageBoxW(hWnd, L"Console Creation Failed!", L"Error",
                    MB_ICONERROR | MB_OK);
                return -1;
            }
            uhConsole::AppendTextToConsole(L"Console Debugger is initialized successfully.\r\n");
        }
        return 0;

    case WM_SIZE:
        OnSize(hWnd, (UINT)wParam, LOWORD(lParam), HIWORD(lParam));
        return 0;

    case WM_NOTIFY:
        OnNotify(hWnd, lParam);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

HWND CreateTabControl(HWND hWndParent, HINSTANCE hInst)
{
    RECT rcClient;
    GetClientRect(hWndParent, &rcClient);

    HWND hWndTab = CreateWindowExW(
        0, WC_TABCONTROLW, L"",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        0, 0, rcClient.right, rcClient.bottom,
        hWndParent, NULL, hInst, NULL);

    if (!hWndTab) 
        return NULL;

    TCITEMW tie = {};
    tie.mask = TCIF_TEXT;

    const wchar_t* tabLabels[] = 
        {L"Current Monitoring", L"Speed Offset", L"Anti Roll", L"Anti Dive", L"Anti Squat"};

    for (int i = 0; i < NUM_TABS; ++i)
    {
        tie.pszText = (LPWSTR)tabLabels[i];
        SendMessageW(hWndTab, TCM_INSERTITEMW, (WPARAM)i, (LPARAM)&tie);
    }

    return hWndTab;
}

void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    if (g_hTab)
    {
        // Define the layout split: 80% for tabs, 20% for console.
        const int console_area_h = (int)(cy * 0.20);
        const int tab_area_h = cy - console_area_h;

        /************ 1. Resize Tab Area and its Contents ***********/
        // Resize the tab control itself to occupy the top part of the window.
        MoveWindow(g_hTab, 0, 0, cx, tab_area_h, TRUE);

        // Calculate the display area for the tab pages, inside the tab control.
        RECT rcTab;
        GetClientRect(g_hTab, &rcTab);
        SendMessage(g_hTab, TCM_ADJUSTRECT, FALSE, (LPARAM)&rcTab);

        // Resize the current page container to fit the tab display area.
        if (g_hCurrentPage) 
        {
            MoveWindow(g_hCurrentPage, rcTab.left, rcTab.top,
                       rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, TRUE);
        }

        // Get the dimensions of the page content area to pass to the page-specific resize function.
        RECT rcPage;
        if (g_hCurrentPage) 
        {
            GetClientRect(g_hCurrentPage, &rcPage);
        } 
        else 
        {
            SetRectEmpty(&rcPage);
        }
        int page_w = rcPage.right - rcPage.left;
        int page_h = rcPage.bottom - rcPage.top;

        // Resize the content within the current tab page.
        if (g_hCurrentPage == hPageMontoring) 
        {
            Monitoring::ReSizeWindow(page_w, page_h);
        }
        else if (g_hCurrentPage == hPageSpeedOffset) 
        {
            SpeedOffset::ReSizeWindow(page_w, page_h);
        }

        /************ 2. Resize Console Area ************/
        const int inputHeight = 30;
        const int padding = 5;
        int console_top_y = tab_area_h;
        
        int output_h = console_area_h - inputHeight - padding;
        if (output_h < 0) output_h = 0;

        // Use rcTab (calculated for the tab control's display area)
        // to align the console with the tab page content area.
        const int console_x = rcTab.left;
        const int console_w = rcTab.right - rcTab.left;

        // The console controls are children of the main window (hwnd), 
        // so coordinates are relative to it.
        MoveWindow(uhConsole::hConsoleOutput, 
            console_x, console_top_y, 
            console_w, output_h, TRUE);
        MoveWindow(uhConsole::hConsoleInput, 
            console_x + padding, console_top_y + output_h + padding, 
            console_w - (2 * padding), inputHeight, TRUE);
    }
}

void OnNotify(HWND hwnd, LPARAM lParam)
{
    LPNMHDR pnm = (LPNMHDR)lParam;
    if (pnm->hwndFrom == g_hTab && pnm->code == TCN_SELCHANGE)
    {
        int iSel = TabCtrl_GetCurSel(g_hTab);

        // --- REFACTORED: Show/Hide logic for tab pages ---
        // Hide the previously visible page
        if (g_hCurrentPage) {
            ShowWindow(g_hCurrentPage, SW_HIDE);
        }

        // Determine which page to show
        if (iSel >= 0 && iSel < NUM_TABS) 
        {
            switch(iSel)
            {
            case 0:
                g_hCurrentPage = hPageMontoring;
                break;
            case 1:
                g_hCurrentPage = hPageSpeedOffset;
                break;
            case 2:
                g_hCurrentPage = hPageAntiRoll;
                break;
            case 3:
                g_hCurrentPage = hPageAntiDive;
                break;
            case 4:
                g_hCurrentPage = hPageAntiSquat;
                break;
            default:
                g_hCurrentPage = NULL;
                break;
            }
        }
        else
        {
            g_hCurrentPage = NULL;
        }

        if (g_hCurrentPage) 
        {
            ShowWindow(g_hCurrentPage, SW_SHOW);

            RECT rcClient;
            GetClientRect(hwnd, &rcClient);
            OnSize(hwnd, 0, rcClient.right, rcClient.bottom);
        }
    }
}
