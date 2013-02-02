//
//  AdMoGoConfigDataCenter.h
//  AdsMogo
//
//  Created by MOGO on 12-6-7.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AdMoGoConfigData.h"
#import "AdMogoConfigDic.h"
@interface AdMoGoConfigDataCenter : NSObject

/*
    config_dict 保存多个用户配置 
 */
@property(readonly,retain,nonatomic) AdMogoConfigDic * config_dict;


/*
 获取用户配置实体
 单例模式
 */
+(id) singleton;
//- (NSMutableDictionary *) config_dict;

/*
    获取用户配置实体
 */
+ (AdMoGoConfigData *) getconfigDictByKey:(NSString *)key;

+ (void) deleteAllData;
@end
