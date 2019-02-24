#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <Winsock2.h>
#include <string>

#include "LuaGG.h"

class Connection
{
	public:
		Connection();
		~Connection();

		bool connectTo(const char* ipAddress, uint16_t port);

		int32_t sendTo(const char* sendBuffer, int sendBufferLength);
		int32_t sendAll(const char* sendBuffer, int sendBufferLength);

		int32_t recvFrom(char* recvBuffer, int recvBufferLength);
		int32_t recvAll(char* recvBuffer, int recvBufferLength);

		int32_t getError();

		void disConnect();

		bool getIPAddress(char* ipAddr);

		SOCKET getSocket() const{return sock;}

	protected:
		SOCKET sock;
};

#endif
