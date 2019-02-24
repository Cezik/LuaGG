#include "AboutDlg.h"

BOOL CALLBACK AboutWindowDlg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	aboutDlg = hWnd;
	switch(message)
	{
		case WM_INITDIALOG:
			char buffer[10];
			sprintf(buffer, " v%s", VERSION);
			SetDlgItemText(aboutDlg, ID_DLG_VERSION, buffer);
			break;

		case WM_NOTIFY:
			switch(((LPNMHDR)lParam)->code)
			{
				case NM_CLICK:
					switch(wParam)
					{
						case ID_DLG_PROTOCOL:
							ShellExecute(NULL, "open", "http://toxygen.net/libgadu/protocol/", NULL, NULL, SW_SHOW);
							break;

						case ID_DLG_LUA:
							ShellExecute(NULL, "open", "http://www.lua.org/", NULL, NULL, SW_SHOW);
							break;

						case ID_DLG_BOOST:
							ShellExecute(NULL, "open", "http://www.boost.org/", NULL, NULL, SW_SHOW);
							break;

						case ID_DLG_PAGE:
							ShellExecute(NULL, "open", "http://czepekpage.pl/", NULL, NULL, SW_SHOW);
							break;
					}
					break;
			}
			break;

		case WM_CLOSE:
		case WM_DESTROY:
			EndDialog(aboutDlg, 0);
			break;
	}
	return false;
}
