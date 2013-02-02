#include "platform_util.h"
#include <locale>
#include <string.h>
#include <string>

#ifdef __IPHONEOS__
char g_application_dir[256] = {0};
char g_resource_dir[256] = "../Documents/";
#elif defined __ANDROID__
char g_application_dir[256] = {0};
char g_resource_dir[256] = "/sdcard/sdlpal/";
#else
char g_application_dir[256] = {0};
char g_resource_dir[256] = {0};
#endif

char*   my_strlwr(   char*   str   )
{
    char*   orig   =   str;
    //   process   the   string
    for   (   ;   *str   != '\0';   str++   )
        *str   =   tolower(*str);
    return   orig;
}

std::string getFullPath(const char* fileName)
{
    std::string temp = g_resource_dir;
    temp += fileName;
    return temp;
}

FILE* open_file(const char* file_name, const char* read_mode)
{
    char szFileName[256] = {0};
	char szTemp[256] = {0};
	FILE* fp = NULL;

	strncpy(szFileName, file_name, sizeof(szFileName) - 1);
    my_strlwr(szFileName);

	// 先查找资源目录，资源目录要求是可以读写的。如果有相同文件，优先读取资源目录下的。（更新文件）
	snprintf(szTemp, sizeof(szTemp) - 1, "%s%s", g_resource_dir, szFileName);
//    printf("%s\n", szTemp);
	
	fp = fopen(szTemp, read_mode);

	if (fp) {
		return fp;
	}
    
#ifdef __IPHONEOS__
    // 写文件只能写在document目录
    if (strchr(read_mode, 'w') != 0) {
        fp = fopen(szTemp, read_mode);
        if (fp) {
            return fp;
        }

		// 从外部拷贝进来的存档可能因为文件所属不是mobile导致无法写入。删除旧文件，重新创建新文件
        remove(szTemp);
		fp = fopen(szTemp, read_mode);
        if (fp) {
            return fp;
        }

        return NULL;
    }
#endif

#ifdef __ANDROID__
    return NULL;
#endif

	snprintf(szTemp, sizeof(szTemp) - 1, "%s%s", g_application_dir, szFileName);
	fp = fopen(szTemp, read_mode);
	if (fp) {
		return fp;
	}

	return NULL;
}

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
unsigned int TimeGet()
{
#ifdef WIN32
	return timeGetTime();
#else
	struct timeval start;
	gettimeofday(&start, NULL);
	unsigned int dwTime = start.tv_sec * 1000 + start.tv_usec / 1000;
	return dwTime;
#endif
}
