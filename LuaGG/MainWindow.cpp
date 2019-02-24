#include "MainWindow.h"

#include "Threads.h"
#include "ChangeStatus.h"
#include "AboutDlg.h"
#include "ScriptManager.h"

extern WinGUI gui;
extern Protocol protocol;
extern Configuration config;
extern LuaScript ls;
extern ScriptManager sm;

HICON statusIcon[ID_ICON_DND];
HBITMAP statusBmp[ID_BMP_CLOSE+1];

BOOL CALLBACK MainDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	mainDlg = hWnd;
	switch(message)
	{
		case WM_INITDIALOG:
		{
			char buffer[100];
			sprintf(buffer, ">> LuaGG %s", VERSION);
			SetDlgItemText(mainDlg, ID_DLG_CONSOLE, buffer);
			addConsoleMsg(">> Skompilowane przez %s dnia %s o godzinie %s", BOOST_COMPILER, __DATE__, __TIME__);
			addConsoleMsg(">> Bot dla Gadu-Gadu napisany przez: Czepek");

			gui.trayIcon.cbSize = sizeof(NOTIFYICONDATA);
			gui.trayIcon.hWnd = mainDlg;
			gui.trayIcon.uID = ID_ICON;
			gui.trayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
			gui.trayIcon.uCallbackMessage = WM_SHELLNOTIFY;
			gui.trayIcon.hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON), IMAGE_ICON, 24, 24, 0);
			sprintf(buffer, "LuaGG - Za³adowany numer: %i", (int32_t)config.getNumber(Configuration::LOGIN_NUMBER));
			strcpy(gui.trayIcon.szTip, buffer);
			strcpy(gui.trayIcon.szInfoTitle, "LuaGG");
			sprintf(buffer, "LuaGG wydanie %s\nNapisane przez Czepek (czepek221@gmail.com)\n\nOpis protoko³u: http://toxygen.net/libgadu/protocol", VERSION);
			strcpy(gui.trayIcon.szInfo, buffer);
			gui.trayIcon.dwInfoFlags = NIIF_USER | NIIF_NOSOUND;

			Shell_NotifyIcon(NIM_ADD, &gui.trayIcon);

			int iStatusBarWidths[] = {18, -1};
			SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETPARTS, 2, (LPARAM)iStatusBarWidths);
			for(int i = ID_ICON_AVAILABLE; i < ID_ICON_DND+1; i++)
				statusIcon[i] = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(i), IMAGE_ICON, 16, 16, LR_SHARED | LR_LOADTRANSPARENT);
			SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_NOT_AVAILABLE]);
			if(config.getBoolean(Configuration::SHOW_TOOLTIPS))
				SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETTIPTEXT, 0, (LPARAM)"Zmiana statusu");

			setStatusBar(">> LuaGG - Za³adowany numer: %i", config.getNumber(Configuration::LOGIN_NUMBER));

			for(int i = ID_BMP_AVAILABLE; i < ID_BMP_CLOSE+1; i++)
				statusBmp[i] = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(i), IMAGE_BITMAP, 13, 13, LR_LOADTRANSPARENT);

			SetMenuItemBitmaps(gui.statusMenu, ID_MENU_FFC, MF_BYCOMMAND, statusBmp[ID_BMP_FFC], statusBmp[ID_BMP_FFC]);
			SetMenuItemBitmaps(gui.statusMenu, ID_MENU_AVAILABLE, MF_BYCOMMAND, statusBmp[ID_BMP_AVAILABLE], statusBmp[ID_BMP_AVAILABLE]);
			SetMenuItemBitmaps(gui.statusMenu, ID_MENU_BUSY, MF_BYCOMMAND, statusBmp[ID_BMP_BUSY], statusBmp[ID_BMP_BUSY]);
			SetMenuItemBitmaps(gui.statusMenu, ID_MENU_DND, MF_BYCOMMAND, statusBmp[ID_BMP_DND], statusBmp[ID_BMP_DND]);
			SetMenuItemBitmaps(gui.statusMenu, ID_MENU_HIDDEN, MF_BYCOMMAND, statusBmp[ID_BMP_INVISIBLE], statusBmp[ID_BMP_INVISIBLE]);
			SetMenuItemBitmaps(gui.statusMenu, ID_MENU_NOT_AVAILABLE, MF_BYCOMMAND, statusBmp[ID_BMP_NOT_AVAILABLE], statusBmp[ID_BMP_NOT_AVAILABLE]);
			SetMenuItemBitmaps(gui.statusMenu, ID_MENU_CHANGE_STATUS, MF_BYCOMMAND, statusBmp[ID_BMP_DESC], statusBmp[ID_BMP_DESC]);

			SetMenuItemBitmaps(gui.trayMenu, ID_MENU_FFC, MF_BYCOMMAND, statusBmp[ID_BMP_FFC], statusBmp[ID_BMP_FFC]);
			SetMenuItemBitmaps(gui.trayMenu, ID_MENU_AVAILABLE, MF_BYCOMMAND, statusBmp[ID_BMP_AVAILABLE], statusBmp[ID_BMP_AVAILABLE]);
			SetMenuItemBitmaps(gui.trayMenu, ID_MENU_BUSY, MF_BYCOMMAND, statusBmp[ID_BMP_BUSY], statusBmp[ID_BMP_BUSY]);
			SetMenuItemBitmaps(gui.trayMenu, ID_MENU_DND, MF_BYCOMMAND, statusBmp[ID_BMP_DND], statusBmp[ID_BMP_DND]);
			SetMenuItemBitmaps(gui.trayMenu, ID_MENU_HIDDEN, MF_BYCOMMAND, statusBmp[ID_BMP_INVISIBLE], statusBmp[ID_BMP_INVISIBLE]);
			SetMenuItemBitmaps(gui.trayMenu, ID_MENU_NOT_AVAILABLE, MF_BYCOMMAND, statusBmp[ID_BMP_NOT_AVAILABLE], statusBmp[ID_BMP_NOT_AVAILABLE]);
			SetMenuItemBitmaps(gui.trayMenu, ID_MENU_CHANGE_STATUS, MF_BYCOMMAND, statusBmp[ID_BMP_DESC], statusBmp[ID_BMP_DESC]);
			SetMenuItemBitmaps(gui.trayMenu, ID_MENU_EXIT, MF_BYCOMMAND, statusBmp[ID_BMP_CLOSE], statusBmp[ID_BMP_CLOSE]);

			ls.registerTimers(mainDlg);
			sm.reloadMods();
			break;
		}

		case WM_NOTIFY:
			switch(((LPNMHDR)lParam)->code)
			{
				case NM_RCLICK:
					if(((LPNMHDR)lParam)->hwndFrom == GetDlgItem(mainDlg, ID_STATUS_BAR))
						gui.doShowMenu(mainDlg, gui.statusMenu);
					break;
			}
			break;

		case WM_TIMER:
		{
			for(std::vector<LuaScripter>::iterator it = ls.getLuaScripts().begin(); it != ls.getLuaScripts().end(); it++)
			{
				if(wParam == (uint32_t)ID_TIMER_BEGIN + it->id)
					ls.loadScript(it->filePath);
			}
			break;
		}

		case WM_SHELLNOTIFY:
			if(wParam == ID_ICON)
			{
				if(lParam == WM_RBUTTONDOWN)
					gui.doShowMenu(mainDlg, gui.trayMenu);
				else if(lParam == WM_LBUTTONDOWN)
				{
					if(gui.minimized)
					{
						ShowWindow(mainDlg, SW_SHOW);
						ShowWindow(mainDlg, SW_RESTORE);
						ModifyMenu(gui.trayMenu, ID_MENU_TRAY, MF_STRING, ID_MENU_TRAY, "&Ukryj okno");
						gui.minimized = false;
					}
					else
					{
						ShowWindow(mainDlg, SW_HIDE);
						ModifyMenu(gui.trayMenu, ID_MENU_TRAY, MF_STRING, ID_MENU_TRAY, "&Poka¿ okno");
						gui.minimized = true;
					}
				}
			}
			break;

		case WM_SIZE:
			if(wParam == SIZE_MINIMIZED)
			{
				gui.minimized = true;
				ShowWindow(mainDlg, SW_HIDE);
				ModifyMenu(gui.trayMenu, ID_MENU_TRAY, MF_STRING, ID_MENU_TRAY, "&Poka¿ okno");
			}
			else
			{
				RECT rcStatus;
				int32_t iStatusHeight, iEditHeight;
				RECT rcClient;
				SendDlgItemMessage(mainDlg, ID_STATUS_BAR, WM_SIZE, 0, 0);
				GetWindowRect(GetDlgItem(mainDlg, ID_STATUS_BAR), &rcStatus);
				iStatusHeight = rcStatus.bottom - rcStatus.top;
				GetClientRect(mainDlg, &rcClient);
				iEditHeight = rcClient.bottom - iStatusHeight;
				SetWindowPos(GetDlgItem(mainDlg, ID_STATUS_BAR), NULL, 0, rcClient.top, rcClient.right, iEditHeight, SWP_NOZORDER);
			}
			break;

		case WM_SOCKET:
		{
			if(WSAGETSELECTERROR(lParam))
			{
				gui.messageBox(MESSAGE_TYPE_FATAL_ERROR, "B³¹d podczas funkcji: WSAAsyncSelect()\nID B³êdu: %i", protocol.getConnection()->getError());
			}

			switch(WSAGETSELECTEVENT(lParam))
			{
				case FD_WRITE:
					CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendPingThread, mainDlg, 0, 0);
					break;

				case FD_READ:
					if(protocol.isConnected())
						protocol.receivePackets();
					break;

				case FD_CLOSE:
					if(protocol.getLastPacket().type == GG_DISCONNECTING || protocol.getLastPacket().type == GG_DISCONNECTING_ACK)
						delete protocol.getConnection();
					break;
			}
			break;
		}

		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch(wmId)
			{
				case ID_MENU_EXIT:
					SendMessage(mainDlg, WM_DESTROY, 0, 0);
					break;

				case ID_MENU_CONNECT:
					if(!protocol.isLoggedIn())
					{
						CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)connectToServer, mainDlg, 0, 0);
					}
					else
						addConsoleMsg(">> Jesteœ ju¿ zalogowany!");
					break;

				case ID_MENU_GET_USERLIST:
					protocol.requestForUserList();
					break;

				case ID_MENU_TRAY:
				{
					if(gui.minimized)
					{
						ShowWindow(mainDlg, SW_SHOW);
						ShowWindow(mainDlg, SW_RESTORE);
						ModifyMenu(gui.trayMenu, ID_MENU_TRAY, MF_STRING, ID_MENU_TRAY, "&Ukryj okno");
						gui.minimized = false;
					}
					else
					{
						ShowWindow(mainDlg, SW_HIDE);
						ModifyMenu(gui.trayMenu, ID_MENU_TRAY, MF_STRING, ID_MENU_TRAY, "&Poka¿ okno");
						gui.minimized = true;
					}
					break;
				}

				case ID_MENU_CLEAN_CONSOLE:
					char buffer[100];
					sprintf(buffer, ">> LuaGG %s - created by Czepek", VERSION);
					SetDlgItemText(mainDlg, ID_DLG_CONSOLE, buffer);
					break;

				case ID_MENU_CHANGE_STATUS:
					ShowWindow(statusDlg, SW_SHOW);
					break;

				case ID_MENU_AVAILABLE:
					if(protocol.changeStatus(GG_STATUS_AVAILABLE, NULL))
						SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_AVAILABLE]);
					break;

				case ID_MENU_BUSY:
					if(protocol.changeStatus(GG_STATUS_BUSY, NULL))
						SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_BUSY]);
					break;

				case ID_MENU_DND:
					if(protocol.changeStatus(GG_STATUS_DND, NULL))
						SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_DND]);
					break;

				case ID_MENU_FFC:
					if(protocol.changeStatus(GG_STATUS_FFC, NULL))
						SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_FFC]);
					break;

				case ID_MENU_HIDDEN:
					if(protocol.changeStatus(GG_STATUS_INVISIBLE, NULL))
						SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_INVISIBLE]);
					break;

				case ID_MENU_NOT_AVAILABLE:
					if(protocol.changeStatus(GG_STATUS_NOT_AVAILABLE, NULL))
						SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_NOT_AVAILABLE]);
					break;

				case ID_MENU_RELOAD:
					sm.reloadMods();
					break;

				case ID_MENU_RELOAD_CONFIG:
					if(!config.reload())
						addConsoleMsg(">> Nie mo¿na prze³adowaæ pliku 'Config.lua'");
					else
					{
						setStatusBar(">> LuaGG - Za³adowany numer: %i", config.getNumber(Configuration::LOGIN_NUMBER));
						addConsoleMsg(">> Plik konfiguracyjny zosta³ prze³adowany!");
					}
					break;

				case ID_MENU_ABOUT:
					DialogBoxA(gui.hInst, MAKEINTRESOURCE(ID_DLG_ABOUT), mainDlg, AboutWindowDlg);
					break;
			}
			break;

		case WM_CLOSE:
			ShowWindow(mainDlg, SW_HIDE);
			ModifyMenu(gui.trayMenu, ID_MENU_TRAY, MF_STRING, ID_MENU_TRAY, "&Poka¿ okno");
			gui.minimized = true;
			break;

		case WM_DESTROY:
			ls.unRegisterTimers(mainDlg);
			protocol.changeStatus(GG_STATUS_NOT_AVAILABLE, NULL);
			DestroyMenu(gui.trayMenu);
			Shell_NotifyIcon(NIM_DELETE, &gui.trayIcon);
			PostQuitMessage(0);
			break;
	}
	return false;
}
