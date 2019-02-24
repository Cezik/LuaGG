#ifndef __WINGUI_H__
#define __WINGUI_H__

#include <Windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <Psapi.h>
#include <string>

#include "LuaGG.h"
#include "Resources.h"

enum MessageBoxType_t
{
	MESSAGE_TYPE_FATAL_ERROR,
	MESSAGE_TYPE_ERROR,
	MESSAGE_TYPE_INFO,
	MESSAGE_TYPE_NO_ICON,
	MESSAGE_TYPE_YES_NO,
	MESSAGE_TYPE_FATAL_YES_NO
};

class WinGUI
{
	public:
		WinGUI();
		~WinGUI();

		void createToolTip(HWND hParent, const char* tipText, ...);

		void addLineToLabel(HWND hwnd, int ResourceID, const char* text, ...);
		bool messageBox(MessageBoxType_t type, const char* text, ...);
		void doShowMenu(HWND hWnd, HMENU rMenu);

		void doSetItem(HWND hLview, char* szBuffer, int nItem, int nSubItem);
		void doCreateItem(HWND hLview, char* szBuffer, int nItem, int nSubItem, int nData);
		void doCreateColumn(HWND hLview, char* szBuffer, int nWidth, int nColumnNumber);
		HWND doCreateListView(HWND hParent, int x, int y, int nWidth, int nHeight);

		HMENU trayMenu, mainMenu, statusMenu;
		bool minimized;
		NOTIFYICONDATA trayIcon;
		HINSTANCE hInst;
		LVITEM lvi;
};

#endif
