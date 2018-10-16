#include "imgui/imgui.h"
#include <stdlib.h> 
#include <ctype.h>
#include <stdio.h>
#include <string>
#include <string>
#include <vector>

class ModuleClient;

#ifndef _BASIC_CONSOLE_H_
#define _BASIC_CONSOLE_H_

// Demonstrating creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.

class Console
{
public:

	Console(const ModuleClient* target);
	~Console();

public:

	std::string				static_buffer;
	char					InputBuf[256];
	ImVector<char*>			Items;
	bool					ScrollToBottom;
	unsigned int			callback_flag = 0;
	ModuleClient*			target = nullptr;

public:

	// Portable helpers
	static int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
	static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
	static char* Strdup(const char *str) { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }

	void					ClearLog();
	void					AddLog(const char* fmt, ...);
	void					_Draw(const char* title, bool* p_open);
	void					ExecCommand(const char* command_line);
};
#endif // !_BASIC_CONSOLE_H_