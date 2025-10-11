#include "CurrentDisplay.h"

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
    if (s_hGroupBoxFont)
        DeleteObject(s_hGroupBoxFont);
    if (s_hValueFont)
        DeleteObject(s_hValueFont);
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
