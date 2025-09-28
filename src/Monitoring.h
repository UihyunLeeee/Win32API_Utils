#include <windows.h>
#include <commctrl.h>

namespace Monitoring
{
    HWND CreateControlTab(HWND hParent, HINSTANCE ghInst);
    void ReSizeWindow(int page_w, int page_h);
}