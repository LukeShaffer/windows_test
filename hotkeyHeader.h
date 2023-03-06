#ifndef HOTKEYHEADER_H_INCLUDED
#define HOTKEYHEADER_H_INCLUDED

#include <windows.h>
#include <iostream>
#include <cstdio>


void HotKeyProcess(MSG,int&, bool&, bool&,bool&,bool&,HINSTANCE,HWND&);
void autoClickerProcess(bool&);
void wasdProcess(MSG&);
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HookProcClick(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);


    static HHOOK hhookKey;
    static HHOOK hhookClicker;
    static HHOOK hhookMouse;
    static MSG hookMsg = {};
    static MSG mouseHookMsg = {};




#endif // HOTKEYHEADER_H_INCLUDED
