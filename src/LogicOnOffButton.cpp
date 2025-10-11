#include "LogicOnOffButton.h"
#include "util.h"

// Initialize static members
HFONT LogicOnOffButton::s_hFont = NULL;
int LogicOnOffButton::s_nNextControlId = 2000; // Start IDs from a high number to avoid conflicts

LogicOnOffButton::LogicOnOffButton() : m_hWnd(NULL),
                                       m_bIsOn(false),
                                       m_nControlId(s_nNextControlId++),
                                       m_onClick(nullptr),
                                       m_pContext(nullptr)
{
}

void LogicOnOffButton::InitFont()
{
    if (!s_hFont)
    {
        LOGFONTW lf = {0};
        HFONT hDefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        if (hDefaultFont)
        {
            GetObjectW(hDefaultFont, sizeof(LOGFONTW), &lf);
        }
        else
        {
            // Fallback if GetStockObject fails (e.g., during early initialization)
            wcscpy_s(lf.lfFaceName, L"Segoe UI");
        }
        lf.lfHeight = -24;
        lf.lfWeight = FW_SEMIBOLD;
        s_hFont = CreateFontIndirectW(&lf);
    }
}

void LogicOnOffButton::CleanupFont()
{
    if (s_hFont)
    {
        DeleteObject(s_hFont);
        s_hFont = NULL;
    }
}

void LogicOnOffButton::Create(HWND hParent, HINSTANCE hInst, const wchar_t *text)
{
    m_hWnd = CreateWindowExW(0, L"BUTTON", text, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                             0, 0, 0, 0, hParent, (HMENU)m_nControlId, hInst, NULL);

    debugger.PrintErrorMsg(L"LogicOnOffButton::Create");

    if (s_hFont)
    {
        SendMessage(m_hWnd, WM_SETFONT, (WPARAM)s_hFont, TRUE);
    }
}

void LogicOnOffButton::Resize(int x, int y, int w, int h)
{
    MoveWindow(m_hWnd, x, y, w, h, TRUE);
}

void LogicOnOffButton::ToggleState() { m_bIsOn = !m_bIsOn; }
bool LogicOnOffButton::IsOn() const { return m_bIsOn; }
int LogicOnOffButton::GetId() const { return m_nControlId; }
HWND LogicOnOffButton::GetHwnd() const { return m_hWnd; }

void LogicOnOffButton::SetOnClick(ClickCallback func, void *context)
{
    m_onClick = func;
    m_pContext = context;
}

void LogicOnOffButton::HandleClick()
{
    ToggleState();
    if (m_onClick)
    {
        m_onClick(m_pContext, m_bIsOn);
    }
}

LRESULT LogicOnOffButton::HandleCtlColor(HDC hdc)
{
    SetBkMode(hdc, TRANSPARENT);
    if (m_bIsOn)
    {
        SetTextColor(hdc, RGB(255, 0, 0)); // Red for ON
    }
    else
    {
        SetTextColor(hdc, RGB(128, 128, 128)); // Gray for OFF
    }
    return (LRESULT)GetStockObject(NULL_BRUSH);
}
