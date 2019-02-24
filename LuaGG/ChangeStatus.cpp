#include "ChangeStatus.h"

extern Protocol protocol;
extern Configuration config;
extern WinGUI gui;

char lastStatus[70];

BOOL CALLBACK ChangeStatusDlg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	statusDlg = hWnd;
	switch(message)
	{
		case WM_INITDIALOG:
		{
			SetDlgItemText(statusDlg, ID_DLG_DESC, config.getString(Configuration::LOGIN_DESCRIPTION).c_str());
			gui.createToolTip(GetDlgItem(statusDlg, ID_DLG_FFC), "Po zaznaczeniu Twój opis zostanie zmieniony na opisowy-poGGadaj ze mn¹.");
			gui.createToolTip(GetDlgItem(statusDlg, ID_DLG_AVAILABLE), "Po zaznaczeniu Twój opis zostanie zmieniony na opisowy-dostêpny.");
			gui.createToolTip(GetDlgItem(statusDlg, ID_DLG_BUSY), "Po zaznaczeniu Twój opis zostanie zmieniony na opisowy-zaraz wracam.");
			gui.createToolTip(GetDlgItem(statusDlg, ID_DLG_DND), "Po zaznaczeniu Twój opis zostanie zmieniony na opisowy-nie przeszkadzaæ.");
			gui.createToolTip(GetDlgItem(statusDlg, ID_DLG_HIDDEN), "Po zaznaczeniu Twój opis zostanie zmieniony na opisowy-niewidoczny.");
			gui.createToolTip(GetDlgItem(statusDlg, ID_DLG_NOT_AVAILABLE), "Po zaznaczeniu Twój opis zostanie zmieniony na opisowy-niedostêpny.");
			gui.createToolTip(GetDlgItem(statusDlg, ID_DLG_DESC), "Opis, który zostanie ustawiony.\nMaksymalnie 70 znaków.");
			gui.createToolTip(GetDlgItem(statusDlg, ID_DLG_SET), "Przycisk, który ustawia opis.");
			break;
		}

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case ID_DLG_CLOSE:
					SendMessage(statusDlg, WM_DESTROY, 0, 0);
					break;

				case ID_DLG_SET:
				{
					Status_t status = GG_STATUS_NOT_AVAILABLE_DESCR;
					int statIcon = 0;
					if(SendDlgItemMessage(statusDlg, ID_DLG_AVAILABLE, BM_GETCHECK, 0, 0))
					{
						status = GG_STATUS_AVAILABLE_DESCR;
						statIcon = ID_ICON_AVAILABLE;
					}
					else if(SendDlgItemMessage(statusDlg, ID_DLG_BUSY, BM_GETCHECK, 0, 0))
					{
						status = GG_STATUS_BUSY_DESCR;
						statIcon = ID_ICON_BUSY;
					}
					else if(SendDlgItemMessage(statusDlg, ID_DLG_HIDDEN, BM_GETCHECK, 0, 0))
					{
						status = GG_STATUS_INVISIBLE_DESCR;
						statIcon = ID_ICON_INVISIBLE;
					}
					else if(SendDlgItemMessage(statusDlg, ID_DLG_NOT_AVAILABLE, BM_GETCHECK, 0, 0))
					{
						status = GG_STATUS_NOT_AVAILABLE_DESCR;
						statIcon = ID_ICON_NOT_AVAILABLE;
					}
					else if(SendDlgItemMessage(statusDlg, ID_DLG_FFC, BM_GETCHECK, 0, 0))
					{
						status = GG_STATUS_FFC_DESCR;
						statIcon = ID_ICON_FFC;
					}
					else if(SendDlgItemMessage(statusDlg, ID_DLG_DND, BM_GETCHECK, 0, 0))
					{
						status = GG_STATUS_DND_DESCR;
						statIcon = ID_ICON_DND;
					}
					GetDlgItemText(statusDlg, ID_DLG_DESC, lastStatus, sizeof(lastStatus));
					if(protocol.changeStatus(status, lastStatus))
					{
						SendDlgItemMessage(statusDlg, ID_DLG_DESC, WM_SETTEXT, 0, (LPARAM)lastStatus);
						SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[statIcon]);
						SendMessage(statusDlg, WM_DESTROY, 0, 0);
					}
					break;
				}
			}
			break;
		}

		case WM_CLOSE:
		case WM_DESTROY:
			ShowWindow(statusDlg, SW_HIDE);
			break;
	}
	return false;
}
