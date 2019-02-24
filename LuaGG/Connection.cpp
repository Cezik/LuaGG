#include "Connection.h"
#include "Resources.h"

Connection::Connection()
{
	sock = 0;
}

Connection::~Connection()
{
	disConnect();
}

bool Connection::connectTo(const char* ipAddress, uint16_t port)
{
	WSADATA wsaData;
	SOCKADDR_IN SA;
	HOSTENT *HE;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(iResult != NO_ERROR)
		return false;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
	{
		WSACleanup();
		return false;
	}

	SA.sin_family = AF_INET;
	SA.sin_port = htons(port);
	uint32_t Address = inet_addr(ipAddress);

	if(Address == INADDR_NONE || Address == INADDR_ANY)
	{
		HE = gethostbyname(ipAddress);
		if(HE != NULL)
			Address = *((u_long*)HE->h_addr_list[0]);
	}
	SA.sin_addr.s_addr = Address;

	iResult = connect(sock, (SOCKADDR*)&SA, sizeof(SA));
	if(iResult == SOCKET_ERROR)
	{
		addConsoleMsg(">> Nie mo¿na po³¹czyæ siê z '%s:%i' !", ipAddress, port);
		return false;
	}
	return true;
}

int32_t Connection::sendTo(const char* sendBuffer, int sendBufferLength)
{
	return send(sock, sendBuffer, sendBufferLength, 0);
}

int32_t Connection::sendAll(const char* sendBuffer, int sendBufferLength)
{
	int sendBytes = 0;
	do
	{
		int iResult = sendTo(sendBuffer, std::min(sendBufferLength - sendBytes, 10000));
		if(iResult <= 0)
			break;
		sendBytes += iResult;
	}while(sendBytes < sendBufferLength);
	return sendBytes;
}

int32_t Connection::recvFrom(char* recvBuffer, int recvBufferLength)
{
	return recv(sock, recvBuffer, recvBufferLength, 0);
}

int32_t Connection::recvAll(char* recvBuffer, int recvBufferLength)
{
	std::string tempBuffer;
	int recvBytes = 0;
	do
	{
		int iResult = recvFrom(recvBuffer, std::min(recvBufferLength - recvBytes, 10000));
		if(iResult <= 0)
			break;
		recvBytes += iResult;
		tempBuffer += recvBuffer;
	}while(recvBytes < recvBufferLength);
	memcpy(recvBuffer, tempBuffer.c_str(), tempBuffer.size()+1);
	return tempBuffer.size();
}

int32_t Connection::getError()
{
	return WSAGetLastError();
}

void Connection::disConnect()
{
	closesocket(sock);
	WSACleanup();
}

bool Connection::getIPAddress(char* ipAddr)
{
	WSADATA wsaData;
	HOSTENT *he;
	char hostName[256];
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(iResult != NO_ERROR)
		return false;

	if(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) != INVALID_SOCKET)
	{
		if(gethostname(hostName, sizeof(hostName)) != SOCKET_ERROR)
		{
			he = gethostbyname(hostName);
			if(he != NULL)
			{
				strcpy(ipAddr, inet_ntoa(*(in_addr*)he->h_addr));
				return true;
			}
		}
	}
	WSACleanup();
	return false;
}
