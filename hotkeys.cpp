#ifndef UNICODE
#define UNICODE
#endif // UNICODE
#define WINVER 0x1703
#define PI 3.14159



#include <windows.h>
#include "hotkeyHeader.h"


    void autoClickerProcess(bool& autoClicking);

    void HotKeyProcess(MSG msg, int& opacity,
                       bool& layerFlag,bool& wasd, bool& mouseHooked,bool& autoClickMode,
                       HINSTANCE hInst,HWND& mouseWindow){
    HWND activeWindow = GetForegroundWindow();

    switch(msg.wParam)
            {
            case(1):    //set topmost
                SetWindowPos(activeWindow, HWND_TOPMOST,0,0,250,250,SWP_NOSIZE|SWP_NOMOVE|SWP_ASYNCWINDOWPOS);
                return;

            case(2):    //set not topmost
                SetWindowPos(activeWindow, HWND_NOTOPMOST,0,0,250,250,SWP_NOSIZE|SWP_NOMOVE|SWP_ASYNCWINDOWPOS);
                return;
            case(3):    //make window more opaque
                //must set window to be a layered window to make it transparent
                if(layerFlag == false){
                    SetWindowLong(activeWindow,
                                    GWL_EXSTYLE,
                                    GetWindowLong(activeWindow, GWL_EXSTYLE) | WS_EX_LAYERED);
                    layerFlag =true;

                    //To allow bg scrolling
                    SetWindowPos(activeWindow,mouseWindow,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                }
                opacity+=10;
                SetLayeredWindowAttributes(activeWindow, 0, (255 * opacity) / 100, 0x00000002);
                return;
            case(4):    //make window more transparent
                //must set window to be a layered window to make it transparent
                if(layerFlag == false){
                    SetWindowLong(activeWindow,
                                    GWL_EXSTYLE,
                                    GetWindowLong(activeWindow, GWL_EXSTYLE) | WS_EX_LAYERED);
                    layerFlag =true;

                    //To allow bg scrolling
                    SetWindowPos(activeWindow,mouseWindow,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                }
                opacity-=10;
                SetLayeredWindowAttributes(activeWindow, 0, (1+(255 * opacity) / 100), 0x00000002);
                return;
            case(5):    //reset window opacity
                // Remove WS_EX_LAYERED from this window styles
                SetWindowLong(activeWindow,
                              GWL_EXSTYLE,
                              GetWindowLong(activeWindow, GWL_EXSTYLE) & ~WS_EX_LAYERED);

                // Ask the window and its children to repaint
                RedrawWindow(activeWindow,
                            NULL,
                            NULL,
                            RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
                            layerFlag=false;
                            opacity =100;
                return;
            case(6):    //enter/exit wasd mode
                wasd = !wasd;
                if(wasd)
                {
                    wchar_t activeText[256];
                    GetWindowText(activeWindow,activeText,256);
                    hhookKey = SetWindowsHookEx(WH_KEYBOARD_LL,HookProc,hInst,0);

                    if (hhookKey == 0) {
                        std::wcout<<"Failed to set hook on "<<activeText<<std::endl;}
                    else {
                        std::wcout<<"Wasd hook established"<<std::endl;}
                    return;
                }
                else if(hhookKey ==NULL)
                    {
                        return;
                    }
                else if(wasd ==false)
                {
                    UnhookWindowsHookEx(hhookKey);
                    return;
                }
                else
                    return;
            case(7):    //if mouse wheel lock is set
                mouseHooked=!mouseHooked;
                if(mouseHooked)
                {
                    mouseWindow = GetForegroundWindow();
                    wchar_t activeText[256];
                    GetWindowText(activeWindow,activeText,256);
                    hhookMouse = SetWindowsHookEx(WH_MOUSE_LL,MouseHookProc,hInst,0);

                    if (hhookMouse == 0) {
                        std::wcout<<"Failed to set hook on "<<activeText<<std::endl;}
                    else {
                        std::wcout<<"Mouse hook established on "<<activeText<<std::endl;}
                    return;
                }
                else
                {
                    UnhookWindowsHookEx(hhookMouse);
                    return;
                }
            case(8)://Auto Clicker Toggled, sets hook on vk_down to trigger auto-clicking
                autoClickMode = !autoClickMode;
                if(autoClickMode)
                {
                    hhookClicker = SetWindowsHookEx(WH_KEYBOARD_LL,HookProcClick,hInst,0);

                    if (hhookClicker == 0) {
                        std::wcout<<"Failed to set clicker hook "<<std::endl;
                    }
                    else {
                        std::wcout<<"Clicker hook established"<<std::endl;
                    }
                    return;
                }
                if(autoClickMode ==false)
                {
                    UnhookWindowsHookEx(hhookClicker);
                    return;
                }
                else
                    return;

            }
}

