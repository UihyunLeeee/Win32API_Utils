#include "Monitoring.h"
#include <CommCtrl.h>

LRESULT CALLBACK QuadrantSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
WNDPROC pfnOrigQuadrantProc = NULL;
HBRUSH g_hBlueBrush = CreateSolidBrush(RGB(220, 230, 255));
bool g_bQuadrantClicked = false;

bool Monitoring::CreateControlTab(HWND hParent, HINSTANCE ghInst)
{
    hPage = CreateWindowExW(0, L"STATIC", L"Current Monitoring",
                            WS_CHILD | WS_BORDER, 0, 0, 0, 0, hParent,
                            NULL, ghInst, NULL);

    // Create four static controls to act as the quadrants.
    g_hQuadrantTL = CreateWindowExW(0, L"STATIC", L"Motion Display Window",
                                    WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
                                    0, 0, 0, 0, hPage, NULL, ghInst, NULL);

    g_hQuadrantTR = CreateWindowExW(0, L"STATIC", L"",
                                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                                    0, 0, 0, 0, hPage, NULL, ghInst, NULL);

    g_hQuadrantBL = CreateWindowExW(0, L"STATIC", L"Bottom-Left (2,1)",
                                    WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER,
                                    0, 0, 0, 0, hPage, NULL, ghInst, NULL);

    g_hQuadrantBR = CreateWindowExW(0, L"STATIC", L"",
                                    WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY | WS_BORDER,
                                    0, 0, 0, 0, hPage, NULL, ghInst, NULL);

    SetWindowSubclass(g_hQuadrantBR, QuadrantSubclassProc, 1, (DWORD_PTR)this);

    CreateSecondArea_TR(g_hQuadrantTR, ghInst);
    CreateSecondArea_BR(g_hQuadrantBR, ghInst);

    return true;
}

void Monitoring::CreateSecondArea_TR(HWND hParent, HINSTANCE ghInst)
{
    // Initialize shared resources for the CurrentDisplay class
    CurrentDisplay::Init();

    // Create the four current display controls inside the top-right quadrant
    m_displayFL.Create(g_hQuadrantTR, ghInst, L"FL", 30, 160);
    m_displayFR.Create(g_hQuadrantTR, ghInst, L"FR", 30, 160);
    m_displayRL.Create(g_hQuadrantTR, ghInst, L"RL", 30, 160);
    m_displayRR.Create(g_hQuadrantTR, ghInst, L"RR", 30, 160);
}

void Monitoring::CreateSecondArea_BR(HWND hParent, HINSTANCE ghInst)
{
    LogicOnOffButton::InitFont();

    const wchar_t *buttonLabels[] = {
        L"Speed Offset", L"Ride", L"Anti Roll",
        L"Anti Dive", L"Anti Squat", L"Preview"};

    for (const auto &label : buttonLabels)
    {
        m_logicButtons.emplace_back();                        // Create a new button object
        m_logicButtons.back().Create(hParent, ghInst, label); // Initialize it
    }
}

LRESULT CALLBACK QuadrantSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {

    case WM_COMMAND:
    {
        debugger.PrintErrorMsg(L"QuadrantSubclassProc: WM_COMMAND");
    }

    case WM_LBUTTONDOWN:
    {
        debugger.PrintErrorMsg(L"QuadrantSubclassProc: WM_LBUTTONDOWN");
        g_bQuadrantClicked = !g_bQuadrantClicked; // Toggle state

        if (g_bQuadrantClicked)
        {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(0, 0, 128));
            SetBkMode(hdcStatic, TRANSPARENT);
            return (LRESULT)g_hBlueBrush;
        }

        uhConsole::AppendTextToConsole(L"Bottom-right quadrant clicked!\r\n");
        InvalidateRect(hWnd, NULL, TRUE);
        return 0; // We handled this message
    }

    case WM_CTLCOLORSTATIC:
    {
        if (g_bQuadrantClicked)
        {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(0, 0, 128));
            SetBkMode(hdcStatic, TRANSPARENT);
            return (LRESULT)g_hBlueBrush;
        }
        // If not clicked, fall through to default processing
    }
    break;
    }
    // For any other messages, pass them to the next subclass or the original window procedure.
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// void Monitoring::ReSizeWindow(int page_w, int page_h)
void Monitoring::ReSizeWindow(const RECT rcTab)
{
    // Resize the current page container to fit the tab display area.
    MoveWindow(hPage, rcTab.left, rcTab.top,
               rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, TRUE);

    // Get the dimensions of the page content area to pass to
    // the page-specific resize function.
    RECT rcPage;
    GetClientRect(hPage, &rcPage);

    int page_w = rcPage.right - rcPage.left;
    int page_h = rcPage.bottom - rcPage.top;

    // Resize the four main quadrant windows
    MoveWindow(g_hQuadrantTL,
               0, 0, page_w / 2, page_h / 2, TRUE);
    MoveWindow(g_hQuadrantTR,
               page_w / 2, 0, page_w / 2, page_h / 2, TRUE);
    MoveWindow(g_hQuadrantBL,
               0, page_h / 2, page_w / 2, page_h / 2, TRUE);
    MoveWindow(g_hQuadrantBR,
               page_w / 2, page_h / 2, page_w / 2, page_h / 2, TRUE);

    // --- Resize TR Quadrant Contents (Current Displays) ---
    {
        RECT rcParent;
        GetClientRect(g_hQuadrantTR, &rcParent);
        int parent_w = rcParent.right;
        int parent_h = rcParent.bottom;

        const int margin = 10;
        const int half_margin = margin / 2;
        int x_mid = parent_w / 2;
        int y_mid = parent_h / 2;

        int w = x_mid - margin - half_margin;
        int h = y_mid - margin - half_margin;

        m_displayFL.Resize(margin, margin, w, h);

        int x_tr = x_mid + half_margin;
        int w_tr = parent_w - x_tr - margin;
        m_displayFR.Resize(x_tr, margin, w_tr, h);

        int y_bl = y_mid + half_margin;
        int h_bl = parent_h - y_bl - margin;
        m_displayRL.Resize(margin, y_bl, w, h_bl);

        m_displayRR.Resize(x_tr, y_bl, w_tr, h_bl);
    }

    // --- Resize BR Quadrant Contents (Buttons) ---
    {
        RECT rcParent;
        GetClientRect(g_hQuadrantBR, &rcParent);
        const int margin = 10;
        const int spacing = 5;
        int btn_w = (rcParent.right - (2 * margin) - (2 * spacing)) / 3;
        int btn_h = (rcParent.bottom - (2 * margin) - spacing) / 2;

        int y1 = margin;
        int y2 = margin + btn_h + spacing;
        int x1 = margin;
        int x2 = margin + btn_w + spacing;
        int x3 = margin + 2 * (btn_w + spacing);

        m_logicButtons[0].Resize(x1, y1, btn_w, btn_h);
        m_logicButtons[1].Resize(x2, y1, btn_w, btn_h);
        m_logicButtons[2].Resize(x3, y1, btn_w, btn_h);

        m_logicButtons[3].Resize(x1, y2, btn_w, btn_h);
        m_logicButtons[4].Resize(x2, y2, btn_w, btn_h);
        m_logicButtons[5].Resize(x3, y2, btn_w, btn_h);
    }
}