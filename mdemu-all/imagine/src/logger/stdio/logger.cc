/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#define thisModuleName "logger:stdio"
#include <engine-globals.h>
#include <logger/interface.h>

#include <stdarg.h>
#include <stdio.h>
#include "platform_util.h"
#include "base/interface.h"

#ifdef CONFIG_BASE_ANDROID
	#include <android/log.h>
#endif
#ifdef USE_LOGGER
uint loggerVerbosity = loggerMaxVerbosity;

static const bool useExternalLogFile = 0;
static FILE *logExternalFile = 0;

static const char *externalLogPath()
{
	static char path[256] = "";
	if(!strlen(path))
	{
		sprintf(path, "%s/imagine.log", Base::documentsPath());
	}
	return path;
}

extern "C" CallResult logger_init()
{
	if(useExternalLogFile)
	{
		logMsg("external log file: %s", externalLogPath());
		logExternalFile = open_file(externalLogPath(), "wb");
		if(!logExternalFile)
		{
			return IO_ERROR;
		}
	}

	//logMsg("init logger");
	return OK;
}

void logger_vprintfn(LoggerSeverity severity, const char* msg, va_list args)
{
	if(severity > loggerVerbosity) return;

	if(logExternalFile)
	{
		va_list args2;
		va_copy(args2, args);
		vfprintf(logExternalFile, msg, args2);
		fflush(logExternalFile);
	}

	#ifdef CONFIG_BASE_ANDROID
		__android_log_vprint(ANDROID_LOG_INFO, "imagine", msg, args);
	#else
		vfprintf(stderr, msg, args);
	#endif
}

void logger_printfn(LoggerSeverity severity, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	logger_vprintfn(severity, msg, args);
	va_end(args);
}
#endif

#undef thisModuleName
