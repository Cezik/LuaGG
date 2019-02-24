#include <cstdlib>
#include <iostream>
#include <string>

#include "Resources.h"
#include "Protocol.h"
#include "LuaScript.h"
#include "Configuration.h"
#include "WinGUI.h"
#include "GaduGadu.h"
#include "Connection.h"

#include <commctrl.h>

extern HWND mainDlg;
extern HWND statusDlg;
extern HWND aboutDlg;
extern HICON statusIcon[ID_ICON_DND];

#ifdef _MSC_VER
typedef signed long long int64_t;
typedef unsigned long uint32_t;
typedef signed long int32_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;
#endif
