#include "SpeedOffset.h"

namespace SpeedOffset
{
    // Main Quad windows handles
    HWND g_hQuadrantTL, g_hQuadrantTR, g_hQuadrantBL, g_hQuadrantBR;

    // Sub windows 
    HWND hCurrent_FL, hCurrent_FR, hCurrent_RL, hCurrent_RR; 


    void CreateSecondArea(HWND hParent, HINSTANCE ghInst);
    
    void CreateControlTab(HWND hPage, HINSTANCE ghInst)
    {
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
    }

    void CreateSecondArea(HWND hParent, HINSTANCE ghInst)
    {

    }

    void ReSizeWindow(int page_w, int page_h)
    {
        // Resize the four main quadrant windows
        MoveWindow(g_hQuadrantTL, 
            0, 0, page_w / 2, page_h / 2, TRUE);
        MoveWindow(g_hQuadrantTR, 
            page_w / 2, 0, page_w / 2, page_h / 2, TRUE);
        MoveWindow(g_hQuadrantBL, 
            0, page_h / 2, page_w / 2, page_h / 2, TRUE);
        MoveWindow(g_hQuadrantBR, 
            page_w / 2, page_h / 2, page_w / 2, page_h / 2, TRUE);
    }
}