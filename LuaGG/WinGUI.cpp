#include "WinGUI.h"

extern Configuration config;

WinGUI::WinGUI()
{
	//
}

WinGUI::~WinGUI()
{
	//
}

void WinGUI::createToolTip(HWND hParent, const char* tipText, ...)
{
	if(!config.getBoolean(Configuration::SHOW_TOOLTIPS))
		return;

	if(!hParent || !tipText)
		return;

	char buffer[2048];
	va_list list;
	va_start(list, tipText);
	_vsnprintf(buffer, sizeof(buffer), tipText, list);
	va_end(list);

	HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_BALLOON | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hParent, NULL, hInst, NULL);
	SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
	ti.hwnd = mainDlg;
	ti.hinst = NULL;
	ti.uId = (UINT_PTR)hParent;
	ti.lpszText = (LPSTR)buffer;
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, 275);
}

void WinGUI::addLineToLabel(HWND hwnd, int ResourceID, const char* text, ...)
{
	char oldBuffer[4096], newBuffer[16384], buffer[8192];
	va_list list;
	va_start(list, text);
	_vsnprintf(buffer, sizeof(buffer), text, list);
	va_end(list);

	GetDlgItemText(hwnd, ResourceID, oldBuffer, sizeof(oldBuffer));
	sprintf(newBuffer, "%s\r\n%s", oldBuffer, buffer);
	SetDlgItemText(hwnd, ResourceID, newBuffer);
}

bool WinGUI::messageBox(MessageBoxType_t type, const char* text, ...)
{
	va_list list;
	va_start(list, text);
	char buffer[2048];
	_vsnprintf(buffer, sizeof(buffer), text, list);
	va_end(list);

	UINT mType;
	switch(type)
	{
		case MESSAGE_TYPE_FATAL_ERROR:
			mType = MB_OK | MB_ICONERROR;
			break;

		case MESSAGE_TYPE_ERROR:
			mType = MB_OK | MB_ICONEXCLAMATION;
			break;

		case MESSAGE_TYPE_INFO:
			mType = MB_OK | MB_ICONINFORMATION;
			break;

		case MESSAGE_TYPE_NO_ICON:
			mType = MB_OK;
			break;

		case MESSAGE_TYPE_YES_NO:
			mType = MB_YESNO;
			if(MessageBoxA(HWND_DESKTOP, buffer, "LuaGG", mType) == IDYES)
				return true;
			else
				return false;
			break;

		case MESSAGE_TYPE_FATAL_YES_NO:
			mType = MB_YESNO | MB_ICONERROR;
			if(MessageBoxA(HWND_DESKTOP, buffer, "LuaGG", mType) == IDYES)
				return true;
			else
				return false;
			break;
		

		default:
			mType = MB_OK;
			break;
	}
	MessageBoxA(HWND_DESKTOP, buffer, "LuaGG", mType);
	return true;
}

void WinGUI::doShowMenu(HWND hWnd, HMENU rMenu)
{
	POINT pMouse;
	GetCursorPos(&pMouse);
	SetForegroundWindow(hWnd);
	TrackPopupMenu(rMenu, TPM_LEFTALIGN, pMouse.x, pMouse.y, 0, hWnd, 0);
	PostMessage(hWnd, WM_NULL, 0, 0);
}

void WinGUI::doSetItem(HWND hLview, char* szBuffer, int nItem, int nSubItem)
{
	lvi.state = 0;
	lvi.stateMask = 0;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = nItem;
	lvi.iSubItem = nSubItem;
	lvi.pszText = szBuffer;
	ListView_SetItem(hLview, &lvi);
}

void WinGUI::doCreateItem(HWND hLview, char* szBuffer, int nItem, int nSubItem, int nData)
{
	lvi.state = 0;
	lvi.stateMask = 0;

	if(nSubItem > 0)
		lvi.mask = LVIF_TEXT;
	else
	{
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.lParam = (LPARAM)nData;
	}

	lvi.iItem = nItem;
	lvi.iSubItem = nSubItem;
	lvi.pszText = szBuffer;

	if(nSubItem > 0)
		ListView_SetItem(hLview, &lvi);
	else
		ListView_InsertItem(hLview, &lvi);
}

void WinGUI::doCreateColumn(HWND hLview, char* szBuffer, int nWidth, int nColumnNumber)
{
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvc.iSubItem = nColumnNumber;
	lvc.pszText = szBuffer;
	lvc.cx = nWidth;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(hLview, nColumnNumber, &lvc);
}

HWND WinGUI::doCreateListView(HWND hParent, int x, int y, int nWidth, int nHeight)
{
	HWND hLview;
	RECT rParent;
	char szBuffer[MAX_PATH];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	GetClientRect(hParent, &rParent);
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	if(!InitCommonControlsEx(&icex))
		messageBox(MESSAGE_TYPE_FATAL_ERROR, "Error while creating user interface.");

	hLview = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_AUTOARRANGE | WS_BORDER | LVS_SHOWSELALWAYS, x, y, nWidth, nHeight, hParent, (HMENU)ID_DLG_LISTVIEW, 0, 0);

	ListView_SetExtendedListViewStyle(hLview, LVS_EX_FULLROWSELECT);
	return hLview;
}
