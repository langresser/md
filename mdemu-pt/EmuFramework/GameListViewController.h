//
//  GameListViewController.h
//  MD
//
//  Created by 王 佳 on 12-9-8.
//  Copyright (c) 2012年 Gingco.Net New Media GmbH. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <DianJinOfferPlatform/DianJinOfferPlatform.h>
#import <DianJinOfferPlatform/DianJinOfferBanner.h>
#import <DianJinOfferPlatform/DianJinBannerSubViewProperty.h>
#import <DianJinOfferPlatform/DianJinTransitionParam.h>
#import <DianJinOfferPlatform/DianJinOfferPlatformProtocol.h>

#import "AdMoGoDelegateProtocol.h"
#import "AdMoGoView.h"
#import "AdMoGoWebBrowserControllerUserDelegate.h"

@interface GameListViewController : UIViewController<UITableViewDataSource, UITableViewDelegate, DianJinOfferBannerDelegate, UIAlertViewDelegate, DianJinOfferPlatformProtocol, AdMoGoDelegate,AdMoGoWebBrowserControllerUserDelegate>
{
    UITableView* m_tableView;
    
    NSMutableArray* m_romData;
    NSMutableArray* m_purchaseList;
    
    NSString* m_currentSelectRom;
    
    AdMoGoView *adView;
}
@end
