#ifndef __SCRIPTMANAGER_H__
#define __SCRIPTMANAGER_H__

#include <string>
#include <map>

#include "LuaGG.h"

struct ScriptBlock
{
	std::string name, description, author, version, contact, file;
	bool enabled;
};
typedef std::map<std::string, ScriptBlock> ScriptMap;

struct LibBlock
{
	std::string first, second;
};
typedef std::map<std::string, LibBlock> LibMap;

class ScriptManager
{
	public:
		ScriptManager();
		~ScriptManager();

		bool loadScripts();
		bool loadFromXML(const std::string& fileName, bool& enabled);

		void clearMods();
		bool reloadMods();

		inline LibMap::iterator getFirstLib() {return libMap.begin();}
		inline LibMap::iterator getLastLib() {return libMap.end();}

	protected:
		bool scriptsLoaded;

		ScriptMap scriptMap;
		LibMap libMap;
};

#endif
