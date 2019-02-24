#ifndef __LUASCRIPT_H__
#define __LUASCRIPT_H__

#include <string>
#include <vector>

#include "Tools.h"
#include "LuaGG.h"

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

typedef struct
{
	int id;
	char* scriptName;
	char* filePath;
	bool timerEnabled;
	int32_t timerTime;
}LuaScripter;

enum LUA_RET_CODE
{
	LUA_NO_ERROR = 0,
	LUA_ERROR = -1,
	LUA_TRUE = 1,
	LUA_FALSE = 0,
	LUA_NULL = 0
};

class LuaScript
{
	public:
		LuaScript();
		~LuaScript();

		void initState();

		void registerTimers(HWND hWnd);
		void unRegisterTimers(HWND hWnd);

		void reportError(const char* Msg);

		bool loadBuffer(const std::string& text);
		bool loadScript(const char* fileName);
		bool loadScriptsFromFile();
		bool saveScriptsFile();
		void loadScripts();
		void doExecuteScript(const char* scriptName);

		static std::string popString(lua_State *L);
		static uint32_t popNumber(lua_State *L);
		static void setField(lua_State *L, const char* index, double val);
		static void setField(lua_State *L, const char* index, const std::string& val);

		static void pushPacket(lua_State *L, const gg_header head);

		void onSendMessage(uint32_t ggNumber, const char* Msg);
		void onReceiveMessage(uint32_t ggNumber, const char* Msg, time_t Date);
		void onSendMessageFromOtherClient(uint32_t ggNumber, const char* Msg, time_t Date);
		void onMultiLogon(int32_t loginIp, time_t logonTime, int64_t connID, const char* clientName);

		std::vector<LuaScripter> getLuaScripts() {return luaScripts;}

	protected:
		void registerFunctions();

		static int32_t luaGetConfigValue(lua_State* L);
		static int32_t luaMessageBox(lua_State* L);
		static int32_t luaSendMessage(lua_State* L);
		static int32_t luaChangeStatus(lua_State * L);
		static int32_t luaAddConsoleMsg(lua_State* L);
		static int32_t luaSetStatusBar(lua_State* L);
		static int32_t luaIsLoggedIn(lua_State* L);
		static int32_t luaIsConnected(lua_State* L);
		static int32_t luaGetCurrentStatus(lua_State* L);
		static int32_t luaGetTrueLoginServer(lua_State* L);
		static int32_t luaGetExePath(lua_State* L);
		static int32_t luaGetLastSendMessage(lua_State* L);
		static int32_t luaGetLastRecvMessage(lua_State* L);
		static int32_t luaGetLastRecvPacket(lua_State* L);
		static int32_t luaDoScriptLib(lua_State* L);
		static int32_t luaLoadScriptLib(lua_State* L);
		static int32_t luaGetPageInfo(lua_State* L);
		static int32_t luaDisconnectMultiLogon(lua_State* L);
		static int32_t luaGetMyIP(lua_State* L);

		static const luaL_Reg luaBitReg[13];
		static int32_t luaBitNot(lua_State *L);
		static int32_t luaBitAnd(lua_State *L);
		static int32_t luaBitOr(lua_State *L);
		static int32_t luaBitXor(lua_State *L);
		static int32_t luaBitLeftShift(lua_State *L);
		static int32_t luaBitRightShift(lua_State *L);
		static int32_t luaBitUNot(lua_State *L);
		static int32_t luaBitUAnd(lua_State *L);
		static int32_t luaBitUOr(lua_State *L);
		static int32_t luaBitUXor(lua_State *L);
		static int32_t luaBitULeftShift(lua_State *L);
		static int32_t luaBitURightShift(lua_State *L);

		lua_State* m_luaState;

		std::vector<LuaScripter> luaScripts;
};

#endif
