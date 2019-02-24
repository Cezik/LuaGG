#include <sstream>

#include "Tools.h"
#include "WinGUI.h"
#include "Configuration.h"

extern WinGUI gui;
extern Configuration config;

bool fileExists(const char* fileName)
{
	std::ifstream file(getFilePath(fileName).c_str());
	if(!file.fail())
		return true;
	return false;
}

bool utf8ToLatin1(char* intext, std::string& outtext)
{
	outtext = "";
	if(intext == NULL)
		return false;
	int inlen = strlen(intext);
	if(inlen == 0)
		return false;
	int outlen = inlen*2;
	uint8_t* outbuf = new uint8_t[outlen];
	int res = UTF8Toisolat1(outbuf, &outlen, (uint8_t*)intext, &inlen);
	if(res < 0)
	{
		delete[] outbuf;
		return false;
	}
	outbuf[outlen] = '\0';
	outtext = (char*)outbuf;
	delete[] outbuf;
	return true;
}

void trim_left(std::string& source, const std::string& t)
{
	source.erase(0, source.find_first_not_of(t));
}

bool replaceString(std::string& text, const std::string key, const std::string value)
{
	std::string::size_type start = text.find(key), pos = 0;
	for(; start != std::string::npos; start = text.find(key, pos))
	{
		text.replace(start, key.size(), value);
		pos += start + key.size();
	}

	return true;
}

bool readXMLString(xmlNodePtr node, const char* tag, std::string& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(nodeValue)
	{
		if(!utf8ToLatin1(nodeValue, value))
			value = nodeValue;
		xmlFree(nodeValue);
		return true;
	}

	return false;
}

bool readXMLContentString(xmlNodePtr node, std::string& value)
{
	char* nodeValue = (char*)xmlNodeGetContent(node);
	if(!nodeValue)
		return false;

	if(!utf8ToLatin1(nodeValue, value))
		value = nodeValue;

	xmlFree(nodeValue);
	return true;
}

bool parseXMLContentString(xmlNodePtr node, std::string& value)
{
	bool result = false;
	std::string compareValue;
	while(node)
	{
		if(xmlStrcmp(node->name, (const xmlChar*)"text") && node->type != XML_CDATA_SECTION_NODE)
		{
			node = node->next;
			continue;
		}

		if(!readXMLContentString(node, compareValue))
		{
			node = node->next;
			continue;
		}

		trim_left(compareValue, "\r");
		trim_left(compareValue, "\n");
		trim_left(compareValue, " ");
		if(compareValue.length() > value.length())
		{
			value = compareValue;
			if(!result)
				result = true;
		}

		node = node->next;
	}

	return result;
}

bool readXMLInteger(xmlNodePtr node, const char* tag, int& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(nodeValue)
	{
		value = atoi(nodeValue);
		xmlFree(nodeValue);
		return true;
	}

	return false;
}

void formatDate(time_t time, char* buffer)
{
	const tm* tms = localtime(&time);
	if(tms)
		sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d", tms->tm_mday, tms->tm_mon + 1, tms->tm_year + 1900, tms->tm_hour, tms->tm_min, tms->tm_sec);
	else
		sprintf(buffer, "UNIX Time: %d", (int32_t)time);
}

void addConsoleMsg(const char* Msg, ...)
{
	char buffer[2048];
	va_list list;
	va_start(list, Msg);
	_vsnprintf(buffer, sizeof(buffer), Msg, list);
	va_end(list);

	if(config.getBoolean(Configuration::SAVE_LOGS))
	{
		FILE* file = fopen("Log.log", "a+");
		char date[21];
		formatDate(time(NULL), date);
		fprintf(file, "[%s] %s\n", date, buffer);
		fclose(file);
	}

	gui.addLineToLabel(mainDlg, ID_DLG_CONSOLE, buffer);
}

void setStatusBar(const char* Msg, ...)
{
	char buffer[512];
	va_list list;
	va_start(list, Msg);
	_vsnprintf(buffer, sizeof(buffer), Msg, list);
	va_end(list);
	SendDlgItemMessage(mainDlg, ID_STATUS_BAR, SB_SETTEXT, 1, (LPARAM)buffer);
}

std::string getExeDir()
{
	char buffer[MAX_PATH];
	if(GetModuleFileName(NULL, buffer, sizeof(buffer)))
	{
		if(PathRemoveFileSpec(buffer))
		{
			PathAddBackslash(buffer);
			return buffer;
		}
	}
	return "";
}

std::string getFilePath(const char* fileName)
{
	char filePath[MAX_PATH];
	sprintf(filePath, "%s%s", getExeDir().c_str(), fileName);
	return filePath;
}

void toLowerCaseString(std::string& source)
{
	std::transform(source.begin(), source.end(), source.begin(), tolower);
}

std::string asLowerCaseString(const std::string& source)
{
	std::string s = source;
	toLowerCaseString(s);
	return s;
}

bool booleanString(std::string source)
{
	toLowerCaseString(source);
	return (source == "yes" || source == "true" || source == "tak" || atoi(source.c_str()) > 0);
}

std::string getLastXMLError()
{
	std::stringstream ss;
	xmlErrorPtr lastError = xmlGetLastError();
	if(lastError->line)
		ss << "Linia: " << lastError->line << ", ";

	ss << "Info: " << lastError->message << std::endl;
	return ss.str();
}

void Ansi2Utf8(const char* text, char*& result)
{
	const int SIZE = 10240;
	result = new char[SIZE];
	WCHAR w[SIZE];
	MultiByteToWideChar(CP_ACP, 0, text, -1, w, SIZE / sizeof(WCHAR));
	WideCharToMultiByte(CP_UTF8, 0, w, -1, result, SIZE, 0, 0);
}

std::string convertIPToString(uint32_t ip)
{
	char buffer[20];
	sprintf(buffer, "%d.%d.%d.%d", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24));
	return buffer;
}
