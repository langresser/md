//
//  GameListViewController.m
//  MD
//
//  Created by 王 佳 on 12-9-8.
//  Copyright (c) 2012年 Gingco.Net New Media GmbH. All rights reserved.
//

#import "GameListViewController.h"
#import "JSONKit.h"
#import "iosUtil.h"
#import <QuartzCore/QuartzCore.h>
#import "DLStarRatingControl.h"
#include "EmuSystem.hh"
#import "UIDevice+Util.h"
#import "UIGlossyButton.h"
#import "MobClick.h"
#import "TDBadgedCell.h"
#import "MDGameViewController.h"

extern int g_currentMB ;

@interface GameListViewController ()

@end

@interface RomData : NSObject
@property(nonatomic, strong) NSString* displayName;
@property(nonatomic, strong) NSString* intro;
@property(nonatomic, strong) NSString* detail;
@property(nonatomic, strong) NSString* iconPath;
@property(nonatomic, strong) NSString* screenPath;
@property(nonatomic, strong) NSString* romPath;
@property(nonatomic) float star;
@property(nonatomic) int price;
@end

@implementation RomData
@synthesize displayName, intro, detail, iconPath, screenPath, romPath, star, price;
@end


@interface RomTableCell : UITableViewCell
@property(nonatomic, strong) UILabel* displayName;
@property(nonatomic, strong) UILabel* intro;
@property(nonatomic, strong) UIImageView* icon;
@end

@implementation RomTableCell
@synthesize displayName, intro, icon;
- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        CGRect rect = self.contentView.bounds;
        
    }
    return self;
}
@end

@implementation GameListViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

-(void)loadView
{
    CGRect rect = [[UIScreen mainScreen]bounds];
    self.view = [[UIView alloc]initWithFrame:rect];
    self.view.backgroundColor = [UIColor colorWithRed:240.0 / 255 green:248.0 / 255 blue:1.0 alpha:1.0];
    CGRect rectTable = CGRectMake(0, 100, rect.size.width, rect.size.height - 100);
    m_tableView = [[UITableView alloc]initWithFrame:rectTable style:UITableViewStyleGrouped];
    m_tableView.dataSource = self;
    m_tableView.delegate = self;
    m_tableView.backgroundColor = [UIColor colorWithRed:240.0 / 255 green:248.0 / 255 blue:1.0 alpha:1.0];
    [m_tableView setBackgroundView:nil];
    [self.view addSubview:m_tableView];
    
//    DJOfferBannerStyle bannerType = kDJBannerStyle320_50;
//    if (isPad()) {
//        bannerType = kDJBannerStyle480_50;
//    }
//    DianJinOfferBanner *_banner = [[DianJinOfferBanner alloc] initWithOfferBanner:CGPointMake(0, 0) style:bannerType];
//    DianJinTransitionParam *transitionParam = [[DianJinTransitionParam alloc] init];
//    transitionParam.animationType = kDJTransitionCube;
//    transitionParam.animationSubType = kDJTransitionFromTop;
//    transitionParam.duration = 1.0;
//    [_banner setupTransition:transitionParam];
//    [_banner startWithTimeInterval:20 delegate:self];
//    _banner.center = CGPointMake(rect.size.width / 2, 25);
//    [self.view addSubview:_banner];

    int offset = 0;
    if (isPad()) {
        offset = 40;
    }
    UIGlossyButton* btnApp = [[UIGlossyButton alloc]initWithFrame:CGRectMake(10 + offset, 60, 80, 30)];
    [btnApp setTitle:@"获取M币" forState:UIControlStateNormal];
    btnApp.titleLabel.font = [UIFont systemFontOfSize:15];
    [btnApp addTarget:self action:@selector(onClickApp) forControlEvents:UIControlEventTouchUpInside];
    
    [btnApp useWhiteLabel: YES];
    btnApp.tintColor = [UIColor brownColor];
	[btnApp setShadow:[UIColor blackColor] opacity:0.8 offset:CGSizeMake(0, 1) blurRadius: 4];
    [btnApp setGradientType:kUIGlossyButtonGradientTypeLinearSmoothBrightToNormal];
    [self.view addSubview:btnApp];
    
    g_currentMB = [[NSUserDefaults standardUserDefaults]integerForKey:@"MB"];

    UILabel* label = [[UILabel alloc]initWithFrame:CGRectMake(100 + offset, 60, 100, 30)];
    label.backgroundColor = [UIColor clearColor];
    label.tag = 400;
    label.font = [UIFont systemFontOfSize:15];
    label.text = [NSString stringWithFormat:@"(当前M币:%d)", g_currentMB];
    [self.view addSubview:label];
    
    offset = 220;
    if (isPad()) {
        offset = 620;
    }
    UIGlossyButton* btnRefresh = [[UIGlossyButton alloc]initWithFrame:CGRectMake(10 + offset, 60, 80, 30)];
    [btnRefresh setTitle:@"刷新" forState:UIControlStateNormal];
    btnRefresh.titleLabel.font = [UIFont systemFontOfSize:15];
    [btnRefresh addTarget:self action:@selector(onClickRefresh) forControlEvents:UIControlEventTouchUpInside];
    
    [btnRefresh useWhiteLabel: YES];
    btnRefresh.tintColor = [UIColor brownColor];
	[btnRefresh setShadow:[UIColor blackColor] opacity:0.8 offset:CGSizeMake(0, 1) blurRadius: 4];
    [btnRefresh setGradientType:kUIGlossyButtonGradientTypeLinearSmoothBrightToNormal];
    [self.view addSubview:btnRefresh];
}

-(void)onClickApp
{
    [MobClick event:@"moreapp"];
    [[DianJinOfferPlatform defaultPlatform]showOfferWall: self delegate:self];
}

-(void)onClickRefresh
{
    NSMutableDictionary* myGame = [m_romData objectAtIndex:0];
    NSMutableArray* myGameList = [myGame objectForKey:@"roms"];
    [myGameList removeAllObjects];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
	NSArray* dirContents = [fileManager contentsOfDirectoryAtPath:basePath error:nil];
    for (NSString* path in dirContents) {
        BOOL isDir = NO;
        NSString* fullPath = [basePath stringByAppendingPathComponent:path];
        if (![fileManager fileExistsAtPath:fullPath isDirectory:&isDir] || isDir) {
            continue;
        }
        
        if ([[fullPath pathExtension] compare:@"db"] == 0) {
            continue;
        }
        
        if ([path compare:@"config"] == 0) {
            continue;
        }
        
        [myGameList addObject:path];
    }
    
    [m_tableView reloadData];
}

-(void)onClickBack
{
    [self dismissModalViewControllerAnimated:NO];
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    NSError* error = nil;
    NSString* filePath = [[NSBundle mainBundle]pathForResource:@"romlist" ofType:@"json"];
    NSString* jsonString = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:&error];
    NSArray* romArray = [jsonString objectFromJSONString];
    m_romData = [NSMutableArray arrayWithArray:romArray];
    
    NSMutableDictionary* myGame = [[NSMutableDictionary alloc]init];
    NSMutableArray* myGameList = [[NSMutableArray alloc]init];
    [myGame setObject:@"我的游戏" forKey:@"group"];
    [myGame setObject:myGameList forKey:@"roms"];
    [m_romData insertObject:myGame atIndex:0];

    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    NSArray* array = [defaults arrayForKey:@"purchaseList"];

    if (array == nil) {
        m_purchaseList = [NSMutableArray array];
    } else {
        m_purchaseList = [[NSMutableArray alloc]initWithArray:array];
    }
    
    adView = [[AdMoGoView alloc] initWithAppKey:kMongoAppKey
                                         adType:AdViewTypeNormalBanner expressMode:NO
                             adMoGoViewDelegate:self];
    adView.adWebBrowswerDelegate = self;
    adView.frame = CGRectMake(0, 0, 320, 50);
    adView.center = CGPointMake(self.view.bounds.size.width / 2, 25);
    [self.view addSubview:adView];
    // Do any additional setup after loading the view from its nib.
}

- (UIViewController *)viewControllerForPresentingModalView{
    return self;
}

- (void)adMoGoDidStartAd:(AdMoGoView *)adMoGoView{
    NSLog(@"广告开始请求回调");
} /**
   * 广告接收成功回调
   */
- (void)adMoGoDidReceiveAd:(AdMoGoView *)adMoGoView{
    NSLog(@"广告接收成功回调"); }
/**
 * 广告接收失败回调 */
- (void)adMoGoDidFailToReceiveAd:(AdMoGoView *)adMoGoView didFailWithError:(NSError *)error{
    NSLog(@"广告接收失败回调"); }
/**
 * 点击广告回调 */
- (void)adMoGoClickAd:(AdMoGoView *)adMoGoView{ NSLog(@"点击广告回调");
}

-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    [self onClickRefresh];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return [m_romData count];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    NSDictionary* dict = [m_romData objectAtIndex:section];
    return [dict objectForKey:@"group"];
}

- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
{
    if (section == 0) {
        return @"将自己的游戏Rom通过手机助手上传到游戏的Documents文件夹下";
    } else {
        return @"";
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [[[m_romData objectAtIndex:section]objectForKey:@"roms"]count];
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return 30;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    if (indexPath.section == 0) {
        return 44;
    }

    if (isPad()) {
        return 120;
    } else {
        return 80;
    }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell* cell = nil;
    
    if (indexPath.section == 0) {
        static NSString* cellIdent = @"MyCellGame";
        cell = [tableView dequeueReusableCellWithIdentifier:cellIdent];
        
        if (cell == nil) {
            cell = [[UITableViewCell alloc]initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdent];
            cell.selectionStyle = UITableViewCellSelectionStyleGray;
            cell.accessoryType = UITableViewCellAccessoryNone;
        }

        NSString* path = [[[m_romData objectAtIndex:0]objectForKey:@"roms"]objectAtIndex:indexPath.row];
        cell.textLabel.text = path;
        
        cell.contentView.layer.masksToBounds=YES;
        cell.contentView.layer.cornerRadius=10.0;
        cell.contentView.layer.borderWidth=1.5;
        cell.contentView.layer.borderColor=[[UIColor darkGrayColor] CGColor];
        return cell;
    } else {
        static NSString* cellIdent = @"MyCellSetting";
        cell = [tableView dequeueReusableCellWithIdentifier:cellIdent];
        if (cell == nil) {
            cell = [[UITableViewCell alloc]initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdent];
            cell.selectionStyle = UITableViewCellSelectionStyleGray;
            cell.accessoryType = UITableViewCellAccessoryNone;
            
            CGRect rectImage = CGRectMake(2, 10, 57, 57);
            CGRect rectName = CGRectMake(60, 5, 110, 20);
            CGRect rectIntro = CGRectMake(60, 30, 230, 50);
            CGRect rectRate = CGRectMake(180, 0, 90, 30);
            CGRect rectLock = CGRectMake(280, 5, 16, 16);
            CGRect rectNew = CGRectMake(260, 55, 36, 16);
            float fontSize = 14;
            
            if (isPad()) {
                rectImage = CGRectMake(2, 10, 100, 100);
                rectName = CGRectMake(120, 3, 220, 25);
                rectIntro = CGRectMake(120, 30, 500, 80);
                rectRate = CGRectMake(200, -5, 90, 30);
                rectLock = CGRectMake(640, 5, 28, 28);
                rectNew = CGRectMake(640, 80, 36, 16);
                fontSize = 20;
            }
            
            UIImageView* image = [[UIImageView alloc]initWithFrame:rectImage];
            image.backgroundColor = [UIColor clearColor];
            image.tag = 300;
            UILabel* name = [[UILabel alloc]initWithFrame:rectName];
            name.backgroundColor = [UIColor clearColor];
            name.font = [UIFont systemFontOfSize:fontSize];
            name.tag = 301;
            UILabel* intro = [[UILabel alloc]initWithFrame:rectIntro];
            intro.backgroundColor = [UIColor clearColor];
            intro.font = [UIFont systemFontOfSize:fontSize - 2];
            intro.textColor = [UIColor grayColor];
            intro.numberOfLines = 0;
            intro.tag = 302;
            
            TDBadgeView* newBadge = [[TDBadgeView alloc]initWithFrame:rectNew];
            newBadge.radius = 9;
            newBadge.badgeColor = [UIColor colorWithRed:0.197 green:0.592 blue:0.219 alpha:1.000];
            [newBadge setParent:cell];
            [newBadge setBadgeString:@"New"];
            [newBadge setShowShadow:YES];
            newBadge.tag = 305;
            
            DLStarRatingControl* rate = [[DLStarRatingControl alloc]initWithFrame:rectRate];
            rate.autoresizingMask =  UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin;
            rate.tag = 303;
            
            UIImageView* imgLock = [[UIImageView alloc]initWithImage:[UIImage imageNamed:@"gui_lock"]];
            imgLock.frame = rectLock;
            imgLock.tag = 304;
            
            cell.selectionStyle = UITableViewCellSelectionStyleGray;
            [cell.contentView addSubview:image];
            [cell.contentView addSubview:name];
            [cell.contentView addSubview:intro];
            [cell.contentView addSubview:rate];
            [cell.contentView addSubview:imgLock];
            [cell.contentView addSubview:newBadge];
            
            //为视图增加边框
            image.layer.masksToBounds=YES;
            image.layer.cornerRadius=10.0;
            image.layer.borderWidth=1.5;
            image.layer.borderColor=[[UIColor darkGrayColor] CGColor];
            
            cell.contentView.layer.masksToBounds=YES;
            cell.contentView.layer.cornerRadius=10.0;
            cell.contentView.layer.borderWidth=1.5;
            cell.contentView.layer.borderColor=[[UIColor darkGrayColor] CGColor];
        }

        UIImageView* icon = (UIImageView*)[cell.contentView viewWithTag:300];
        UILabel* displayName = (UILabel*)[cell.contentView viewWithTag:301];
        UILabel* intro = (UILabel*)[cell.contentView viewWithTag:302];
        DLStarRatingControl* rate = (DLStarRatingControl*)[cell.contentView viewWithTag:303];
        UIImageView* imgLock = (UIImageView*)[cell.contentView viewWithTag:304];
        TDBadgeView* imgNew = (TDBadgeView*)[cell.contentView viewWithTag:305];

        NSDictionary* dict = [[[m_romData objectAtIndex:indexPath.section]objectForKey:@"roms"]objectAtIndex:indexPath.row];
        NSString* romPath = [dict objectForKey:@"rom"];
        
#if 0
        NSString* fullPath = [[[NSBundle mainBundle]bundlePath]stringByAppendingPathComponent:romPath];
        BOOL isDir = NO;
        if (![[NSFileManager defaultManager]fileExistsAtPath:fullPath isDirectory:&isDir]) {
            NSLog(@"file not found: %@", romPath);
        }
#endif
        
        if (romPath == nil) {
            displayName.textColor = [UIColor grayColor];
        }

        NSString* fileName = [[romPath stringByDeletingPathExtension]stringByDeletingPathExtension];
        icon.image = [UIImage imageNamed:[fileName stringByAppendingPathExtension:@"png"]];
        if (icon.image == nil) {
            icon.image = [UIImage imageNamed:[fileName stringByAppendingPathExtension:@"jpg"]];
        }
        displayName.text = [dict objectForKey:@"name"];
        intro.text = [dict objectForKey:@"intro"];
        float rateNum = [[dict objectForKey:@"star"]floatValue];
        rate.rating = rateNum;
        bool isNew = [[dict objectForKey:@"new"]intValue] != 0;
        imgNew.hidden = !isNew;
        
        imgLock.hidden = [self isRomPurchase:indexPath notify:NO];
    }
    
    return cell;
}

// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return NO;
}

- (void)appActivatedDidFinish:(NSDictionary *)resultDic
{
    NSLog(@"%@", resultDic);
    NSNumber *result = [resultDic objectForKey:@"result"];
    if ([result boolValue]) {
        NSNumber *awardAmount = [resultDic objectForKey:@"awardAmount"];
        NSString *identifier = [resultDic objectForKey:@"identifier"];
        NSLog(@"app identifier = %@", identifier);
        g_currentMB += [awardAmount floatValue];
        
        [self updateMBInfo];
    }
}

-(BOOL)isRomPurchase:(NSIndexPath*)indexPath notify:(BOOL)notify
{
    if ([self isHackEnable]) {
        return YES;
    }

    NSDictionary* dict = [[[m_romData objectAtIndex:indexPath.section]objectForKey:@"roms"]objectAtIndex:indexPath.row];
    NSString* romPath = [dict objectForKey:@"rom"];
    int purchase = [[dict objectForKey:@"purchase"]intValue];
    if (purchase <= 0) {
        return YES;
    }
    
    for (NSString* path in m_purchaseList) {
        if ([romPath compare:path] == NSOrderedSame) {
            return YES;
        }
    }
    
    if (notify) {
        NSString* title = [NSString stringWithFormat:@"消耗10M币解锁此游戏，或者消耗70M币解锁全部游戏。您可以通过安装精品推荐应用的方式免费获取M币，当前M币:%d", g_currentMB];
        UIAlertView* alert = [[UIAlertView alloc]initWithTitle:@"提示" message:title delegate:self cancelButtonTitle:@"知道了" otherButtonTitles:@"解锁", @"解锁全部", @"获取M币", nil];
        alert.tag = 600;
        [alert show];
    }
    
    return NO;
}

-(BOOL)isHackEnable
{
    NSString* flag = [[NSUserDefaults standardUserDefaults] stringForKey:kRemoveAdsFlag];
    if (flag && [flag isEqualToString:[[UIDevice currentDevice] uniqueDeviceIdentifier]]) {
        return YES;
    }
    
    return NO;
}

-(void)updateMBInfo
{
    UILabel* label = (UILabel*)[self.view viewWithTag:400];
    label.text = [NSString stringWithFormat:@"(当前M币:%d)", g_currentMB];

    [[NSUserDefaults standardUserDefaults]setInteger:g_currentMB forKey:@"MB"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (alertView.tag == 600) {
        if (buttonIndex == 1) {
            [MobClick event:@"purchase" label:m_currentSelectRom];
            if (g_currentMB < 10) {
                UIAlertView* alert = [[UIAlertView alloc]initWithTitle:@"错误" message:[NSString stringWithFormat:@"M币不足，当前M币:%d",g_currentMB] delegate:nil cancelButtonTitle:@"知道了" otherButtonTitles:nil];
                [alert show];
            } else {
                [m_purchaseList addObject:m_currentSelectRom];
                [[NSUserDefaults standardUserDefaults]setObject:m_purchaseList forKey:@"purchaseList"];
                [[NSUserDefaults standardUserDefaults] synchronize];

                g_currentMB -= 10;
                [self updateMBInfo];

                [m_tableView reloadData];
            }
        } else if (buttonIndex == 2) {
            if (g_currentMB < 70) {
                [MobClick event:@"purchaseall" label:@"false"];
                UIAlertView* alert = [[UIAlertView alloc]initWithTitle:@"错误" message:[NSString stringWithFormat:@"M币不足，当前M币:%d",g_currentMB] delegate:nil cancelButtonTitle:@"知道了" otherButtonTitles:nil];
                [alert show];
            } else {
                [MobClick event:@"purchaseall" label:@"true"];
                g_currentMB -= 70;
                [self updateMBInfo];

                [[NSUserDefaults standardUserDefaults] setObject:[[UIDevice currentDevice] uniqueDeviceIdentifier] forKey:kRemoveAdsFlag];
                [[NSUserDefaults standardUserDefaults] synchronize];
                
                [m_tableView reloadData];
            }
            
        } else if (buttonIndex == 3) {
            [MobClick event:@"moreapp"];
            [[DianJinOfferPlatform defaultPlatform]showOfferWall:self delegate:self];
        }
    }
}

std::string g_romPath = "";
bool g_isUserRom = false;
extern void startGameFromMenu();
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    NSString* romPath = nil;
    BOOL isUserRom = NO;
    if (indexPath.section == 0) {
        romPath = [[[m_romData objectAtIndex:indexPath.section]objectForKey:@"roms"]objectAtIndex:indexPath.row];
        isUserRom = YES;
    } else {
        NSDictionary* dict = [[[m_romData objectAtIndex:indexPath.section]objectForKey:@"roms"]objectAtIndex:indexPath.row];
        romPath = [dict objectForKey:@"rom"];
        isUserRom = NO;
    }
    
    [MobClick event:@"openrom" label:romPath];
    
    m_currentSelectRom = romPath;
    
    if (indexPath.section != 0 && ![self isRomPurchase:indexPath notify:YES]) {
        return;
    }
    
    [self onClickBack];
    
    extern int openglViewIsInit;
    if (openglViewIsInit == 0) {
        g_romPath = [romPath UTF8String];
        g_isUserRom = isUserRom;
    } else {
        // 默认打开
        if(EmuSystem::loadGame([romPath UTF8String], false, isUserRom))
        {
            startGameFromMenu();
        }
    }
}


@end
