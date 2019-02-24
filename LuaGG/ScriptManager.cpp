#include "ScriptManager.h"

#include <boost/filesystem.hpp>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

extern WinGUI gui;
extern LuaScript ls;

ScriptManager::ScriptManager()
{
	scriptsLoaded = false;
}

ScriptManager::~ScriptManager()
{
	//
}

bool ScriptManager::loadScripts()
{
	char scriptPath[MAX_PATH];
	sprintf(scriptPath, "%sSkrypty", getExeDir().c_str());
	boost::filesystem::path modsPath(scriptPath);
	if(!boost::filesystem::exists(modsPath))
	{
		gui.messageBox(MESSAGE_TYPE_FATAL_ERROR, "Nie mo¿na zlokalizowaæ folderu \"Skrypty\"\nFolder ten prawdopodobnie nie istnieje!");
		return false;
	}

	int32_t i = 0, j = 0;
	bool enabled = false;

	for(boost::filesystem::directory_iterator it(modsPath), end; it != end; ++it)
	{
		std::string s = it->leaf();
		if(boost::filesystem::is_directory(it->status()) && (s.size() > 4 ? s.substr(s.size() - 4) : "") != ".xml")
			continue;

		std::string log = ">> £adowanie '";
		log += s;
		if(loadFromXML(s, enabled))
		{
			log += "' zakoñczone z powodzeniem";
			if(!enabled)
			{
				++j;
				log += ", ale skrypt jest wy³¹czony";
			}
			log += ".";
		}
		else
			log += "' zakoñczone z niepowodzeniem!";
		addConsoleMsg("%s", log.c_str());
		++i;
	}
	std::string tag, tag2;
	if(i == 0)
		tag = "skryptów";
	else if(i == 1)
		tag = "skrypt";
	else if(i <= 4)
		tag = "skrypty";
	else if(i > 4)
		tag = "skryptów";

	if(j == 0)
		tag2 = "wy³¹czonych";
	else if(j == 1)
		tag2 = "wy³¹czony";
	else if(j <= 4)
		tag2 = "wy³¹czone";
	else if(j > 4)
		tag2 = "wy³¹czonych";

	addConsoleMsg(">> Za³adowano %i %s, w tym %i %s.", i, tag.c_str(), j, tag2.c_str());
	scriptsLoaded = true;
	return true;
}

bool ScriptManager::loadFromXML(const std::string& fileName, bool& enabled)
{
	enabled = false;
	char scriptPath[MAX_PATH];
	sprintf(scriptPath, "%sSkrypty\\%s", getExeDir().c_str(), fileName.c_str());

	xmlDocPtr doc = xmlParseFile(scriptPath);
	if(!doc)
	{
		addConsoleMsg(">> Nie mo¿na za³adowaæ skryptu: %s", fileName.c_str());
		addConsoleMsg(">> %s", getLastXMLError().c_str());
		return false;
	}

	std::string strVal;

	xmlNodePtr p, root = xmlDocGetRootElement(doc);
	if(xmlStrcmp(root->name,(const xmlChar*)"LuaGG"))
	{
		addConsoleMsg(">> Skrypt '%s' zaczyna siê nieodpowiednim tagiem!", fileName.c_str());
		xmlFreeDoc(doc);
		return false;
	}

	ScriptBlock sBlock;
	if(!readXMLString(root, "Nazwa", strVal))
	{
		addConsoleMsg(">> Skrypt '%s' nie jest podpisany!", fileName.c_str());
		xmlFreeDoc(doc);
		return false;
	}
	else
		sBlock.name = strVal;

	sBlock.enabled = false;
	if(readXMLString(root, "Aktywowany", strVal) && booleanString(strVal))
		sBlock.enabled = true;

	sBlock.file = fileName;
	if(readXMLString(root, "Autor", strVal))
		sBlock.author = strVal;
	if(readXMLString(root, "Wersja", strVal))
		sBlock.version = strVal;
	if(readXMLString(root, "Kontakt", strVal))
		sBlock.contact = strVal;

	if(sBlock.enabled)
	{
		p = root->children;
		while(p)
		{
			if(!xmlStrcmp(p->name, (const xmlChar*)"Info"))
			{
				if(parseXMLContentString(p->children, strVal))
				{
					replaceString(strVal, "\t", "");
					sBlock.description = strVal;
				}
			}
			if(!xmlStrcmp(p->name, (const xmlChar*)"Konfiguracja"))
			{
				if(!readXMLString(p, "Nazwa", strVal))
				{
					p = p->next;
					continue;
				}

				toLowerCaseString(strVal);
				std::string strLib;
				if(parseXMLContentString(p->children, strLib))
				{
					LibMap::iterator it = libMap.find(strVal);
					if(it == libMap.end())
					{
						LibBlock lb;
						lb.first = fileName;
						lb.second = strLib;

						libMap[strVal] = lb;
					}
				}
			}
			if(!xmlStrcmp(p->name, (const xmlChar*)"Skrypt"))
			{
				if(parseXMLContentString(p->children, strVal))
				{
					if(ls.loadBuffer(strVal))
						enabled = sBlock.enabled;
				}
			}

			p = p->next;
		}
	}

	scriptMap[sBlock.name] = sBlock;

	xmlFreeDoc(doc);
	return true;
}

void ScriptManager::clearMods()
{
	scriptMap.clear();
	libMap.clear();
}

bool ScriptManager::reloadMods()
{
	clearMods();
	return loadScripts();
}
