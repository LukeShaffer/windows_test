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
        WS_EX_CLIENTEDGE,   // dwExStyle
        CLASS_NAME,         // lpClassName
        L"Hotkey Window",   // lpWindowName
        WS_OVERLAPPEDWINDOW |WS_HSCROLL |WS_VSCROLL,    // dwStyle
        0,0,250,250,    // x, y, width, height
        NULL,   // hWndParent
        NULL,   // hMenu
        hInstance, // hInstance
        NULL    // lpParam
    );

    if(createdWindow == NULL){
        return -1;
    }

    HINSTANCE hInst = (HINSTANCE)GetWindowLong(createdWindow, GWL_HINSTANCE );

    //hotkey section
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
    //std::default_random_engine generator;
    //std::normal_distribution<double> distribution(5.0,2.0);
    //double number = distribution(generator); //how to pick random numbers from the distribution


    // disable showing the window
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
    //toggles auto clicker as clicking or not clicking
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
                (LPVOID)&clickingLoop,       // __drv_aliasesMem LPVOID lpParameter
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
        The code defining what happens in the windows keyboard hook
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
        else if((signed int)hooked_mouse.mouseData>>16 <0 &&(ALT&0x8000||ALT&0x0001)) //if the mousewheel was rotated down
        {
            SetWindowPos(activeWindow,mouseWindow,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE|SWP_NOREDRAW|SWP_NOSENDCHANGING|SWP_DEFERERASE);
            SendMessage(mouseWindow,WM_MOUSEWHEEL,-7864320,22938243);
            SetWindowPos(activeWindow,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
            return 1;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


DWORD WINAPI autoClickerProcess(LPVOID lpParam){
    wave wavesequence[21];

    bool autoClicking = *(bool*)lpParam;
    while (autoClicking){
        if(autoClicking == false){
            delete[] wavesequence;
            return NULL;
        }

        std::cout<<"click variables reset"<<std::endl;
        unsigned int totalclicks =0;
        unsigned int clicks = 0;

        int holdchance      = 0;
        int movechance      = 0;
        int movedirection   = 0;

        int xpos            = 0,
            ypos            = 0;
        POINT p;

        unsigned int n      = 0;
        unsigned int length = 0;

        n =0;
        length = 0;

        wavesequence[21] = {};
        wavesequence[0].InitWave(n,length,0);


         std::cout<<"creating click waves..."<<std::endl;
        for(n=1;n<21;n++){ //create 21 click-waves
            wavesequence[n].InitWave(n,length,wavesequence[n-1].stopValue);
            printf("Click wave %d created\n",n);
        }
        std::cout<<"click waves created"<<std::endl;
        totalclicks = wavesequence[n-1].stopClick;

            unsigned int cosvalue  = 0;
            unsigned int plusvalue = 0;
            int amplitude          = 0;
            unsigned int midpoint  = 0;
            unsigned int period    = 0;
            int startclick         = 0;
            wave activewave;

        std::cout<<"starting click loop..."<<std::endl;
        while(clicks<totalclicks) //do the clicks
        {
            if(autoClicking == false){
                std::cout<<"Input detected, ending click loop"<<std::endl;
                delete[] wavesequence;
                return NULL;
            }

            holdchance = rand()%100+1;
            movechance = rand()%300+1;


            for(n=0;n<21;n++){
               if(wavesequence[n].stopClick>clicks){
                break;
               }
            }

           amplitude   = wavesequence[n].GetAmp();
           midpoint    = wavesequence[n].GetMid();
           period      = wavesequence[n].GetPeriod();
           startclick  = wavesequence[n].startClick;
           cosvalue    = (amplitude*cos(2*3.14159*(1/period)*(clicks-startclick))+midpoint);

            if (GetCursorPos(&p)){
                xpos = p.x;
                ypos = p.y;
            }
            if (movechance<=1){
                movedirection = rand()%8;


                switch(movedirection){
                    case(0):{   //move left
                        xpos-=rand()%3+1;
                        break;
                    }
                    case(1):{ //move right
                        xpos+=rand()%3+1;
                        break;
                    }
                    case(2):{  // move up
                        ypos-= rand()%3+1;
                        break;
                    }
                    case(3):{ //move down
                        ypos+=rand()%3+1;
                        break;
                    }
                    case(4):{  //move up-left
                        xpos-=rand()%3+1;
                        ypos-=rand()%3+1;
                        break;
                    }
                    case(5):{ //move up-right
                        xpos+=rand()%3+1;
                        ypos-=rand()%3+1;
                        break;
                    }
                    case(6):{ //move down-right
                        xpos+=rand()%3+1;
                        ypos+=rand()%3+1;
                        break;
                    }
                    case(7):{ //move down-left
                        xpos-=rand()%3+1;
                        ypos+=rand()%3+1;
                        break;
                    }
                }

            }
            SetCursorPos(xpos,ypos);


            if (holdchance<=98){ //regular click
                plusvalue  = rand()%2; //chance to be above or below cosine value
                mouse_event(MOUSEEVENTF_LEFTDOWN,xpos,ypos,0,0);
                Sleep(rand()%10+1);
                mouse_event(MOUSEEVENTF_LEFTUP,xpos,ypos,0,0);

                if(plusvalue   ==1){
                Sleep( cosvalue+rand()%25);
                }

                if(plusvalue==0){
                    Sleep(cosvalue-rand()%75);
                }
                //((rand()%150*(rand()%2*(-1)))+420);
                clicks++;
            }


            else if (holdchance>98){//delayed click
                mouse_event(MOUSEEVENTF_LEFTDOWN,xpos,ypos,0,0);
                Sleep(rand()%10+1);
                mouse_event(MOUSEEVENTF_LEFTUP,xpos,ypos,0,0);
                Sleep(rand()%50+600);
                clicks++;
            }

        }// end while (clicks < totalClicks)
        std::cout<<"click loop ended"<<std::endl;
        clicks =0;
        //delete[] wavesequence;
    }//end of while (autoClicking)
    // just to have a return value
    return NULL;
}






