#ifndef __GADUGADU_H__
#define __GADUGADU_H__

#include "LuaGG.h"

#pragma pack(push, 1)

/* Packets what we send to the server - begin */
#define GG_LOGIN 0x0031
#define GG_NEW_STATUS 0x0038
#define GG_LIST_EMPTY 0x0012
#define GG_SEND_MSG 0x002d
#define GG_RECV_MSG_ACK 0x0046
#define GG_OWN_DISCONNECT 0x0062
#define GG_PING 0x0008
#define GG_USERLIST_REQUEST 0x0040
/* Packets what we send to the server - end */

/* Packets what we recv from the server - begin */
#define GG_WELCOME 0x0001
#define GG_SEND_MSG_ACK 0x0005
#define GG_LOGIN_OK 0x0035
#define GG_LOGIN_FAILED 0x0043
#define GG_DISCONNECTING 0x000b
#define GG_DISCONNECTING_ACK 0x000d
#define GG_RECV_MSG 0x002e
#define GG_RECV_OWN_MSG 0x005A
#define GG_MULTILOGON_INFO 0x005B
#define GG_USERLIST_REPLY 0x0041
#define GG_PONG 0x0007
//#define GG_NOTIFY_REPLY80 0x0037
#define GG_NOTIFY_REPLY80 0x0036
/* Packets what we recv from the server - end */

/* Constants - begin */
	/* Message has been send confirmation - begin */
	#define GG_ACK_BLOCKED 0x0001
	#define GG_ACK_DELIVERED 0x0002
	#define GG_ACK_QUEUED 0x0003
	#define GG_ACK_MBOXFULL 0x0004
	#define GG_ACK_NOT_DELIVERED 0x0006
	/* Message has been send confirmation - end */

	/* Password hash - begin */
	#define GG_LOGIN_HASH_GG32 0x01 // is not supported by LuaGG (and by server?)
	#define GG_LOGIN_HASH_SHA1 0x02
	/* Password hash - end */

	/* Login features - begin */
	#define GG_F_RECV_MSG 0x00000002
	#define GG_F_NEW_STATUS 0x00000004 // Client use changing status from protocol 8+
	#define GG_F_NEW_STATUSES 0x00000010 // Client support statuses 'GG_STATUS_FFC' and 'GG_STATUS_DND'
	#define GG_F_NEW_DISCONNECT 0x00000040 // Client recv 'non-empty' packet if logging in is failed
	#define GG_F_UNKNOWN 0x00000100 // This is needed by new protocol
	#define GG_F_MSG_ACK 0x00000400 // Client need to send 'GG_SEND_MSG_ACK' packet after sending message
	#define GG_F_MULTI_LOGON 0x00004000
	/* Login features - end */
	
	/* User list - begin */
	#define GG_USERLIST_FORMAT_TYPE_NONE 0x00 // brak treœci listy kontaktów
	#define GG_USERLIST_FORMAT_TYPE_GG70 0x01 // format listy kontaktów zgodny z Gadu-Gadu 7.0
	#define GG_USERLIST_FORMAT_TYPE_GG100 0x02 // format listy kontaktów zgodny z Gadu-Gadu 10.0
	/* User list - end */

#define GG_MSG_MAXSIZE 1989
#define GG_STATUS_DESCR_MAXSIZE 255
#define GG_VERSION "Gadu-Gadu Client build 10.0.0.10450"

enum Status_t
{
	GG_STATUS_NOT_AVAILABLE = 0x0001,
	GG_STATUS_NOT_AVAILABLE_DESCR = 0x0015,

	GG_STATUS_FFC = 0x0017,
	GG_STATUS_FFC_DESCR = 0x0018,

	GG_STATUS_AVAILABLE = 0x0002,
	GG_STATUS_AVAILABLE_DESCR = 0x0004,

	GG_STATUS_BUSY = 0x0003,
	GG_STATUS_BUSY_DESCR = 0x0005,
	
	GG_STATUS_DND = 0x0021,
	GG_STATUS_DND_DESCR = 0x0022,

	GG_STATUS_INVISIBLE = 0x0014,
	GG_STATUS_INVISIBLE_DESCR = 0x0016,
	
	GG_STATUS_BLOCKED = 0x0006,

	GG_STATUS_IMAGE_MASK = 0x0100,
	GG_STATUS_DESCR_MASK = 0x4000,
	GG_STATUS_FRIENDS_MASK = 0x8000
};

enum userList
{
	GG_USERLIST_PUT = 0x00, // export
	GG_USERLIST_GET = 0x02 // import
};

enum userListReply
{
	GG_USERLIST_PUT_REPLY = 0x00, /* pocz¹tek eksportu listy */
	GG_USERLIST_PUT_MORE_REPLY = 0x02, /* kontynuacja */
	GG_USERLIST_GET_MORE_REPLY = 0x04, /* pocz¹tek importu listy */
	GG_USERLIST_GET_REPLY = 0x06 /* ostatnia czêœæ importu */
};

/* Constants - end */

/* Structures - begin */
struct gg_header
{
	uint32_t type;
	uint32_t length;
};

struct gg_login_reply
{
	gg_header header;
	uint32_t unknown1;
};

struct gg_login
{
	gg_header header;
	uint32_t uin;
	char language[2];
	uint8_t hash_type;
	char hash[64];
	uint32_t status;
	uint32_t flags;
	uint32_t features;
	uint32_t local_ip;
	uint16_t local_port;
	uint32_t external_ip;
	uint16_t external_port;
	uint8_t image_size;
	uint8_t unknown1;
	uint32_t versionLen;
	char version[40];
	uint32_t descriptionSize;
	char description[GG_STATUS_DESCR_MAXSIZE];
};

struct gg_send_msg_ack
{
	uint32_t status;
	uint32_t recipient;
	uint32_t seq;
};

struct gg_notify
{
	gg_header header;
	uint32_t uin;
	char type;
};

struct gg_userlist
{
	gg_header header;
	uint8_t type;
	int version;
	uint8_t formatType;
	uint8_t unknown;
	char request[];
};

struct gg_welcome
{
	gg_header header;
	uint32_t seed;
};

struct gg_send_msg
{
	uint32_t recipient;
	uint32_t seq;
	uint32_t msgClass;
	uint32_t offsetPlain;
	uint32_t offsetAttributes;
};

struct gg_recv_msg
{
	uint32_t sender;
	uint32_t seq;
	uint32_t time;
	uint32_t msgClass;
	uint32_t offset_plain;
	uint32_t offset_attributes;
	char htmlMsg[GG_MSG_MAXSIZE];
	char plainMsg[GG_MSG_MAXSIZE];
	char attributes[];
};

struct gg_recv_msg_ack
{
	gg_header header;
	uint32_t num;
};

struct gg_new_status
{
	gg_header header;
	uint32_t status;
	uint32_t flags;
	uint32_t description_size;
	char description[GG_STATUS_DESCR_MAXSIZE];
};

struct gg_multilogon_item
{
	uint32_t loginIp;
	uint32_t flags;
	uint32_t features;
	uint32_t logonTime;
	int64_t connID;
	uint32_t unknown;
	uint32_t clientNameSize;
	char clientName[40];
};

struct gg_multilogon_info
{
	uint32_t count;
	gg_multilogon_item items;
};

struct gg_multilogon_disconnect
{
	gg_header header;
	int64_t connID;
};

struct gg_notify_reply80
{
	uint32_t uin;
	uint32_t status;
	uint32_t features;
	uint32_t remoteIP;
	uint16_t remotePort;
	uint8_t imageSize;
	uint8_t unknown1;
	uint32_t flags;
	uint32_t descriptionSize;
	char description[GG_STATUS_DESCR_MAXSIZE];
};

/* Structures - end */

#pragma pack(pop)

#endif
