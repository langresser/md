//
//  iosUtil.m
//  smcios
//
//  Created by 王 佳 on 12-8-8.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "iosUtil.h"
#import "CGJoystick.h"

bool isPad()
{
	BOOL result = NO;
	if ([[UIDevice currentDevice] respondsToSelector:@selector(userInterfaceIdiom)]) {
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 30200
		result = ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad);
#endif
	}
	return result;
}

void openWebsite(const char* url)
{
    NSURL* urlx = [NSURL URLWithString:[NSString stringWithUTF8String:url]];
    [[UIApplication sharedApplication]openURL:urlx];
}

void getScreenSize(int* width, int* height)
{
    CGSize size = [UIScreen mainScreen].bounds.size;
    if (width) {
        *width = size.width > size.height ? size.width : size.height;
    }
    
    if (height) {
        *height = size.width > size.height ? size.height : size.width;
    }
}

extern char g_application_dir[256];
extern char g_resource_dir[256];
void initDir()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString  *plistPath = [paths objectAtIndex:0];
    strlcpy(g_resource_dir, [plistPath UTF8String], sizeof(g_resource_dir));
    
    g_resource_dir[strlen(g_resource_dir)] = '/';
}

void getFileStatus(const char* pszName)
{
    NSFileManager* fmgr = [NSFileManager defaultManager];
    if (!fmgr) {
        return;
    }
    
    NSError* error;
    NSDictionary* attr = [fmgr attributesOfItemAtPath:[NSString stringWithUTF8String:pszName] error:&error];
    
    
    NSLog(@"file: %s    attr:%@ ", pszName, attr);
}
