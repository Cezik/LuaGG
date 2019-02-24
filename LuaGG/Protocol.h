#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <string>
#include <openssl/sha.h>

#include "LuaGG.h"

class Connection;

class Protocol
{
	public:
		Protocol();
		~Protocol();

		const char* getTrueLoginServer() {return trueLoginServer;}
		const char* getLastSendMsg() {return lastSendMsg;}
		const char* getLastRecvMsg() {return lastRecvMsg;}

		bool login(uint32_t ggNumber, const char* ggPassword, Status_t status, const char* loginDescription = NULL);
		bool changeStatus(Status_t newStatus, const char* statusDescription = NULL);
		bool sendMessage(uint32_t ggNumber, const char* Msg);
		bool sendPing();

		void receivePackets();

		bool isLoggedIn() const {return loggedIn;}
		bool isConnected() const {return connected;}

		Connection* getConnection() const{return trueCon;}
		SOCKET getRealSocket() const;

		bool requestForUserList();
		bool disconnectMultiLogon(int64_t connectionID);

		int32_t getCurrentStatus() const{return currentStatus;}

		uint16_t getLoginTries() const{return loginTries;}
		void setLoginTries(uint16_t _tries) {loginTries = _tries;}

		gg_header getLastPacket() const{return lastPacket;}

		int32_t getMsgCount() const{return msgCount;}
		void setMsgCount(int32_t _value){msgCount = _value;}

	protected:
		bool getTrueLoginServer(uint32_t ggNumber);
		const char* gg_sha_hash(const char* password, uint32_t seed);
		bool connectToTrueServer();

		char* trueLoginServer;

		gg_welcome welcome;
		gg_header lastPacket;
		Connection* trueCon;

		bool loggedIn;
		bool connected;

		char* lastSendMsg;
		char* lastRecvMsg;

		int32_t currentStatus;
		int32_t msgCount;
		uint16_t loginTries;

		friend class Connection;
};

#endif
