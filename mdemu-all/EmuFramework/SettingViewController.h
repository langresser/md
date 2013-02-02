//
//  SettingViewController.h
//  MD
//
//  Created by 王 佳 on 12-9-5.
//  Copyright (c) 2012年 Gingco.Net New Media GmbH. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "iosUtil.h"
#import <DianJinOfferPlatform/DianJinOfferPlatform.h>
#import <DianJinOfferPlatform/DianJinBannerSubViewProperty.h>
#import <DianJinOfferPlatform/DianJinTransitionParam.h>
#import <DianJinOfferPlatform/DianJinOfferPlatformProtocol.h>
#import "CPPickerViewCell.h"

@interface SettingViewController : UIViewController<UITableViewDataSource, UITableViewDelegate, UIAlertViewDelegate, DianJinOfferPlatformProtocol, CPPickerViewCellDelegate, CPPickerViewCellDataSource>
{
    UIView* settingView;
    UITableView* m_tableView;
}

@end
