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

#define thisModuleName "fs"
#include <logger/interface.h>
#include <util/strings.h>
#include <base/interface.h>

#include "sys.hh"

#if defined (CONFIG_BASE_WIN32)
	#include <direct.h>
#else
	#include <sys/stat.h>
	#include <unistd.h>
#endif

bool Fs::fileExists(const char *path)
{
	return FsSys::fileType(path) != TYPE_NONE;
}

CallResult Fs::changeToAppDir(const char *launchCmd)
{
	char path[strlen(launchCmd)+1];
	logMsg("app called with cmd %s", launchCmd);
	strcpy(path, launchCmd);
	dirNameInPlace(path);
	if(FsSys::chdir(path) != 0)
	{
		logErr("error changing working directory to %s", path);
		return INVALID_PARAMETER;
	}
	//logMsg("changed working directory to %s", path);
	if(!Base::appPath)
	{
		Base::appPath = string_dup(FsSys::workDir());
		logMsg("set app dir to %s", Base::appPath);
	}
	return OK;
}

#undef thisModuleName
