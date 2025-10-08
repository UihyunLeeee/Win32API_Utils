#include <windows.h>
#include <commctrl.h>
#include <vector>
#include "LogicOnOffButton.h"
#include "uhConsole.h" 

class CurrentDisplay 
{
public:
    HWND hGroupBox;
    HWND hValueDisplay;
    HWND hProgressBar;

    // Static members for shared resources like fonts
    static HFONT s_hGroupBoxFont;
    static HFONT s_hValueFont;

    // Static methods to initialize and clean up shared resources
    static void Init();
    static void Cleanup();
    void Create(HWND hParent, HINSTANCE hInst,
                const wchar_t *title, int progressMin, int progressMax);
    void Resize(int x, int y, int w, int h);
};

class Monitoring
{
public:
    HWND hPage; // Exposed for main.cpp to set the current page
    // Main Quad windows handles
    HWND g_hQuadrantTL, g_hQuadrantTR, g_hQuadrantBL, g_hQuadrantBR;

    std::vector<LogicOnOffButton> m_logicButtons;

    CurrentDisplay m_displayFL, m_displayFR, m_displayRL, m_displayRR;

    Monitoring() : hPage(NULL), g_hQuadrantTL(NULL), g_hQuadrantTR(NULL),
                   g_hQuadrantBL(NULL), g_hQuadrantBR(NULL)
    { /* Vector will be populated in CreateSecondArea_BR */ }

    bool CreateControlTab(HWND hParent, HINSTANCE ghInst);
    //void CreateSecondArea_TL(HWND hParent, HINSTANCE ghInst);
    void CreateSecondArea_TR(HWND hParent, HINSTANCE ghInst);
    //void CreateSecondArea_BL(HWND hParent, HINSTANCE ghInst);
    void CreateSecondArea_BR(HWND hParent, HINSTANCE ghInst); // NOLINT

    // Message Handlers
    bool OnCommand(WPARAM wParam, LPARAM lParam);
    LRESULT OnCtlColorStatic(WPARAM wParam, LPARAM lParam);

    void ReSizeWindow(const RECT rcPage);
};