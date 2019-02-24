#include "Protocol.h"
#include "Configuration.h"
#include "Threads.h"

extern LuaScript ls;
extern WinGUI gui;
extern Protocol protocol;
extern Configuration config;

Protocol::Protocol()
{
	loggedIn = false;
	connected = false;
	currentStatus = GG_STATUS_NOT_AVAILABLE;
	loginTries = 0;
	msgCount = 0;
	trueLoginServer = new char[255];
	lastSendMsg = new char[2000];
	lastRecvMsg = new char[2000];
}

Protocol::~Protocol()
{
	//
}

SOCKET Protocol::getRealSocket() const
{
	return getConnection()->getSocket();
}

const char* Protocol::gg_sha_hash(const char* password, uint32_t seed)
{
	SHA_CTX ctx;
	static char result[20];
	SHA1_Init(&ctx);
	SHA1_Update(&ctx, password, strlen(password));
	SHA1_Update(&ctx, &seed, sizeof(seed));
	SHA1_Final((uint8_t*)result, &ctx);
	return result;
}

bool Protocol::getTrueLoginServer(uint32_t ggNumber)
{
	Connection* con = new Connection();
	if(!con->connectTo("appmsg.gadu-gadu.pl", 80))
	{
		delete con;
		strcpy(trueLoginServer, "");
		return false;
	}

	char sendBuffer[512];
	sprintf(sendBuffer, "GET /appsvc/appmsg_ver8.asp?"
						"fmnumber='%i'&version='7.7.3315'&lastmsg='' HTTP/1.1\r\n"
						"Connection: Keep-Alive\r\n"
						"Host: appmsg.gadu-gadu.pl\r\n\r\n", ggNumber);

	int iResult = con->sendTo(sendBuffer, sizeof(sendBuffer));
	if(iResult == SOCKET_ERROR)
	{
		delete con;
		return false;
	}

	char recvBuffer[8192];
	iResult = con->recvAll(recvBuffer, sizeof(recvBuffer));
	if(iResult == SOCKET_ERROR)
	{
		delete con;
		return false;
	}

	boost_regex::regex_type findIP("(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
	boost_regex next(recvBuffer, recvBuffer + strlen(recvBuffer), findIP, 0);
	boost_regex end;

	if(next != end)
	{
		memcpy(trueLoginServer, next->str().c_str(), next->str().size()+1);
		delete con;
		addConsoleMsg(">> Pobrano prawdziwy serwer logowania: %s", trueLoginServer);
		return true;
	}

	delete con;
	return false;
}

bool Protocol::connectToTrueServer()
{
	if(!getTrueLoginServer((uint32_t)config.getNumber(Configuration::LOGIN_NUMBER)))
		return false;

	trueCon = new Connection();
	if(getTrueLoginServer())
	{
		if(!trueCon->connectTo(getTrueLoginServer(), 8074))
		{
			addConsoleMsg(">> Nie mo¿na po³¹czyæ siê z serwerem logowania!");
			delete trueCon;
			return false;
		}

		if(trueCon->recvFrom((char*)&welcome, sizeof(gg_welcome)) == SOCKET_ERROR)
		{
			delete trueCon;
			addConsoleMsg(">> Nie mo¿na pobraæ ziarna potrzebnego do obliczenia skrótu has³a!");
			return false;
		}
	}
	connected = true;
	return true;
}

bool Protocol::login(uint32_t ggNumber, const char* ggPassword, Status_t status, const char* loginDescription/* = NULL*/)
{
	if(!connectToTrueServer())
		return false;

	gg_login log;
	log.header.type = GG_LOGIN;
	log.header.length = sizeof(gg_login) - sizeof(gg_header);
	log.uin = ggNumber;
	strcpy(log.language, "pl");
	log.hash_type = GG_LOGIN_HASH_SHA1;
	sprintf(log.hash, "%s\0", gg_sha_hash(ggPassword, welcome.seed));
	log.status = status;
	log.flags = 0x00;
	log.features = GG_F_RECV_MSG | GG_F_NEW_STATUS | GG_F_NEW_STATUSES | GG_F_NEW_DISCONNECT | GG_F_UNKNOWN | GG_F_MSG_ACK;
	if(config.getBoolean(Configuration::USE_MULTILOGON))
		log.features |= GG_F_MULTI_LOGON;
	log.local_ip = 0x00;
	log.local_port = 0x00;
	log.external_ip = 0x00;
	log.external_port = 0x00;
	log.image_size = 0xFF;
	log.unknown1 = 0x64;
	log.versionLen = strlen(GG_VERSION);
	strcpy(log.version, GG_VERSION);

	if(loginDescription)
	{
		strncpy(log.description, loginDescription, GG_STATUS_DESCR_MAXSIZE);
		log.descriptionSize = strlen(log.description);
	}
	else
		log.descriptionSize = 0;

	if(trueCon->sendTo((char*)&log, sizeof(log)) != SOCKET_ERROR)
	{
		gg_login_reply loginStatus;
		if(trueCon->recvFrom((char*)&loginStatus, sizeof(loginStatus)) == SOCKET_ERROR)
		{
			addConsoleMsg(">> Nie mo¿na odebraæ statusu logowania!");
			delete trueCon;
			return false;
		}
		else
		{
			if(loginStatus.header.type == GG_LOGIN_OK)
			{
				gg_header emptyList;
				emptyList.type = GG_LIST_EMPTY;
				emptyList.length = 0;
				if(trueCon->sendTo((char*)&emptyList, sizeof(emptyList)) != SOCKET_ERROR)
				{
					loggedIn = true;
					return true;
				}
			}
			else
			{
				addConsoleMsg(">> Nie zosta³eœ zalogowany do serwera! Status logowania: 0x00%X", loginStatus);
				return false;
			}
		}
	}
	return false;
}

bool Protocol::changeStatus(Status_t newStatus, const char* statusDescription /* = NULL*/)
{
	if(!isLoggedIn())
		return false;

	gg_new_status stat;
	char* utf8;
	Ansi2Utf8(statusDescription, utf8);

	if(statusDescription)
	{
		strncpy(stat.description, utf8, GG_STATUS_DESCR_MAXSIZE);
		stat.description_size = strlen(stat.description) + 1;
	}
	else
		stat.description_size = 0;

	currentStatus = newStatus;

	switch(newStatus)
	{
		case GG_STATUS_NOT_AVAILABLE:
		case GG_STATUS_NOT_AVAILABLE_DESCR:
			loggedIn = connected = false;
			addConsoleMsg(">> Zosta³eœ roz³¹czony!");
			delete trueCon;
			SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_NOT_AVAILABLE]);
			break;

		case GG_STATUS_FFC:
		case GG_STATUS_FFC_DESCR:
			SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_FFC]);
			break;

		case GG_STATUS_DND:
		case GG_STATUS_DND_DESCR:
			SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_DND]);
			break;

		case GG_STATUS_AVAILABLE:
		case GG_STATUS_AVAILABLE_DESCR:
			SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_AVAILABLE]);
			break;

		case GG_STATUS_BUSY:
		case GG_STATUS_BUSY_DESCR:
			SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_BUSY]);
			break;

		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE_DESCR:
			SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_INVISIBLE]);
			break;

		case GG_STATUS_BLOCKED:
		case GG_STATUS_IMAGE_MASK:
		case GG_STATUS_DESCR_MASK:
		case GG_STATUS_FRIENDS_MASK:
			break;
	}

	stat.header.type = GG_NEW_STATUS;
	stat.header.length = sizeof(gg_new_status) - sizeof(gg_header) + stat.description_size;
	stat.status = newStatus;
	stat.flags = 0x00000001;
	if(config.getBoolean(Configuration::ACCEPT_LINKS))
		stat.flags |= 0x00800000;

	if(trueCon->sendTo((char*)&stat, sizeof(stat) + stat.description_size) == SOCKET_ERROR)
		return false;

	return true;
}

bool Protocol::sendMessage(uint32_t ggNumber, const char* Msg)
{
	if(!isLoggedIn())
		return false;

	if(ggNumber <= 0)
	{
		addConsoleMsg(">> Numer odbiory wiadomoœci jest niepoprawny!");
		return false;
	}
	else if(!Msg)
	{
		addConsoleMsg(">> Wiadomoœæ jest pusta!");
		return false;
	}
	else if(strlen(Msg) > GG_MSG_MAXSIZE)
	{
		addConsoleMsg(">> Wiadomoœæ jest zbyt d³uga! Maksymalna d³ugoœæ: %i", GG_MSG_MAXSIZE);
		return false;
	}
	char* utf8;
	Ansi2Utf8(Msg, utf8);
	const uint32_t msgLen = strlen(utf8) + 1;

	gg_header head;
	gg_send_msg sendMsg;

	head.type = GG_SEND_MSG;
	head.length = sizeof(gg_send_msg) + msgLen;

	sendMsg.recipient = ggNumber;
	sendMsg.msgClass = 0x0008;
	sendMsg.offsetPlain = sizeof(gg_send_msg) + msgLen;
	sendMsg.offsetAttributes = msgLen;
	if(trueCon->sendTo((char*)&head, sizeof(gg_header)) != SOCKET_ERROR)
	{
		if(trueCon->sendTo((char*)&sendMsg, sizeof(gg_send_msg)) != SOCKET_ERROR)
		{
			if(trueCon->sendTo(utf8, msgLen) == SOCKET_ERROR)
			{
				addConsoleMsg(">> Nie mo¿na wys³aæ wiadomoœci!");
				return false;
			}
			else
			{
				strcpy(lastSendMsg, utf8);
				return true;
			}
		}
	}
	return false;
}

bool Protocol::sendPing()
{
	if(!isLoggedIn())
		return false;

	gg_header ping = {GG_PING, 0};
	if(trueCon->sendTo((char*)&ping, sizeof(ping)) == SOCKET_ERROR)
		return false;
	return true;
}

void Protocol::receivePackets()
{
	if(!isConnected())
		return;

	gg_header recvPacket;
	if(trueCon->recvFrom((char*)&recvPacket, sizeof(gg_header)) != SOCKET_ERROR)
	{
		lastPacket = recvPacket;
		switch(recvPacket.type)
		{
			case GG_RECV_MSG:
			{
				if(isLoggedIn())
				{
					gg_recv_msg recvMsg;
					if(trueCon->recvAll((char*)&recvMsg, recvPacket.length) != SOCKET_ERROR)
					{
						msgCount++;
						ls.onReceiveMessage(recvMsg.sender, recvMsg.htmlMsg, recvMsg.time);
						memcpy(lastRecvMsg, recvMsg.htmlMsg, strlen(recvMsg.htmlMsg) + 1);
						gg_recv_msg_ack recvMsgAck;
						recvMsgAck.header.type = GG_RECV_MSG_ACK;
						recvMsgAck.header.length = sizeof(gg_recv_msg_ack) - sizeof(gg_header);
						recvMsgAck.num = msgCount;
						trueCon->sendTo((char*)&recvMsgAck, sizeof(recvMsgAck));
					}
				}
				break;
			}

			case GG_RECV_OWN_MSG:
			{
				if(config.getBoolean(Configuration::USE_MULTILOGON))
				{
					gg_recv_msg recvMsg;
					if(trueCon->recvAll((char*)&recvMsg, recvPacket.length) != SOCKET_ERROR)
						ls.onSendMessageFromOtherClient(recvMsg.sender, recvMsg.htmlMsg, recvMsg.time);
				}
				break;
			}

			case GG_MULTILOGON_INFO:
			{
				if(config.getBoolean(Configuration::USE_MULTILOGON))
				{
					gg_multilogon_info logonInfo;
					if(trueCon->recvAll((char*)&logonInfo, recvPacket.length) != SOCKET_ERROR)
						ls.onMultiLogon(logonInfo.items.loginIp, logonInfo.items.logonTime, logonInfo.count, logonInfo.items.clientName);
				}
				break;
			}

			case GG_SEND_MSG_ACK:
			{
				gg_send_msg_ack sendMsgAck;
				if(trueCon->recvFrom((char*)&sendMsgAck, recvPacket.length) != SOCKET_ERROR)
				{
					if(sendMsgAck.status == GG_ACK_DELIVERED || sendMsgAck.status == GG_ACK_QUEUED)
						ls.onSendMessage(sendMsgAck.recipient, lastSendMsg);
					else if(sendMsgAck.status == GG_ACK_MBOXFULL)
						addConsoleMsg(">> Wiadomoœæ nie zosta³a wys³anana. Skrzynka odbiorcza jest pe³na!");
					else if(sendMsgAck.status == GG_ACK_BLOCKED)
						addConsoleMsg(">> Wiadomoœæ nie zosta³a wys³ana, i¿ serwer j¹ zablokowa³!");
					else if(sendMsgAck.status == GG_ACK_NOT_DELIVERED)
						addConsoleMsg(">> Wiadomoœæ nie zosta³a wys³ana!");
				}
				break;
			}

			case GG_DISCONNECTING:
			case GG_DISCONNECTING_ACK:
			{
				addConsoleMsg(">> Zosta³eœ roz³¹czony przez serwer!");
				loggedIn = connected = false;
				delete trueCon;
				if(config.getBoolean(Configuration::AUTO_RECONNECT))
				{
					if(loginTries <= config.getNumber(Configuration::AUTO_RECONNECT_TRIES))
						CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)connectToServer, mainDlg, 0, 0);
				}
				else
				{
					if(gui.messageBox(MESSAGE_TYPE_YES_NO, "Zosta³eœ roz³¹czony, czy chcesz siê po³¹czyæ na nowo?"))
						CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)connectToServer, mainDlg, 0, 0);
				}
				break;
			}
/*
			case GG_USERLIST_REPLY:
			{
				gg_userlist_reply listReply;
				int recvSize = 0;
				std::string userList;
				addConsoleMsg(">> UserList packet length: %i", recvPacket.length);
				do
				{
					recvSize = trueCon->recvFrom((char*)&listReply, sizeof(listReply));
					userList += listReply.request;
				}while(recvSize > 0);
				break;
			}
*/
			case GG_USERLIST_REPLY:
			{
				gg_userlist listReply;
				addConsoleMsg(">> UserList packet length: %i", recvPacket.length);
				if(trueCon->recvAll((char*)&listReply, recvPacket.length) != SOCKET_ERROR)
				{
					/* TODO (#1#): Decompress received 'listReply.request' using 'Deflate' algorithm */
				}
				break;
			}

			case GG_NOTIFY_REPLY80:
			{
				gg_notify_reply80 notify;
				if(trueCon->recvFrom((char*)&notify, recvPacket.length) != SOCKET_ERROR)
				{
					if(config.getNumber(Configuration::LOGIN_NUMBER) == notify.uin)
					{
						switch(notify.status)
						{
							case GG_STATUS_FFC:
								SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_FFC]);
								break;

							case GG_STATUS_FFC_DESCR:
								SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_FFC]);
								SetDlgItemText(statusDlg, ID_DLG_DESC, notify.description);
								break;

							case GG_STATUS_DND:
								SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_DND]);
								break;

							case GG_STATUS_DND_DESCR:
								SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_DND]);
								SetDlgItemText(statusDlg, ID_DLG_DESC, notify.description);
								break;

							case GG_STATUS_AVAILABLE:
								SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_AVAILABLE]);
								break;

							case GG_STATUS_AVAILABLE_DESCR:
								SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_AVAILABLE]);
								SetDlgItemText(statusDlg, ID_DLG_DESC, notify.description);
								break;

							case GG_STATUS_BUSY:
								SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_BUSY]);
								break;

							case GG_STATUS_BUSY_DESCR:
								SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_BUSY]);
								SetDlgItemText(statusDlg, ID_DLG_DESC, notify.description);
								break;

							case GG_STATUS_INVISIBLE:
								SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_INVISIBLE]);
								break;

							case GG_STATUS_INVISIBLE_DESCR:
								SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETICON, 0, (LPARAM)statusIcon[ID_ICON_INVISIBLE]);
								SetDlgItemText(statusDlg, ID_DLG_DESC, notify.description);
								break;
						}
					}
					else
					{
						/* TODO (#1#): Parse rest of user list */
					}
				}
				break;
			}


			default:
				//addConsoleMsg(">> Odebrano nieznany pakiet! (0x%X)", recvPacket.type);
				break;
		}
	}
}

bool Protocol::requestForUserList()
{
	if(!isLoggedIn())
		return false;

	gg_userlist listReq;

	listReq.header.type = GG_USERLIST_REQUEST;
	listReq.header.length = (sizeof(gg_userlist) - sizeof(gg_header));
	listReq.type = GG_USERLIST_GET;
	listReq.version = 0x00;
	listReq.formatType = GG_USERLIST_FORMAT_TYPE_GG100;
	listReq.unknown = 0x01;

	if(trueCon->sendTo((char*)&listReq, sizeof(listReq)) == SOCKET_ERROR)
		return false;
	return true;
}

bool Protocol::disconnectMultiLogon(int64_t connectionID)
{
	if(!isLoggedIn())
		return false;

	gg_multilogon_disconnect disc;
	disc.header.type = GG_OWN_DISCONNECT;
	disc.header.length = (sizeof(gg_multilogon_disconnect) - sizeof(gg_header));
	disc.connID = connectionID;

	if(trueCon->sendTo((char*)&disc, sizeof(disc)) == SOCKET_ERROR)
		return false;
	return true;
}
