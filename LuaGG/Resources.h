#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <Windows.h>

#define BUILD_INFO "Wersja beta"

#define VERSION "1.0.0.0"
#define VERSIONSTR "1.0.0.0\0"
#define VERSIONINT 1,0,0,0

#define WM_SOCKET WM_USER+1
#define WM_SHELLNOTIFY WM_USER+5

#ifndef _MSC_VER
#define SBT_TOOLTIPS 0x0800
#define NIIF_USER 0x00000004
#endif

#define ID_STATIC -1
#define ID_ICON 100
#define ID_ICON_AVAILABLE 101
#define ID_ICON_BUSY 102
#define ID_ICON_INVISIBLE 103
#define ID_ICON_NOT_AVAILABLE 104
#define ID_ICON_FFC 105
#define ID_ICON_DND 106
#define ID_BMP_AVAILABLE 107
#define ID_BMP_BUSY 108
#define ID_BMP_INVISIBLE 109
#define ID_BMP_NOT_AVAILABLE 110
#define ID_BMP_DESC 111
#define ID_BMP_DND 112
#define ID_BMP_FFC 114
#define ID_BMP_CLOSE 115

#define ID_MENU 102
#define ID_TRAY_MENU 103
#define ID_STATUS_MENU 104

#define ID_MENU_EXIT 105
#define ID_MENU_CONNECT 106
#define ID_MENU_TRAY 107
#define ID_MENU_CLEAN_CONSOLE 108
#define ID_MENU_GET_USERLIST 109
#define ID_MENU_CHANGE_STATUS 110
#define ID_MENU_AVAILABLE 111
#define ID_MENU_BUSY 112
#define ID_MENU_HIDDEN 113
#define ID_MENU_NOT_AVAILABLE 114
#define ID_MENU_RELOAD 115
#define ID_MENU_RELOAD_CONFIG 116
#define ID_MENU_ABOUT 117
#define ID_MENU_DND 118
#define ID_MENU_FFC 119

#define ID_DLG 200
#define ID_DLG_CONSOLE 201
#define ID_DLG_ABOUT 202
#define ID_STATUS_BAR 203

#define ID_DLG_STATUS 300
#define ID_DLG_AVAILABLE 301
#define ID_DLG_BUSY 302
#define ID_DLG_HIDDEN 303
#define ID_DLG_NOT_AVAILABLE 304
#define ID_DLG_DESC 305
#define ID_DLG_SET 306
#define ID_DLG_CLOSE 307
#define ID_DLG_FFC 308
#define ID_DLG_DND 309

#define ID_DLG_VERSION 310
#define ID_DLG_PROTOCOL 311
#define ID_DLG_LUA 312
#define ID_DLG_BOOST 313
#define ID_DLG_PAGE 314

#define ID_DLG_LISTVIEW 400

#define ID_TIMER_BEGIN 1000

#endif
