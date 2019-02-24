#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <string>

#include "Tools.h"
#include "LuaGG.h"

extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
}

class Configuration
{
	public:
		Configuration();
		~Configuration();

		enum config_t
		{
			LOGIN_NUMBER = 0,
			LOGIN_STATUS,
			SHOW_TOOLTIPS,
			AUTO_RECONNECT,
			AUTO_RECONNECT_TRIES,
			ACCEPT_LINKS,
			SAVE_LOGS,
			USE_MULTILOGON,
			SEND_PING_TIME,
			LAST_CONFIG
		};

		enum config_string_t
		{
			DUMMY_STR = 0,
			CONFIG_FILE,
			LOGIN_PASSWORD,
			LOGIN_DESCRIPTION,
			LAST_STRING_CONFIG
		};

		bool loadFile(const char* filename);
		bool reload();
		bool isLoaded() {return m_loaded;}

		void getConfigValue(const std::string& key, lua_State* _L);

		int64_t getNumber(uint32_t _what) const;
		const std::string& getString(uint32_t _what) const;
		bool getBoolean(uint32_t _what) const {return getNumber(_what) != 0;}

	private:
		lua_State* L;
		bool m_loaded;
		int64_t m_config[LAST_CONFIG];
		std::string m_configString[LAST_STRING_CONFIG];
		int64_t getGlobalNumber(lua_State* _L, const std::string& _identifier, int64_t _default = 0);
		std::string getGlobalString(lua_State* _L, const std::string& _identifier, const std::string& _default = "");
		bool getGlobalBoolean(lua_State* _L, const std::string& _identifier, bool _default = false);
		static void moveValue(lua_State* fromL, lua_State* toL);
};

#endif
