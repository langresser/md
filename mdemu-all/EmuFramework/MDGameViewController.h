//
//  SettingView.h
//  MD
//
//  Created by 王 佳 on 12-8-20.
//  Copyright (c) 2012年 Gingco.Net New Media GmbH. All rights reserved.
//
#pragma once
#import "iosUtil.h"
#import "SettingViewController.h"
#import "GameListViewController.h"

#define kUmengAppKey @"504b6946527015169e00004f"
#define kMongoAppKey @"1df841c4721346c7abc9bc917339c74b"
#define kDianjinAppId 10036
#define kDianjinAppKey @"0f3294fd5e50445ca4d28a259409ffd0"

@interface MDGameViewController : UIViewController<UIPopoverControllerDelegate>
{
    GameListViewController* gameListVC;
    SettingViewController* settingVC;
    UIPopoverController * popoverVC;
}

@property(nonatomic, strong) GameListViewController* gameListVC;
@property(nonatomic, strong) SettingViewController* settingVC;
@property(nonatomic, strong) UIPopoverController * popoverVC;
+(MDGameViewController*)sharedInstance;

-(void)showGameList;
@end

