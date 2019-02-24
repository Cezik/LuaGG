#include "Threads.h"

extern Protocol protocol;
extern Configuration config;

DWORD sendPingThread()
{
	const uint32_t sendPingTime = config.getNumber(Configuration::SEND_PING_TIME);
	do
	{
		if(protocol.sendPing())
			Sleep(sendPingTime);
	}while(protocol.isLoggedIn());
	return 0;
}

DWORD connectToServer()
{
	const uint32_t ggNumber = (uint32_t)config.getNumber(Configuration::LOGIN_NUMBER);
	const std::string ggPassword = config.getString(Configuration::LOGIN_PASSWORD);
	Status_t status = (Status_t)config.getNumber(Configuration::LOGIN_STATUS);
	const std::string ggDesc = config.getString(Configuration::LOGIN_DESCRIPTION);
	uint32_t stat = 0;

	if(status <= 0 || status > 6)
	{
		addConsoleMsg(">> Status w pliku konfiguracyjnym jest niepoprawny!");
		addConsoleMsg(">> Status, który zostanie u¿yty: Dostêpny (1)");
		status = GG_STATUS_AVAILABLE;
		stat = ID_ICON_AVAILABLE;
	}
	else if(status == 1)
	{
		status = GG_STATUS_FFC;
		stat = ID_ICON_AVAILABLE;
	}
	else if(status == 2)
	{
		status = GG_STATUS_AVAILABLE;
		stat = ID_ICON_AVAILABLE;
	}
	else if(status == 3)
	{
		status = GG_STATUS_BUSY;
		stat = ID_ICON_BUSY;
	}
	else if(status == 4)
	{
		status = GG_STATUS_INVISIBLE;
		stat = ID_ICON_INVISIBLE;
	}
	else if(status == 5)
	{
		status = GG_STATUS_DND;
		stat = ID_ICON_DND;
	}
	else if(status == 6)
	{
		status = GG_STATUS_FFC_DESCR;
		stat = ID_ICON_FFC;
	}
	else if(status == 7)
	{
		status = GG_STATUS_AVAILABLE_DESCR;
		stat = ID_ICON_AVAILABLE;
	}
	else if(status == 8)
	{
		status = GG_STATUS_BUSY_DESCR;
		stat = ID_ICON_BUSY;
	}
	else if(status == 9)
	{
		status = GG_STATUS_DND_DESCR;
		stat = ID_ICON_DND;
	}
	else if(status == 10)
	{
		status = GG_STATUS_INVISIBLE_DESCR;
		stat = ID_ICON_INVISIBLE;
	}

	if(protocol.login(ggNumber, ggPassword.c_str(), (Status_t)status, ggDesc.c_str()))
	{
		addConsoleMsg(">> GRATULACJE! Zosta³eœ zalogowany do serwera!");
		SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[stat]);
		WSAAsyncSelect(protocol.getRealSocket(), mainDlg, WM_SOCKET, (FD_READ | FD_WRITE | FD_CLOSE));
	}
	else
		protocol.setLoginTries(protocol.getLoginTries()+1);
	return 0;
}
