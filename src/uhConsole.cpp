#include "uhConsole.h"
#include <vector>

namespace uhConsole
{
    HWND hConsoleOutput = NULL;
    HWND hConsoleInput = NULL;
    WNDPROC pfnOrigInputProc = NULL; // For subclassing the input edit control
    
    void AppendTextToConsole(const wchar_t *text)
    {
        int outLen = GetWindowTextLengthW(hConsoleOutput);
        SendMessageW(hConsoleOutput, EM_SETSEL, (WPARAM)outLen, (LPARAM)outLen);
        SendMessageW(hConsoleOutput, EM_REPLACESEL, 0, (LPARAM)text);
    }

    LRESULT CALLBACK ConsoleInputProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_KEYDOWN && wParam == VK_RETURN)
        {
            // Get text from input
        int len = GetWindowTextLengthW(hConsoleInput); // Does not include null terminator
        if (len > 0)
            {
            std::vector<wchar_t> buffer(len + 1);
            GetWindowTextW(hConsoleInput, buffer.data(), len + 1);

                // Process the command (for now, just echo it to the output)
                std::wstring output = L"> ";
            output += buffer.data();
                output += L"\r\n";

                AppendTextToConsole(output.c_str());

                // Clear the input box
                SetWindowTextW(hConsoleInput, L"");
            }
            return 0; // We handled the message
        }

        // Call the original window procedure for other messages
        return CallWindowProcW(pfnOrigInputProc, hWnd, uMsg, wParam, lParam);
    }

    void CreateConsoleTab(HWND hPage, HINSTANCE ghInst)
    {
        // Create the output edit control (multiline, read-only)
        hConsoleOutput = CreateWindowExW(0, L"EDIT", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | 
            ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
            0, 0, 0, 0, hPage, NULL, ghInst, NULL);

        // Create the input edit control (single line)
        hConsoleInput = CreateWindowExW(0, L"EDIT", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            0, 0, 0, 0, hPage, NULL, ghInst, NULL);

        // Subclass the input edit control to handle the Enter key
        pfnOrigInputProc = (WNDPROC)SetWindowLongPtrW(hConsoleInput,
                                                      GWLP_WNDPROC, (LONG_PTR)ConsoleInputProc);

        // Set a default font for the console controls
        HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        SendMessage(hConsoleOutput, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hConsoleInput, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
} // namespace uhConsole