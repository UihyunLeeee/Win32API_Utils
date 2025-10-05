#include <windows.h>
#include <commctrl.h>

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

    CurrentDisplay m_displayFL, m_displayFR, m_displayRL, m_displayRR;

    bool CreateControlTab(HWND hParent, HINSTANCE ghInst);
    void CreateSecondArea(HWND hParent, HINSTANCE ghInst);
    //void ReSizeWindow(int page_w, int page_h);
    void ReSizeWindow(const RECT rcPage);
};