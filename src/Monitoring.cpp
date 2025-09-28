#include "Monitoring.h"
#include <CommCtrl.h>

class CurrentDisplay 
{
public:
    HWND hGroupBox;
    HWND hValueDisplay;
    HWND hProgressBar;

    void Create(HWND hParent, HINSTANCE hInst, const wchar_t *title, 
        int progressMin, int progressMax, HFONT hGroupFont, HFONT hValueFont)
    {
        hGroupBox = CreateWindowExW(0, L"BUTTON", title,
             WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
             0, 0, 0, 0, hParent, NULL, hInst, NULL);

        if (hGroupFont)
            SendMessage(hGroupBox, WM_SETFONT, (WPARAM)hGroupFont, TRUE);

        hValueDisplay = CreateWindowExW(0, L"STATIC", L"0.30 A", 
            WS_CHILD | WS_VISIBLE | SS_CENTER, 
            0, 0, 0, 0, hGroupBox, NULL, hInst, NULL);

        if (hValueFont)
            SendMessage(hValueDisplay, WM_SETFONT, (WPARAM)hValueFont, TRUE);

        hProgressBar = CreateWindowExW(0, PROGRESS_CLASSW, NULL, 
            WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 
            0, 0, 0, 0, hGroupBox, NULL, hInst, NULL);

        SendMessage(hProgressBar, PBM_SETRANGE32, progressMin, progressMax);
        SendMessage(hProgressBar, PBM_SETPOS, progressMin, 0);
    }

    void Resize(int x, int y, int w, int h)
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
};


/********************************************************************/

void Monitoring::CreateSecondArea(HWND hParent, HINSTANCE ghInst)
{
    // Create a larger font for the group boxes.
    // This font handle should be destroyed (DeleteObject) when the application closes,
    // but for simplicity in this example, we'll let the OS clean it up.
    if (!g_hGroupBoxFont)
    {
        LOGFONTW lf = {0};
        // Start with the default GUI font for consistency with the system theme.
        HFONT hDefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        if (hDefaultFont)
        {
            GetObjectW(hDefaultFont, sizeof(LOGFONTW), &lf);
        }
        else
        {
            // Fallback if we can't get the default font
            wcscpy_s(lf.lfFaceName, L"Segoe UI");
        }

        // Make the font larger and bold. lf.lfHeight is typically a negative number.
        lf.lfHeight = -40; // Tweak this value for desired size
        lf.lfWeight = FW_BOLD;
        g_hGroupBoxFont = CreateFontIndirectW(&lf);
    }
    // Create a font for the value displays.
    if (!g_hValueFont)
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

        // Make the font bold and a good size for reading values.
        lf.lfHeight = -40; // Tweak this value for desired size
        lf.lfWeight = FW_BOLD;
        g_hValueFont = CreateFontIndirectW(&lf);
    }
    // Create static labels and value displays inside the top-right quadrant (g_hQuadrantTR)
    hCurrent_FL = CreateWindowExW(0, L"BUTTON", L"FL (A)",
                                  WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                  0, 0, 0, 0, g_hQuadrantTR, NULL, ghInst, NULL);
    hCurrent_FR = CreateWindowExW(0, L"BUTTON", L"FR (A)",
                                  WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                  0, 0, 0, 0, g_hQuadrantTR, NULL, ghInst, NULL);
    hCurrent_RL = CreateWindowExW(0, L"BUTTON", L"RL (A)",
                                  WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                  0, 0, 0, 0, g_hQuadrantTR, NULL, ghInst, NULL);
    hCurrent_RR = CreateWindowExW(0, L"BUTTON", L"RR (A)",
                                  WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                  0, 0, 0, 0, g_hQuadrantTR, NULL, ghInst, NULL);

    // Set the custom font for the newly created controls.
    if (g_hGroupBoxFont)
    {
        SendMessage(hCurrent_FL, WM_SETFONT, (WPARAM)g_hGroupBoxFont, TRUE);
        SendMessage(hCurrent_FR, WM_SETFONT, (WPARAM)g_hGroupBoxFont, TRUE);
        SendMessage(hCurrent_RL, WM_SETFONT, (WPARAM)g_hGroupBoxFont, TRUE);
        SendMessage(hCurrent_RR, WM_SETFONT, (WPARAM)g_hGroupBoxFont, TRUE);
    }

    // Create child controls for the FL group box
    hValueDisplay_FL = CreateWindowExW(0, L"STATIC", L"0.30 A",
                                       WS_CHILD | WS_VISIBLE | SS_CENTER,
                                       0, 0, 0, 0, hCurrent_FL, NULL, ghInst, NULL);

    // Set the custom font for the value display.
    if (g_hValueFont)
    {
        SendMessage(hValueDisplay_FL, WM_SETFONT, (WPARAM)g_hValueFont, TRUE);
    }

    hProgressBar_FL = CreateWindowExW(0, PROGRESS_CLASSW, NULL,
                                      WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                                      0, 0, 0, 0, hCurrent_FL, NULL, ghInst, NULL);

    // Set progress bar range from 0.3 to 1.6 (as 30 to 160)
    SendMessage(hProgressBar_FL, PBM_SETRANGE32, 30, 160);
    SendMessage(hProgressBar_FL, PBM_SETPOS, 30, 0); // Initial value 0.3
}

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

    //CreateSecondArea(g_hQuadrantTR, ghInst);

    return true;
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

    // Top-left
    MoveWindow(hCurrent_FL,
               margin,
               margin,
               x_mid - margin - half_margin,
               y_mid - margin - half_margin,
               TRUE);

    // Resize children of hCurrent_FL
    RECT rcGroupFL;
    GetClientRect(hCurrent_FL, &rcGroupFL);
    int group_w = rcGroupFL.right;
    int group_h = rcGroupFL.bottom;

    const int inner_margin = 10;
    const int label_h = 50;
    const int spacing = 5;
    const int top_offset = 50; // To clear the groupbox title text

    // Position the value display static text
    MoveWindow(hValueDisplay_FL,
               inner_margin, top_offset, group_w - (2 * inner_margin), label_h, TRUE);

    // Position the progress bar
    int progress_y = top_offset + label_h + spacing;
    MoveWindow(hProgressBar_FL,
               inner_margin,
               progress_y,
               group_w - (2 * inner_margin),
               group_h - progress_y - inner_margin, TRUE);

    // Top-right
    MoveWindow(hCurrent_FR,
               x_mid + half_margin,
               margin,
               parent_w - (x_mid + half_margin) - margin,
               y_mid - margin - half_margin,
               TRUE);

    // Bottom-left
    MoveWindow(hCurrent_RL,
               margin,
               y_mid + half_margin,
               x_mid - margin - half_margin,
               parent_h - (y_mid + half_margin) - margin,
               TRUE);

    // Bottom-right
    MoveWindow(hCurrent_RR,
               x_mid + half_margin,
               y_mid + half_margin,
               parent_w - (x_mid + half_margin) - margin,
               parent_h - (y_mid + half_margin) - margin,
               TRUE);
}