#include "config.h"
#include "Util.h"
#include <commctrl.h> // For Tab Controls

#include <string> // For std::wstring

#include "Monitoring.h"
#include "uhConsole.h"

// Function Prototypes
HINSTANCE ghInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND CreateTabControl(HWND hWndParent, HINSTANCE hInst);
void CreateTabPages(HWND hWndParent);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnNotify(HWND hwnd, LPARAM lParam);

// Global Variables
HWND g_hTab; // Handle to the tab control

// --- REFACTORED: Use an array for tab pages for better scalability ---
const int NUM_TABS = 5;
HWND g_hPages[NUM_TABS];
HWND g_hCurrentPage; 
HWND g_hConsol; 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nShowCmd)
{
    if (debugger.IsValid())
    {
        // The new GUI console will be used for output.
    }

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
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // Start with a default size
        NULL, NULL, hInstance, NULL);

    if (hWnd == NULL)
    {
        MessageBoxW(NULL, L"Window Creation Failed!", L"Error",
            MB_ICONERROR | MB_OK);
        return 0;
    }

    ShowWindow(hWnd, SW_SHOWMAXIMIZED);
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
            CreateTabPages(hWnd);

            // Create the console window on the first tab page
            uhConsole::CreateConsoleTab(g_hPages[0], ghInst);
            if(!uhConsole::hConsoleOutput || !uhConsole::hConsoleInput )
            {
                MessageBoxW(hWnd, L"Console Creation Failed!", L"Error",
                    MB_ICONERROR | MB_OK);
                return -1;
            }
            g_hCurrentPage = g_hPages[0];
            ShowWindow(g_hCurrentPage, SW_SHOW);
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

void CreateTabPages(HWND hWndParent)
{
    // Create page containers (simple STATIC windows)
    g_hPages[0] = CreateWindowExW(0, L"STATIC", NULL,
        WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWndParent, NULL, ghInst, NULL);
    g_hPages[1] = CreateWindowExW(0, L"STATIC", NULL, 
        WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWndParent, NULL, ghInst, NULL);
    g_hPages[2] = CreateWindowExW(0, L"STATIC", NULL, 
        WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWndParent, NULL, ghInst, NULL);
    g_hPages[3] = CreateWindowExW(0, L"STATIC", NULL, 
        WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWndParent, NULL, ghInst, NULL);
    g_hPages[4] = CreateWindowExW(0, L"STATIC", NULL,
        WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWndParent, NULL, ghInst, NULL);

    // Create the content (the 2x2 grid) for the first page.
    Monitoring::CreateControlTab(g_hPages[0], ghInst);
}

void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    if (g_hTab)
    {
        /************ Resize Main Window Area *******/
        MoveWindow(g_hTab, 0, 0, cx, cy, TRUE);

        RECT rcTab;
        GetClientRect(g_hTab, &rcTab);
        SendMessage(g_hTab, TCM_ADJUSTRECT, FALSE, (LPARAM)&rcTab);

        if (g_hCurrentPage) {
            MoveWindow(g_hCurrentPage, rcTab.left, rcTab.top,
                       rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, TRUE);
        }

        RECT rcPage;
        GetClientRect(g_hCurrentPage, &rcPage);
        int page_w = rcPage.right - rcPage.left;
        int page_h = rcPage.bottom - rcPage.top;

        // Resize content based on which tab is selected
        int monitoring_h = (int)(page_h * 0.8);
        /************ Resize Monitoring Area (Top 80%) ************/
        if (g_hCurrentPage == g_hPages[0])
        {
            Monitoring::ReSizeWindow(page_w, monitoring_h);
        }
        /************ Resize Console Area (Bottom 20%) ************/
        int inputHeight = 30;
        int padding = 5;
        int console_top_y = monitoring_h;
        int console_h = page_h - console_top_y;
        int output_h = console_h - inputHeight - padding;
        if (output_h < 0)
            output_h = 0;

        MoveWindow(uhConsole::hConsoleOutput, 0, console_top_y, page_w, output_h, TRUE);
        MoveWindow(uhConsole::hConsoleInput, padding, console_top_y + output_h + padding,
                   page_w - (2 * padding), inputHeight, TRUE);
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
        if (iSel >= 0 && iSel < NUM_TABS) {
            g_hCurrentPage = g_hPages[iSel];
        }
        else {
            g_hCurrentPage = NULL;
        }

        // Show the new page and force a resize to position it correctly.
        if (g_hCurrentPage) {
            ShowWindow(g_hCurrentPage, SW_SHOW);
            // Force a resize of the main window's client area to correctly position the new page
            RECT rcClient;
            GetClientRect(hwnd, &rcClient);
            OnSize(hwnd, 0, rcClient.right, rcClient.bottom);
        }
    }
}
