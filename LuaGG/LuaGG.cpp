#include "LuaGG.h"

#include "MainWindow.h"
#include "ChangeStatus.h"

#include "ScriptManager.h"
ScriptManager sm;

Protocol protocol;
LuaScript ls;
Configuration config;
WinGUI gui;

HWND mainDlg, statusDlg, aboutDlg;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_WIN95_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);
	if(!config.loadFile(getFilePath("Config.lua").c_str()))
	{
		gui.messageBox(MESSAGE_TYPE_FATAL_ERROR, "Nie mo¿na za³adowaæ pliku: \"Config.lua\"!\nPlik ten prawdopodobnie nie istnieje!");
		return 0;
	}

	if(!ls.loadScriptsFromFile())
	{
		if(gui.messageBox(MESSAGE_TYPE_FATAL_YES_NO, "Nie mo¿na za³adowaæ pliku: \"Skrypty.xml\"!\nPlik ten prawdopodobnie nie istnieje!\nCzy chcesz utworzyæ nowy plik?"))
			ls.saveScriptsFile();
		else
			return 0;
	}

	WNDCLASSEX wincl;

	wincl.hInstance = hInstance;
	wincl.lpszClassName = "LuaGGClass";
	wincl.lpfnWndProc = NULL;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof(WNDCLASSEX);

	wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON));
	wincl.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON), IMAGE_ICON, 16, 16, 0);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	if(!RegisterClassEx(&wincl))
		return 0;

	gui.hInst = hInstance;

	gui.statusMenu = GetSubMenu(LoadMenu(gui.hInst, MAKEINTRESOURCE(ID_STATUS_MENU)), 0);
	gui.trayMenu = GetSubMenu(LoadMenu(gui.hInst, MAKEINTRESOURCE(ID_TRAY_MENU)), 0);
	gui.mainMenu = GetSubMenu(LoadMenu(gui.hInst, MAKEINTRESOURCE(ID_MENU)), 0);

	CreateDialog(gui.hInst, MAKEINTRESOURCE(ID_DLG_STATUS), NULL, ChangeStatusDlg);
	DialogBoxA(gui.hInst, MAKEINTRESOURCE(ID_DLG), HWND_DESKTOP, MainDlgProc);

	return 0;
}
