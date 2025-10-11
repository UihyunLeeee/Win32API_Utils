#include <windows.h>
#include <commctrl.h>
#include <vector>
#include "LogicOnOffButton.h"
#include "CurrentDisplay.h"
#include "uhConsole.h"
#include "Util.h"

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
    { /* Vector will be populated in CreateSecondArea_BR */
    }

    bool CreateControlTab(HWND hParent, HINSTANCE ghInst);
    // void CreateSecondArea_TL(HWND hParent, HINSTANCE ghInst);
    void CreateSecondArea_TR(HWND hParent, HINSTANCE ghInst);
    // void CreateSecondArea_BL(HWND hParent, HINSTANCE ghInst);
    void CreateSecondArea_BR(HWND hParent, HINSTANCE ghInst); // NOLINT

    void ReSizeWindow(const RECT rcPage);
};