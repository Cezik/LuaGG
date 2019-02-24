#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <boost/regex.hpp>
#include <libxml/parser.h>

#include <fstream>
#include <Shlwapi.h>
#include <zlib.h>
#include "LuaGG.h"

#define CHUNK 16384

typedef boost::regex_token_iterator<const char*> boost_regex;

bool fileExists(const char* fileName);

bool utf8ToLatin1(char* intext, std::string& outtext);
void trim_left(std::string& source, const std::string& t);
bool replaceString(std::string& text, const std::string key, const std::string value);

bool readXMLString(xmlNodePtr node, const char* tag, std::string& value);
bool readXMLContentString(xmlNodePtr node, std::string& value);
bool parseXMLContentString(xmlNodePtr node, std::string& value);
bool readXMLInteger(xmlNodePtr node, const char* tag, int& value);

void formatDate(time_t time, char* buffer);

void addConsoleMsg(const char* Msg, ...);
void setStatusBar(const char* Msg, ...);

std::string getExeDir();
std::string getFilePath(const char* fileName);

void toLowerCaseString(std::string& source);
std::string asLowerCaseString(const std::string& source);
bool booleanString(std::string source);
std::string getLastXMLError();

void Ansi2Utf8(const char* text, char*& result);
std::string convertIPToString(uint32_t ip);

#endif
