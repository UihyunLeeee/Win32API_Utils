#include "HookManager.h"
#include "Util.h" 
#include "resource.h" // For ID_TRAY_SETSIZE

HHOOK HookManager::m_keyboardHook = NULL;
HWND HookManager::m_appWindow = NULL;

LRESULT CALLBACK HookManager::LowLevelKeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // nCode < 0 means the message is not for us.
    if (nCode >= 0)
    {
        // We are interested in key-down events.
        if (wParam == WM_KEYDOWN)
        {
            // lParam is a pointer to a KBDLLHOOKSTRUCT.
            KBDLLHOOKSTRUCT* pkbhs = (KBDLLHOOKSTRUCT*)lParam;

            // Check if the key that was pressed is '1'.
            // pkbhs->vkCode holds the virtual-key code.
            if (pkbhs->vkCode == '1')
            {
                // Now, check if the Left or Right Control key is being held down.
                // GetAsyncKeyState checks the current state of the key.
                // The 0x8000 bit is set if the key is currently pressed.
                bool isCtrlPressed = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ||
                                     (GetAsyncKeyState(VK_RCONTROL) & 0x8000);

                if (isCtrlPressed)
                {
                    debugger.PrintErrorMsg(L"*** Ctrl+1 Detected! Sending WM_COMMAND to restore window layout... ***");
                    // Post a message to our main window to trigger the action.
                    // We use PostMessage because it's non-blocking and safer to call from a hook.
                    if (m_appWindow && IsWindow(m_appWindow))
                    {
                        PostMessage(m_appWindow, WM_COMMAND, MAKEWPARAM(ID_TRAY_SETSIZE, 0), 0);
                    }
                }
            }
        }
    }

    // CRITICAL: You MUST call the next hook in the chain.
    // If you don't, you will block all other keyboard input in the entire system.
    return CallNextHookEx(m_keyboardHook, nCode, wParam, lParam);
}


// --- Class Implementation ---

HookManager::HookManager()
{
    // Constructor doesn't need to do anything.
}

HookManager::~HookManager()
{
    // The destructor ensures that the hook is uninstalled when the object is destroyed.
    // This is a very important safety feature (RAII).
    UninstallHook();
}

void HookManager::InstallHook(HWND hWnd)
{
    // Only install the hook if it's not already installed.
    if (m_keyboardHook == NULL)
    {
        m_appWindow = hWnd; // Store the main window handle
        extern ConsoleDebugger debugger;
        debugger.PrintErrorMsg(L"Installing global keyboard hook...");

        // SetWindowsHookEx installs the hook.
        // WH_KEYBOARD_LL: The type of hook (low-level keyboard).
        // LowLevelKeyboardProc: A pointer to our callback function.
        // GetModuleHandle(NULL): A handle to our own application instance.
        // 0: The hook is global (associated with all threads).
        m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardHookProc, GetModuleHandle(NULL), 0);

        if (m_keyboardHook) 
        {
            debugger.PrintErrorMsg(L"Hook installed successfully.");
        } 
        else 
        {
            debugger.PrintErrorMsg(L"Failed to install hook!");
        }
    }
}

void HookManager::UninstallHook()
{
    if (m_keyboardHook != NULL)
    {
        extern ConsoleDebugger debugger;
        debugger.PrintErrorMsg(L"Uninstalling global keyboard hook...");
        UnhookWindowsHookEx(m_keyboardHook);
        m_keyboardHook = NULL;
    }
}
