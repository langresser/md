#pragma once
#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32
#include "iosUtil.h"
#else
#define snprintf _snprintf
#endif

extern char g_resource_dir[256];

void initDir();
FILE* open_file(const char* file, const char* mode);

void openWebsite(const char* url);
unsigned int TimeGet();

#if 0
class TimeLogger
{
public:
	TimeLogger(const char* name)
	{
		m_name = name;
		m_startTime = TimeGet();
	}
	~TimeLogger()
	{
		unsigned int costTime = TimeGet() - m_startTime;
		printf("%s  cost   %dms\n", m_name.c_str(), costTime);
	}
private:
	unsigned int m_startTime;
	std::string m_name;
};
#endif



//#define ENABLE_PROFILE

#ifdef ENABLE_PROFILE
#define LOGTIME(r) TimeLogger timeLogger(r)
#define LOGTIME2(r, x) TimeLogger timeLogger#x(r)
#else
#define LOGTIME(r)
#define LOGTIME2(r, x)
#endif