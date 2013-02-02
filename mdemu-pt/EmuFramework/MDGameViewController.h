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

#define kUmengAppKey @"509bfd385270152926000094"
#define kMongoAppKey @"754f4cb22e904b69ba5d47d057fcea7e"
#define kDianjinAppId 12517
#define kDianjinAppKey @"0bafd91b9f356e51a868293e6ca4dc24"

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

