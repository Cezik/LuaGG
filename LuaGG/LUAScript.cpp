#include "LUAScript.h"
#include "WinGUI.h"
#include "Protocol.h"
#include "ScriptManager.h"
#include "Connection.h"
#include "Configuration.h"

extern ScriptManager sm;
extern WinGUI gui;
extern Protocol protocol;
extern Configuration config;

LuaScript::LuaScript()
{
	initState();
}

LuaScript::~LuaScript()
{
	lua_close(m_luaState);
}

void LuaScript::initState()
{
	m_luaState = luaL_newstate();
	if(!m_luaState)
		return;

#ifndef __USE_LUALIBRARIES__
	//Here you load only the "safe" libraries
	luaopen_base(m_luaState);
	luaopen_table(m_luaState);
	luaopen_os(m_luaState);
	luaopen_string(m_luaState);
	luaopen_math(m_luaState);
	// Unsafe, but immensely useful
	luaopen_debug(m_luaState);
#else
	//And here you load both "safe" and "unsafe" libraries
	luaL_openlibs(m_luaState);
#endif

	registerFunctions();

	if(fileExists("Globals.lua"))
		loadScript("Globals.lua");
	else
		addConsoleMsg(">> Nie mo¿na za³adowaæ pliku: \"Globals.lua\"!");
}

void LuaScript::registerTimers(HWND hWnd)
{
	for(std::vector<LuaScripter>::iterator it = luaScripts.begin(); it != luaScripts.end(); it++)
	{
		if(it->timerEnabled)
		{
			SetTimer(hWnd, ID_TIMER_BEGIN + it->id, it->timerTime, 0);
		}
	}
}

void LuaScript::unRegisterTimers(HWND hWnd)
{
	for(std::vector<LuaScripter>::iterator it = luaScripts.begin(); it != luaScripts.end(); it++)
	{
		if(it->timerEnabled)
		{
			KillTimer(hWnd, ID_TIMER_BEGIN + it->id);
		}
	}
}

void LuaScript::registerFunctions()
{
	//lua_register(L, "name", C_function);

	//getConfigValue(key)
	lua_register(m_luaState, "getConfigValue", LuaScript::luaGetConfigValue);

	//messageBox(type, message)
	lua_register(m_luaState, "messageBox", LuaScript::luaMessageBox);

	//sendMessage(ggNumber, message)
	lua_register(m_luaState, "sendMessage", LuaScript::luaSendMessage);

	//changeStatus(status, desc)
	lua_register(m_luaState, "changeStatus", LuaScript::luaChangeStatus);

	//addConsoleMsg(message)
	lua_register(m_luaState, "addConsoleMsg", LuaScript::luaAddConsoleMsg);

	//setStatusBar(message)
	lua_register(m_luaState, "setStatusBar", LuaScript::luaSetStatusBar);

	//isLoggedIn()
	lua_register(m_luaState, "isLoggedIn", LuaScript::luaIsLoggedIn);

	//isConnected()
	lua_register(m_luaState, "isConnected", LuaScript::luaIsConnected);

	//getCurrentStatus()
	lua_register(m_luaState, "getCurrentStatus", LuaScript::luaGetCurrentStatus);

	//getTrueLoginServer()
	lua_register(m_luaState, "getTrueLoginServer", LuaScript::luaGetTrueLoginServer);

	//getExePath()
	lua_register(m_luaState, "getExePath", LuaScript::luaGetExePath);

	//getLastSendMessage()
	lua_register(m_luaState, "getLastSendMessage", LuaScript::luaGetLastSendMessage);

	//getLastSendMessage()
	lua_register(m_luaState, "getLastRecvMessage", LuaScript::luaGetLastRecvMessage);

	//getLastRecvPacket()
	lua_register(m_luaState, "getLastRecvPacket", LuaScript::luaGetLastRecvPacket);

	//getMyIP()
	lua_register(m_luaState, "getMyIP", LuaScript::luaGetMyIP);

	//disconnectMultiLogon(connectionID)
	lua_register(m_luaState, "disconnectMultiLogon", LuaScript::luaDisconnectMultiLogon);

	//loadmodlib(lib)
	lua_register(m_luaState, "loadmodlib", LuaScript::luaLoadScriptLib);

	//domodlib(lib)
	lua_register(m_luaState, "domodlib", LuaScript::luaDoScriptLib);

	//getPageInfo(address, page_query, regex_pattern, <optional>includeTextInRegex = 1)
	lua_register(m_luaState, "getPageInfo", LuaScript::luaGetPageInfo);

	luaL_register(m_luaState, "bit", LuaScript::luaBitReg);
}

std::string LuaScript::popString(lua_State *L)
{
	lua_pop(L, 1);
	const char* str = "";
	if(lua_isstring(L, 0) != LUA_FALSE)
		str = lua_tostring(L, 0);
	return str;
}

uint32_t LuaScript::popNumber(lua_State *L)
{
	lua_pop(L, 1);
	return (uint32_t)lua_tonumber(L, 0);
}

void LuaScript::setField(lua_State *L, const char* index, double val)
{
	lua_pushstring(L, index);
	lua_pushnumber(L, val);
	lua_settable(L, -3);
}

void LuaScript::setField(lua_State *L, const char* index, const std::string& val)
{
	lua_pushstring(L, index);
	lua_pushstring(L, val.c_str());
	lua_settable(L, -3);
}

void LuaScript::reportError(const char* Msg)
{
	FILE* file = fopen(getFilePath("LUA Errors.log").c_str(), "a+");
	char date[21];
	formatDate(time(NULL), date);
	fprintf(file, "[%s] %s\n", date, Msg);
	addConsoleMsg(">> %s", Msg);
	fclose(file);
}

bool LuaScript::loadBuffer(const std::string& text)
{
	int ret = luaL_loadbuffer(m_luaState, text.c_str(), text.length(), "LuaScript::loadBuffer");
	if(ret)
	{
		reportError(popString(m_luaState).c_str());
		return false;
	}

	if(!lua_isfunction(m_luaState, -1))
		return false;

	ret = lua_pcall(m_luaState, 0, 0, 0);
	if(ret)
	{
		reportError(popString(m_luaState).c_str());
		return false;
	}
	return true;
}

bool LuaScript::loadScript(const char* fileName)
{
	int ret = luaL_loadfile(m_luaState, getFilePath(fileName).c_str());
	if(ret != 0)
	{
		reportError(popString(m_luaState).c_str());
		return false;
	}

	if(lua_isfunction(m_luaState, -1) == 0)
		return false;

	ret = lua_pcall(m_luaState, 0, 0, 0);
	if(ret != 0)
	{
		reportError(popString(m_luaState).c_str());
		return false;
	}
	return true;
}

bool LuaScript::loadScriptsFromFile()
{
	xmlDocPtr doc = xmlParseFile(getFilePath("Skrypty.xml").c_str());
	if(!doc)
	{
		addConsoleMsg(">> Nie mo¿na odczytaæ pliku: \"Skrypty.xml\"!");
		return false;
	}

	xmlNodePtr root, p;
	root = xmlDocGetRootElement(doc);
	if(xmlStrcmp(root->name,(const xmlChar*)"LuaGG") != 0)
	{
		xmlFreeDoc(doc);
		return false;
	}

	luaScripts.clear();
	p = root->children;
	int cID = 0;
	while(p)
	{
		std::string strVal;
		int intVal;
		LuaScripter ls;
		if(xmlStrcmp(p->name, (const xmlChar*)"Skrypt") == 0)
		{
			if(readXMLString(p, "NazwaSkryptu", strVal))
			{
				ls.scriptName = new char[strVal.size()+1];
				strcpy(ls.scriptName, strVal.c_str());
			}
			if(readXMLString(p, "NazwaPliku", strVal))
			{
				ls.filePath = new char[strVal.size()+1];
				strcpy(ls.filePath, strVal.c_str());
				ls.id = cID;
			}
			ls.timerEnabled = false;
			if(readXMLString(p, "CzasAktywowany", strVal) && booleanString(strVal))
				ls.timerEnabled = true;
			if(readXMLInteger(p, "Czas", intVal))
				ls.timerTime = intVal;
			luaScripts.push_back(ls);
			cID++;
		}
		p = p->next;
	}
	xmlFreeDoc(doc);
	loadScripts();

	return true;
}

bool LuaScript::saveScriptsFile()
{
	xmlNodePtr root, listNode;
	xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
	doc->children = xmlNewDocNode(doc, NULL, (const xmlChar*)"LuaGG", NULL);
	root = doc->children;

	for(std::vector<LuaScripter>::iterator it = luaScripts.begin(); it != luaScripts.end(); it++)
	{
		listNode = xmlNewNode(NULL,(const xmlChar*)"Skrypt");
		xmlSetProp(listNode, (const xmlChar*) "NazwaSkryptu", (const xmlChar*)it->scriptName);
		xmlSetProp(listNode, (const xmlChar*) "NazwaPliku", (const xmlChar*)it->filePath);
		xmlSetProp(listNode, (const xmlChar*) "CzasAktywowany", (const xmlChar*)it->timerEnabled);
		xmlSetProp(listNode, (const xmlChar*) "Czas", (const xmlChar*)it->timerTime);
		xmlAddChild(root, listNode);
	}

	if(xmlSaveFile(getFilePath("Skrypty.xml").c_str(), doc))
	{
		xmlFreeDoc(doc);
		return true;
	}
	else
	{
		xmlFreeDoc(doc);
		return false;
	}
}

void LuaScript::loadScripts()
{
	for(std::vector<LuaScripter>::iterator it = luaScripts.begin(); it != luaScripts.end(); it++)
	{
		loadScript(it->filePath);
	}
}

void LuaScript::doExecuteScript(const char* name)
{
	if(!name)
		return;

	for(std::vector<LuaScripter>::iterator it = luaScripts.begin(); it != luaScripts.end(); it++)
	{
		if(strcmp(it->scriptName, name))
			loadScript(it->filePath);
	}
}

void LuaScript::pushPacket(lua_State *L, const gg_header head)
{
	lua_newtable(L);
	setField(L, "type", head.type);
	setField(L, "length", head.length);
}

void LuaScript::onSendMessage(uint32_t ggNumber, const char* Msg)
{
	lua_getglobal(m_luaState, "onSendMsg");
	lua_pushnumber(m_luaState, ggNumber);
	lua_pushstring(m_luaState, Msg);
	lua_call(m_luaState, 2, 0);
}

void LuaScript::onReceiveMessage(uint32_t ggNumber, const char* Msg, time_t Date)
{
	lua_getglobal(m_luaState, "onRecvMsg");
	lua_pushnumber(m_luaState, ggNumber);
	lua_pushstring(m_luaState, Msg);
	lua_pushnumber(m_luaState, (int32_t)Date);
	lua_call(m_luaState, 3, 0);
}

void LuaScript::onSendMessageFromOtherClient(uint32_t ggNumber, const char* Msg, time_t Date)
{
	lua_getglobal(m_luaState, "onSendMsgFromOtherClient");
	lua_pushnumber(m_luaState, ggNumber);
	lua_pushstring(m_luaState, Msg);
	lua_pushnumber(m_luaState, (int32_t)Date);
	lua_call(m_luaState, 3, 0);
}

void LuaScript::onMultiLogon(int32_t loginIp, time_t logonTime, int64_t connID, const char* clientName)
{
	lua_getglobal(m_luaState, "onMultiLogon");
	lua_pushnumber(m_luaState, loginIp);
	lua_pushnumber(m_luaState, (int32_t)logonTime);
	lua_pushnumber(m_luaState, connID);
	lua_pushstring(m_luaState, clientName);
	lua_call(m_luaState, 4, 0);
}


int32_t LuaScript::luaGetConfigValue(lua_State *L)
{
	config.getConfigValue(popString(L), L);
	return 1;
}

int32_t LuaScript::luaMessageBox(lua_State* L)
{
	const int32_t type = luaL_checkinteger(L, 1);
	const char* text = luaL_checkstring(L, 2);
	bool iResult = gui.messageBox((MessageBoxType_t)type, text);
	lua_pushboolean(L, iResult);
	return 1;
}

int32_t LuaScript::luaSendMessage(lua_State* L)
{
	const int32_t ggNumber = luaL_checkinteger(L, 1);
	const char* text = luaL_checkstring(L, 2);
	if(protocol.sendMessage(ggNumber, text))
		lua_pushinteger(L, LUA_TRUE);
	else
		lua_pushinteger(L, LUA_FALSE);
	return 1;
}

int32_t LuaScript::luaChangeStatus(lua_State * L)
{
	const int32_t status = luaL_checkinteger(L, 1);
	const char* text = luaL_checkstring(L, 2);
	if(protocol.changeStatus((Status_t)status, text))
		lua_pushinteger(L, LUA_TRUE);
	else
		lua_pushinteger(L, LUA_FALSE);
	return 1;
}

int32_t LuaScript::luaAddConsoleMsg(lua_State* L)
{
	addConsoleMsg(luaL_checkstring(L, 1));
	return 1;
}

int32_t LuaScript::luaSetStatusBar(lua_State* L)
{
	setStatusBar(luaL_checkstring(L, 1));
	return 1;
}

int32_t LuaScript::luaIsLoggedIn(lua_State* L)
{
	lua_pushboolean(L, protocol.isLoggedIn());
	return 1;
}

int32_t LuaScript::luaIsConnected(lua_State* L)
{
	lua_pushboolean(L, protocol.isConnected());
	return 1;
}

int32_t LuaScript::luaGetCurrentStatus(lua_State* L)
{
	lua_pushinteger(L, protocol.getCurrentStatus());
	return 1;
}

int32_t LuaScript::luaGetTrueLoginServer(lua_State* L)
{
	lua_pushstring(L, protocol.getTrueLoginServer());
	return 1;
}

int32_t LuaScript::luaGetExePath(lua_State* L)
{
	lua_pushstring(L, getExeDir().c_str());
	return 1;
}

int32_t LuaScript::luaGetLastSendMessage(lua_State* L)
{
	lua_pushstring(L, protocol.getLastSendMsg());
	return 1;
}

int32_t LuaScript::luaGetLastRecvMessage(lua_State* L)
{
	lua_pushstring(L, protocol.getLastRecvMsg());
	return 1;
}

int32_t LuaScript::luaGetMyIP(lua_State* L)
{
	char ipAddr[256];
	if(protocol.getConnection()->getIPAddress(ipAddr))
		lua_pushstring(L, ipAddr);
	else
		lua_pushboolean(L, false);
	return 1;
}

int32_t LuaScript::luaGetLastRecvPacket(lua_State* L)
{
	pushPacket(L, protocol.getLastPacket());
	return 1;
}

int32_t LuaScript::luaLoadScriptLib(lua_State* L)
{
	std::string name = asLowerCaseString(popString(L));
	for(LibMap::iterator it = sm.getFirstLib(); it != sm.getLastLib(); ++it)
	{
		if(asLowerCaseString(it->first) != name)
			continue;

		luaL_loadstring(L, it->second.second.c_str());
		lua_pushvalue(L, -1);
		break;
	}

	return 1;
}

int32_t LuaScript::luaDoScriptLib(lua_State* L)
{
	std::string name = asLowerCaseString(popString(L));
	for(LibMap::iterator it = sm.getFirstLib(); it != sm.getLastLib(); ++it)
	{
		if(asLowerCaseString(it->first) != name)
			continue;

		bool ret = luaL_dostring(L, it->second.second.c_str());

		lua_pushboolean(L, !ret);
		break;
	}

	return 1;
}

int32_t LuaScript::luaGetPageInfo(lua_State* L)
{
	Connection* con = new Connection();
	const char* pageAddr = luaL_checkstring(L, 1);
	if(con->connectTo(pageAddr, 80))
	{
		const char* page_query = luaL_checkstring(L, 2);
		const char* regex_pattern = luaL_checkstring(L, 3);
		
		int32_t parameters = lua_gettop(L), includeTextInRegex = 1;
		if(parameters > 3)
			includeTextInRegex = luaL_checkinteger(L, 4);

		char sendBuffer[512];
		sprintf(sendBuffer, "GET /%s HTTP/1.1\r\n"
							"Connection: Keep-Alive\r\n"
							"Host: %s\r\n\r\n", page_query, pageAddr);

		if(con->sendTo(sendBuffer, sizeof(sendBuffer)) != SOCKET_ERROR)
		{
			char recvBuffer[8192];
			std::string fullRecvBuffer;
			int recvSize;
			do
			{
				ZeroMemory(recvBuffer, sizeof(recvBuffer));
				recvSize = con->recvFrom(recvBuffer, sizeof(recvBuffer));
				fullRecvBuffer += recvBuffer;
			}while(recvSize > 0);

			boost_regex::regex_type rxp(regex_pattern);
			boost_regex next(fullRecvBuffer.c_str(), fullRecvBuffer.c_str() + fullRecvBuffer.size(), rxp, includeTextInRegex);
			boost_regex end;

			if(next != end)
			{
				lua_pushstring(L, next->str().c_str());
			}
			else
				lua_pushvalue(L, -1);
		}
	}
	else
		lua_pushvalue(L, -1);
	con->disConnect();
	delete con;
	return 1;
}

int32_t LuaScript::luaDisconnectMultiLogon(lua_State* L)
{
	protocol.disconnectMultiLogon(luaL_checkinteger(L, 1));
	return 1;
}

const luaL_Reg LuaScript::luaBitReg[] =
{
	//{"cast", LuaScriptInterface::luaBitCast},
	{"bnot", LuaScript::luaBitNot},
	{"band", LuaScript::luaBitAnd},
	{"bor", LuaScript::luaBitOr},
	{"bxor", LuaScript::luaBitXor},
	{"lshift", LuaScript::luaBitLeftShift},
	{"rshift", LuaScript::luaBitRightShift},
	// Unsigned
	{"ubnot", LuaScript::luaBitUNot},
	{"uband", LuaScript::luaBitUAnd},
	{"ubor", LuaScript::luaBitUOr},
	{"ubxor", LuaScript::luaBitUXor},
	{"ulshift", LuaScript::luaBitULeftShift},
	{"urshift", LuaScript::luaBitURightShift},
	//{"arshift", LuaScript::luaBitArithmeticalRightShift},
	{NULL,NULL}
};

int LuaScript::luaBitNot(lua_State *L)
{
	int32_t number = (int32_t)popNumber(L);
	lua_pushnumber(L, ~number);
	return 1;
}

int LuaScript::luaBitUNot(lua_State *L)
{
	uint32_t number = (uint32_t)popNumber(L);
	lua_pushnumber(L, ~number);
	return 1;
}

#define MULTIOP(type, name, op) \
	int LuaScript::luaBit##name(lua_State *L) { \
		int n = lua_gettop(L); \
		type w = (type)popNumber(L); \
		for(int i = 2; i <= n; ++i){ \
			w op popNumber(L); \
		} \
		lua_pushnumber(L, w); \
		return 1; \
	}

MULTIOP(int32_t, And, &=)
MULTIOP(int32_t, Or, |=)
MULTIOP(int32_t, Xor, ^=)
MULTIOP(uint32_t, UAnd, &=)
MULTIOP(uint32_t, UOr, |=)
MULTIOP(uint32_t, UXor, ^=)

#define SHIFTOP(type, name, op) \
	int LuaScript::luaBit##name(lua_State *L) { \
		type n2 = (type)popNumber(L), n1 = (type)popNumber(L); \
		lua_pushnumber(L, (n1 op n2)); \
		return 1; \
	}

SHIFTOP(int32_t, LeftShift, <<)
SHIFTOP(int32_t, RightShift, >>)
SHIFTOP(uint32_t, ULeftShift, <<)
SHIFTOP(uint32_t, URightShift, >>)
