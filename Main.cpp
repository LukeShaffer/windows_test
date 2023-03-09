#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include <thread>
#include <cstdio>
#include <math.h>
#include <ctime>
//#include <random> //std::default_random_engine


#include "hotkeyHeader.h"
#include "cosWaves.h"

#define PI 3.141592


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND mouseWindow;
HWND activeWindow;
HWND createdWindow;
UINT uACToggle;
bool clickingLoop = false;
// Old implementation with pthreads through mingw
// std::thread clickingThread;
HANDLE clickingThread;

int WINAPI WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR pCmdLine,
        int CmdShow) {

    srand(time(0));

    const wchar_t CLASS_NAME[] = L"Hot Keys";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    createdWindow = CreateWindowEx(
        WS_EX_CLIENTEDGE,                               // dwExStyle
        CLASS_NAME,                                     // lpClassName
        L"Hotkey Window",                               // lpWindowName
        WS_OVERLAPPEDWINDOW |WS_HSCROLL |WS_VSCROLL,    // dwStyle
        0,0,250,250,                                    // x, y, width, height
        NULL,                                           // hWndParent
        NULL,                                           // hMenu
        hInstance,                                      // hInstance
        NULL                                            // lpParam
    );

    if(createdWindow == NULL){
        return -1;
    }

    HINSTANCE hInst = (HINSTANCE)GetWindowLong(createdWindow, GWL_HINSTANCE );

    //hotkey section
    // (hwnd, hotkey_id, fsModifiers, virtual_key)
    RegisterHotKey(createdWindow,1,MOD_ALT|0x4000,VK_UP);        //On top key
    RegisterHotKey(createdWindow,2,MOD_ALT|0x4000,VK_DOWN);      //Not on top
    RegisterHotKey(createdWindow,3,MOD_ALT|0x4000,VK_ADD);       //make window more opaque
    RegisterHotKey(createdWindow,4,MOD_ALT|0x4000,VK_SUBTRACT);  //make window more transparent
    RegisterHotKey(createdWindow,5,MOD_ALT|0x4000,VK_BACK);      //Set Window to 100% opaque
    RegisterHotKey(createdWindow,6,MOD_ALT|0x4000,0x51);         //Enter wasd mode, 0x51 is 'q'
    RegisterHotKey(createdWindow,7,MOD_ALT|0x4000,0x4c);         //Select Scroll Lock Window, 0x4C is 'L'
    RegisterHotKey(createdWindow,8,MOD_ALT|0x4000,0x45);         //Toggle Auto Clicker, 0x45 is 'e'

    // control vars for various hotkeys
    bool layerFlag = false;
    int opacity = 100;
    bool wasd = false;
    bool mouseHooked = false;
    bool autoClickMode = false;

    // Custom Windows message that I use to signal autoclicker toggle
    uACToggle = RegisterWindowMessageW(L"ACTOGGLE");

    // disable showing the window, used for message printing / debugging
    //ShowWindow(createdWindow, CmdShow);

    MSG msg = {};
    BOOL msgSuc = 1;

    while(msgSuc != 0){
        msgSuc = GetMessage(&msg, NULL, 0, 0);
        if(msgSuc == -1){
            //handle errors, I'll add this later
        }

        // any of my registered hotkeys have been pressed
        else if (msg.message == WM_HOTKEY){
            HotKeyProcess(msg, opacity, layerFlag, wasd, mouseHooked,
                autoClickMode, hInst, mouseWindow);
        }
        // debug, print if the created window (hidden) was scrolled on
        else{
            if(msg.message == WM_MOUSEWHEEL){
                mouseHookMsg = msg;
                std::cout<< "wParam: " << (signed int)mouseHookMsg.wParam
                    << " lParam: " << (signed int)mouseHookMsg.lParam
                    <<std::endl;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    }
    return 0;
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    /*
        Main function for my hotkey window
    */
    switch(uMsg){
        // default destructor
        case(WM_DESTROY):
            PostQuitMessage(0);
            return 0;
        // default update
        case(WM_PAINT):
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            EndPaint(hwnd, &ps);
            return 0;
    }
    // compiler complains that uACToggle wasn't initialized with a constant expression
    // toggles auto clicker as clicking or not clicking
    if (uMsg == uACToggle){
        clickingLoop = !clickingLoop;
        // tmp var to hold thread ID - this is separate from the handle
        DWORD threadID;
        if(clickingLoop){
            // clickingThread = std::thread(autoClickerProcess,std::ref(clickingLoop));
            clickingThread = CreateThread(
                NULL,                // LPSECURITY_ATTRIBUTES lpThreadAttributes
                0,                   // SIZE_T dwStackSize - use default
                autoClickerProcess,  // LPTHREAD_START_ROUTINE lpStartAddress
                NULL,                // __drv_aliasesMem LPVOID lpParameter
                0,                   // DWORD dwCreationFlags
                &threadID
            );
            std::cout<<"clicker thread initialized\n";
        }
        else if (!clickingLoop){
            //clickingThread.join();
            TerminateThread(clickingThread, 0);
            std::cout<<"clicker thread safely closed"<<std::endl;
        }

        std::cout<<"uACToggle message received"<<std::endl;
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    /*
        WASD Hook implementation
    */
    KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*) lParam);

    //is 0 if pressed down, 1 if key released
    bool keyUp = hooked_key.flags >> 7;
    UINT keyDir = keyUp? WM_KEYUP: WM_KEYDOWN;

    if(nCode == HC_ACTION) {
        switch(hooked_key.vkCode) {
            case(0x57): //'w' to up arrow 0x26
                hooked_key.vkCode = 0x26;
                hooked_key.scanCode = 72;
                SendMessage(
                    GetForegroundWindow(),
                    keyDir,
                    hooked_key.vkCode,
                    hookMsg.lParam
                );
                return 1;

            case(0x41): //'a' to left arrow 0x25
                hooked_key.vkCode = 0x25;
                hooked_key.scanCode = 75;
                SendMessage(
                    GetForegroundWindow(),
                    keyDir,
                    hooked_key.vkCode,
                    hookMsg.lParam
                );
                return 1;
            case(0x53): //'s' to down arrow 0x28
                hooked_key.vkCode = 0x28;
                hooked_key.scanCode=80;
                SendMessage(
                    GetForegroundWindow(),
                    keyDir,
                    hooked_key.vkCode,
                    hookMsg.lParam
                );
                return 1;
            case(0x44): //'d' to right arrow 0x27
                hooked_key.vkCode = 0x27;
                hooked_key.scanCode=77;
                SendMessage(
                    GetForegroundWindow(),
                    keyDir,
                    hooked_key.vkCode,
                    hookMsg.lParam
                );
                return 1;
            }
    }
    fflush(stdout);
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK HookProcClick(int nCode, WPARAM wParam, LPARAM lParam){
    /*
        Separate autoclicker toggle hook
    */
    KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*) lParam);

    //is 0 if pressed down, 1 if key released
    bool keyUp = hooked_key.flags >> 7;

    if(nCode == HC_ACTION){
        switch(hooked_key.vkCode){
            case(VK_DOWN):
                if(!keyUp)
                {
                    SendMessage(createdWindow, uACToggle, 0, 0);
                }
                return 1;  //1 to block output, 0 to pass through
            }
    }
    fflush(stdout);
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam){
    int ALT = GetAsyncKeyState(VK_MENU);
    HWND activeWindow = GetForegroundWindow();
    MSLLHOOKSTRUCT hooked_mouse = *((MSLLHOOKSTRUCT*) lParam);
    if(nCode == HC_ACTION) {
        ALT = GetAsyncKeyState(VK_MENU);

        // if the mousewheel was rotated up
        if((signed int)hooked_mouse.mouseData>>16 >0  && (ALT&0x8000||ALT&0x0001)){
            // insert the active window under the user's scroll window
            //SetWindowPos(activeWindow,mouseWindow,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW|SWP_NOSENDCHANGING|SWP_DEFERERASE);
            SetFocus(mouseWindow);
            // send the scroll message
            SendMessage(mouseWindow,WM_MOUSEWHEEL,7864320,22938243);
            // return the active window to the top
            //SetWindowPos(activeWindow,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
            SetFocus(activeWindow);
            return 1;
        }

        // mousewheel was rotated down
        else if((signed int)hooked_mouse.mouseData>>16 <0 &&(ALT&0x8000||ALT&0x0001)) {
            SetWindowPos(activeWindow,mouseWindow,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE|SWP_NOREDRAW|SWP_NOSENDCHANGING|SWP_DEFERERASE);
            SendMessage(mouseWindow,WM_MOUSEWHEEL,-7864320,22938243);
            SetWindowPos(activeWindow,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
            return 1;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


DWORD WINAPI autoClickerProcess(LPVOID lpParam){
    wave waveSequence[21];

    // bool autoClicking = *(bool*)lpParam;
    while (true){
        std::cout<<"click variables reset"<<std::endl;
        unsigned int currentClick   = 0;

        int holdChance              = 0,
            moveChance              = 0,
            moveDirection           = 0;

        int xpos                    = 0,
            ypos                    = 0;
        POINT p;

        unsigned int length         = 0;

        waveSequence[21] = {};
        waveSequence[0].InitWave(0, length, 0);


        std::cout << "creating click waves..." << std::endl;
        for(int n=1; n<21; n++){ //create 21 click-waves
            waveSequence[n].InitWave(n, length, waveSequence[n-1].stopValue);
            printf("Click wave %d created\n", n);
        }
        std::cout<< "click waves created" << std::endl;
        unsigned int cosvalue   = 0,
                     plusvalue  = 0,
                     midpoint   = 0,
                     period     = 0,
                     startClick = 0;
        int amplitude           = 0;
        wave activeWave;

        std::cout<<"starting click loop..."<<std::endl;
        // do the clicks
        for (int n=0; n<21; n++){
            activeWave = waveSequence[n];

            amplitude   = activeWave.GetAmp();
            midpoint    = activeWave.GetMid();
            period      = activeWave.GetPeriod();
            startClick  = activeWave.startClick;

            currentClick = startClick;
            while (currentClick < activeWave.stopClick) {
                // The base wait time for out current click
                cosvalue =
                    amplitude *
                    cos(2 * PI * (1 / period) * (currentClick - startClick))
                    + midpoint;
                
                // chance to ignore cosValue and wait for a long time 
                holdChance = rand() % 100 + 1;

                // chance to jiggle cursor
                moveChance = rand() % 300 + 1;

                // returns position of cursor in screen coords
                if (GetCursorPos(&p)){
                    xpos = p.x;
                    ypos = p.y;
                }

                if (moveChance <= 1){
                    // specific value does not mean anything, just equally
                    // weighted 0-7
                    moveDirection = rand() % 8;

                    // screen origin is TOP LEFT PIXEL, so smaller y is higher
                    int xChange = rand() % 3 + 1;
                    int yChange = rand() % 3 + 1;

                    // adjust xChanges
                    if (moveDirection < 3){
                        xChange = -xChange;
                    }
                    else if (moveDirection == 3 || moveDirection == 7){
                        xChange = 0;
                    }

                    // adjust yChanges
                    if (moveDirection > 1 && moveDirection < 5) {
                        yChange = -yChange;
                    }
                    else if (moveDirection == 1 || moveDirection == 5){
                        yChange = 0;
                    }
                    xpos += xChange;
                    ypos += yChange;
                    SetCursorPos(xpos, ypos);
                }

                //regular click
                if (holdChance <= 98){
                    //chance to be above or below cosine value
                    plusvalue = rand() % 2; 

                    mouse_event(MOUSEEVENTF_LEFTDOWN, xpos, ypos, 0, 0);
                    Sleep(rand() % 10 + 1);  // hold mouse left for some time
                    mouse_event(MOUSEEVENTF_LEFTUP, xpos, ypos, 0, 0);

                    // bias click values to be mostly below my calculated vals
                    if(plusvalue == 1) Sleep(cosvalue + rand() % 25);
                    else if(plusvalue == 0) Sleep(cosvalue - rand() % 75);
                    //((rand()%150*(rand()%2*(-1)))+420);
                }
                //delayed click
                else if (holdChance > 98){
                    mouse_event(MOUSEEVENTF_LEFTDOWN, xpos, ypos, 0, 0);
                    Sleep(rand() % 10 + 1);
                    mouse_event(MOUSEEVENTF_LEFTUP, xpos, ypos, 0, 0);
                    Sleep(rand() % 50 + 600);
                }
                currentClick++;
            }// end while (currentClick > totalClicks)
        }
        std::cout<<"click loop ended"<<std::endl;
        //delete[] waveSequence;
    }
    return NULL;
}






