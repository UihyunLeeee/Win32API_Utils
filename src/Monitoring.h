#include <windows.h>
#include <commctrl.h>

class Monitoring
{
public:
    HWND hPage; // Exposed for main.cpp to set the current page
    // Main Quad windows handles
    HWND g_hQuadrantTL, g_hQuadrantTR, g_hQuadrantBL, g_hQuadrantBR;

    // Sub windows
    HWND hCurrent_FL, hCurrent_FR, hCurrent_RL, hCurrent_RR;
    HWND hProgressBar_FL, hValueDisplay_FL;

     // Font for the groupbox titles
    HFONT g_hGroupBoxFont = NULL;
    HFONT g_hValueFont = NULL;

    void CreateSecondArea(HWND hParent, HINSTANCE ghInst);
    bool CreateControlTab(HWND hParent, HINSTANCE ghInst);
    void ReSizeWindow(int page_w, int page_h);
};