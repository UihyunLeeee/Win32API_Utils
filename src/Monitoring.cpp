#include "Monitoring.h"
#include <CommCtrl.h>

// Define the static members of CurrentDisplay
HFONT CurrentDisplay::s_hGroupBoxFont = NULL;
HFONT CurrentDisplay::s_hValueFont = NULL;

void CurrentDisplay::Init()
{
    // Create a larger font for the group boxes.
    if (!s_hGroupBoxFont)
    {
        LOGFONTW lf = {0};
        HFONT hDefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        if (hDefaultFont)
        {
            GetObjectW(hDefaultFont, sizeof(LOGFONTW), &lf);
        }
        else
        {
            wcscpy_s(lf.lfFaceName, L"Segoe UI");
        }
        lf.lfHeight = -40; // Tweak this value for desired size
        lf.lfWeight = FW_BOLD;
        s_hGroupBoxFont = CreateFontIndirectW(&lf);
    }
    // Create a font for the value displays.
    if (!s_hValueFont)
    {
        LOGFONTW lf = {0};
        HFONT hDefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        if (hDefaultFont)
        {
            GetObjectW(hDefaultFont, sizeof(LOGFONTW), &lf);
        }
        else
        {
            wcscpy_s(lf.lfFaceName, L"Segoe UI");
        }
        lf.lfHeight = -40; // Tweak this value for desired size
        lf.lfWeight = FW_BOLD;
        s_hValueFont = CreateFontIndirectW(&lf);
    }
}

void CurrentDisplay::Cleanup()
{
    if (s_hGroupBoxFont) DeleteObject(s_hGroupBoxFont);
    if (s_hValueFont) DeleteObject(s_hValueFont);
}

void CurrentDisplay::Create(HWND hParent, HINSTANCE hInst, 
    const wchar_t *title, int progressMin, int progressMax)
{
    hGroupBox = CreateWindowExW(0, L"BUTTON", title,
                                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                0, 0, 0, 0, hParent, NULL, hInst, NULL);

    if (s_hGroupBoxFont)
        SendMessage(hGroupBox, WM_SETFONT, (WPARAM)s_hGroupBoxFont, TRUE);

    hValueDisplay = CreateWindowExW(0, L"STATIC", L"0.30 A",
                                    WS_CHILD | WS_VISIBLE | SS_CENTER,
                                    0, 0, 0, 0, hGroupBox, NULL, hInst, NULL);

    if (s_hValueFont)
        SendMessage(hValueDisplay, WM_SETFONT, (WPARAM)s_hValueFont, TRUE);

    hProgressBar = CreateWindowExW(0, PROGRESS_CLASSW, NULL,
                                   WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                                   0, 0, 0, 0, hGroupBox, NULL, hInst, NULL);

    SendMessage(hProgressBar, PBM_SETRANGE32, progressMin, progressMax);
    SendMessage(hProgressBar, PBM_SETPOS, progressMin, 0);
}

void CurrentDisplay::Resize(int x, int y, int w, int h)
{
    MoveWindow(hGroupBox, x, y, w, h, TRUE);

    RECT rcGroup;
    GetClientRect(hGroupBox, &rcGroup);
    int group_w = rcGroup.right;
    int group_h = rcGroup.bottom;

    const int inner_margin = 10;
    const int label_h = 50;
    const int spacing = 5;
    const int top_offset = 50;

    MoveWindow(hValueDisplay, inner_margin, top_offset,
               group_w - (2 * inner_margin), label_h, TRUE);

    int progress_y = top_offset + label_h + spacing;
    MoveWindow(hProgressBar, inner_margin, progress_y,
               group_w - (2 * inner_margin),
               group_h - progress_y - inner_margin, TRUE);
}

/********************************************************************/

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

    g_hQuadrantBR = CreateWindowExW(0, L"STATIC", L"Bottom-Right (2,2)", 
        WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 
        0, 0, 0, 0, hPage, NULL, ghInst, NULL);

    CreateSecondArea(g_hQuadrantTR, ghInst);

    return true;
}

void Monitoring::CreateSecondArea(HWND hParent, HINSTANCE ghInst)
{
    // Initialize shared resources for the CurrentDisplay class
    CurrentDisplay::Init();
    
    // Create the four current display controls inside the top-right quadrant
    m_displayFL.Create(g_hQuadrantTR, ghInst, L"FL", 30, 160);
    m_displayFR.Create(g_hQuadrantTR, ghInst, L"FR", 30, 160);
    m_displayRL.Create(g_hQuadrantTR, ghInst, L"RL", 30, 160);
    m_displayRR.Create(g_hQuadrantTR, ghInst, L"RR", 30, 160);
}


//void Monitoring::ReSizeWindow(int page_w, int page_h)
void Monitoring::ReSizeWindow(const RECT rcTab)
{
    // Resize the current page container to fit the tab display area.
    MoveWindow(hPage, rcTab.left, rcTab.top,
               rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, TRUE);

    // Get the dimensions of the page content area to pass to the page-specific resize function.
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

    // Resize the current monitor display
    RECT rcParent;
    GetClientRect(g_hQuadrantTR, &rcParent);
    int parent_w = rcParent.right;
    int parent_h = rcParent.bottom;

    const int margin = 10;
    const int half_margin = margin / 2;
    int x_mid = parent_w / 2;
    int y_mid = parent_h / 2;

    // Calculate dimensions for the four sub-quadrants
    int w = x_mid - margin - half_margin;
    int h = y_mid - margin - half_margin;

    // Top-left (FL)
    m_displayFL.Resize(margin, margin, w, h);

    // Top-right (FR)
    int x_tr = x_mid + half_margin;
    int w_tr = parent_w - x_tr - margin;
    m_displayFR.Resize(x_tr, margin, w_tr, h);

    // Bottom-left (RL)
    int y_bl = y_mid + half_margin;
    int h_bl = parent_h - y_bl - margin;
    m_displayRL.Resize(margin, y_bl, w, h_bl);

    // Bottom-right (RR)
    m_displayRR.Resize(x_tr, y_bl, w_tr, h_bl);
}