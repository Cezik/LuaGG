#include "Configuration.h"

Configuration::Configuration()
{
	m_loaded = false;
	L = NULL;
}

Configuration::~Configuration()
{
	lua_close(L);
}

bool Configuration::loadFile(const char* filename)
{
	if(L)
		lua_close(L);

	L = lua_open();
	if(!L)
		return false;

	if(luaL_dofile(L, filename))
	{
		lua_close(L);
		L = NULL; 
		return false;
	}

	if(!m_loaded)
	{
		m_configString[CONFIG_FILE] = filename;
		m_config[LOGIN_NUMBER] = getGlobalNumber(L, "LoginNumber");
		m_configString[LOGIN_PASSWORD] = getGlobalString(L, "LoginPassword");
		m_configString[LOGIN_DESCRIPTION] = getGlobalString(L, "LoginDesc");
		m_config[LOGIN_STATUS] = getGlobalNumber(L, "LoginStatus");
		m_config[SEND_PING_TIME] = getGlobalNumber(L, "sendPingTime", 60000);
	}

	m_config[SHOW_TOOLTIPS] = getGlobalBoolean(L, "ShowToolTips", true);
	m_config[AUTO_RECONNECT] = getGlobalBoolean(L, "autoReConnect", false);
	m_config[ACCEPT_LINKS] = getGlobalBoolean(L, "acceptLinks", false);
	m_config[SAVE_LOGS] = getGlobalBoolean(L, "saveLogs", false);
	m_config[USE_MULTILOGON] = getGlobalBoolean(L, "useMultiLogOn", false);

	m_config[AUTO_RECONNECT_TRIES] = getGlobalNumber(L, "autoReConnectTries", 5);

	m_loaded = true;
	return true;
}

bool Configuration::reload()
{
	if(!m_loaded)
		return false;

	return loadFile(m_configString[CONFIG_FILE].c_str());
}

const std::string& Configuration::getString(uint32_t _what) const
{
	if(m_loaded && _what < LAST_STRING_CONFIG)
		return m_configString[_what];
	else
	{
		addConsoleMsg(">> OSTRZE¯ENIE: LUA getString() > %i", _what);
		return m_configString[DUMMY_STR];
	}
}

int64_t Configuration::getNumber(uint32_t _what) const
{
	if(m_loaded && _what < LAST_CONFIG)
		return m_config[_what];
	else
	{
		addConsoleMsg(">> OSTRZE¯ENIE: LUA getNumber() > %i", _what);
		return 0;
	}
}

int64_t Configuration::getGlobalNumber(lua_State* _L, const std::string& _identifier, int64_t _default)
{
	lua_getglobal(_L, _identifier.c_str());

	if(!lua_isnumber(_L, -1))
	{
		lua_pop(_L, 1);
		return _default;
	}

	int64_t val = (int64_t)lua_tonumber(_L, -1);
	lua_pop(_L, 1);

	return val;
}

std::string Configuration::getGlobalString(lua_State* _L, const std::string& _identifier, const std::string& _default)
{
	lua_getglobal(_L, _identifier.c_str());

	if(!lua_isstring(_L, -1))
	{
		lua_pop(_L, 1);
		return _default;
	}

	int len = (int)lua_strlen(_L, -1);
	std::string ret(lua_tostring(_L, -1), len);
	lua_pop(_L, 1);

	return ret;
}

bool Configuration::getGlobalBoolean(lua_State* _L, const std::string& _identifier, bool _default)
{
	lua_getglobal(_L, _identifier.c_str());

	if(lua_isnumber(_L, -1))
	{
		int val = (int)lua_tonumber(_L, -1);
		lua_pop(_L, 1);
		return val != 0;
	}
	else if(lua_isstring(_L, -1))
	{
		std::string val = lua_tostring(_L, -1);
		lua_pop(_L, 1);
		return val == "yes" || val == "tak" || val == "true" || atoi(val.c_str()) > 0;
	}
	else if(lua_isboolean(_L, -1))
	{
		bool v = lua_toboolean(_L, -1) != 0;
		lua_pop(_L, 1);
		return v;
	}

	return _default;
}

void Configuration::getConfigValue(const std::string& key, lua_State* toL)
{
	lua_getglobal(L, key.c_str());
	moveValue(L, toL);
}

void Configuration::moveValue(lua_State* from, lua_State* to)
{
	switch(lua_type(from, -1))
	{
		case LUA_TNIL:
			lua_pushnil(to);
			break;

		case LUA_TBOOLEAN:
			lua_pushboolean(to, lua_toboolean(from, -1));
			break;

		case LUA_TNUMBER:
			lua_pushnumber(to, lua_tonumber(from, -1));
			break;

		case LUA_TSTRING:
		{
			size_t len;
			const char* str = lua_tolstring(from, -1, &len);
			lua_pushlstring(to, str, len);
		}
			break;

		case LUA_TTABLE:
			lua_newtable(to);

			lua_pushnil(from);
			while(lua_next(from, -2))
			{
				moveValue(from, to);
				lua_pushvalue(from, -1);
				moveValue(from, to);
				lua_insert(to, -2);
				lua_settable(to, -3);
			}
		default:
			break;
	}
	lua_pop(from, 1);
}
