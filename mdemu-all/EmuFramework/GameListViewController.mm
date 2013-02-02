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
    
    DJOfferBannerStyle bannerType = kDJBannerStyle320_50;
    if (isPad()) {
        bannerType = kDJBannerStyle480_50;
    }
    _banner = [[DianJinOfferBanner alloc] initWithOfferBanner:CGPointMake(0, 0) style:bannerType];
    DianJinTransitionParam *transitionParam = [[DianJinTransitionParam alloc] init];
    transitionParam.animationType = kDJTransitionCube;
    transitionParam.animationSubType = kDJTransitionFromTop;
    transitionParam.duration = 1.0;
    [_banner setupTransition:transitionParam];
    [_banner startWithTimeInterval:20 delegate:self];
    _banner.center = CGPointMake(rect.size.width / 2, 25);
    [self.view addSubview:_banner];

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
    
    [self adjustView:self.interfaceOrientation];
}

-(void)onClickApp
{
    [MobClick event:@"moreapp"];
    [[DianJinOfferPlatform defaultPlatform]showOfferWall: self delegate:self];
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

    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    NSArray* array = [defaults arrayForKey:@"purchaseList"];

    if (array == nil) {
        m_purchaseList = [NSMutableArray array];
    } else {
        m_purchaseList = [[NSMutableArray alloc]initWithArray:array];
    }
}

-(void)adjustView:(UIInterfaceOrientation)toInterfaceOrientation
{
    float width = self.view.bounds.size.width > self.view.bounds.size.height ? self.view.bounds.size.width : self.view.bounds.size.height;
    float height = self.view.bounds.size.width > self.view.bounds.size.height ? self.view.bounds.size.height : self.view.bounds.size.width;
    
    if (UIInterfaceOrientationIsLandscape(toInterfaceOrientation)) {
        m_tableView.frame = CGRectMake(0, 100, width, height - 100);
        _banner.center = CGPointMake(width / 2, 25);
    } else {
        m_tableView.frame = CGRectMake(0, 100, height, width - 100);
        _banner.center = CGPointMake(height / 2, 25);
    }
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
    [self adjustView:toInterfaceOrientation];
    [m_tableView reloadData];
}

-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (YES);
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
    return @"";
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
    if (isPad()) {
        return 120;
    } else {
        return 80;
    }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell* cell = nil;
    
    CGRect rectImage = CGRectMake(2, 10, 57, 57);
    CGRect rectName = CGRectMake(60, 5, 110, 20);
    CGRect rectIntro = CGRectMake(60, 30, 230, 50);
    CGRect rectRate = CGRectMake(220, 0, 85, 15);
    CGRect rectLock = CGRectMake(280, 5, 16, 16);
    CGRect rectNew = CGRectMake(260, 55, 36, 16);

    float fontSize = 14;
    
    if (isPad()) {
        if (UIInterfaceOrientationIsLandscape(self.interfaceOrientation)) {
            rectImage = CGRectMake(2, 10, 100, 100);
            rectName = CGRectMake(120, 3, 220, 25);
            rectIntro = CGRectMake(120, 30, 800, 80);
            rectRate = CGRectMake(830, -5, 90, 30);
            rectLock = CGRectMake(900, 5, 28, 28);
            rectNew = CGRectMake(880, 80, 36, 16);
        } else {
            rectImage = CGRectMake(2, 10, 100, 100);
            rectName = CGRectMake(120, 3, 220, 25);
            rectIntro = CGRectMake(120, 30, 500, 80);
            rectRate = CGRectMake(560, -5, 90, 30);
            rectLock = CGRectMake(640, 5, 28, 28);
            rectNew = CGRectMake(640, 80, 36, 16);
        }
        fontSize = 20;
    } else {
        if (UIInterfaceOrientationIsLandscape(self.interfaceOrientation)) {
            rectImage = CGRectMake(2, 10, 57, 57);
            rectName = CGRectMake(60, 5, 110, 20);
            rectIntro = CGRectMake(60, 30, 400, 50);
            rectRate = CGRectMake(360, 2, 85, 15);
            rectLock = CGRectMake(440, 5, 16, 16);
            rectNew = CGRectMake(410, 55, 36, 16);
        }    
    }

    static NSString* cellIdent = @"MyCellSetting";
    cell = [tableView dequeueReusableCellWithIdentifier:cellIdent];
    if (cell == nil) {
        cell = [[UITableViewCell alloc]initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdent];
        cell.selectionStyle = UITableViewCellSelectionStyleGray;
        cell.accessoryType = UITableViewCellAccessoryNone;
        
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
    icon.frame = rectImage;
    UILabel* displayName = (UILabel*)[cell.contentView viewWithTag:301];
    displayName.frame = rectName;
    UILabel* intro = (UILabel*)[cell.contentView viewWithTag:302];
    intro.frame = rectIntro;
    DLStarRatingControl* rate = (DLStarRatingControl*)[cell.contentView viewWithTag:303];
    rate.frame = rectRate;
    UIImageView* imgLock = (UIImageView*)[cell.contentView viewWithTag:304];
    imgLock.frame = rectLock;
    TDBadgeView* imgNew = (TDBadgeView*)[cell.contentView viewWithTag:305];
    imgNew.frame = rectNew;

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
        NSString* title = [NSString stringWithFormat:@"消耗10M币解锁此游戏，或者消耗150M币解锁全部游戏。您可以通过安装精品推荐应用的方式免费获取M币，当前M币:%d", g_currentMB];
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
            if (g_currentMB < 150) {
                [MobClick event:@"purchaseall" label:@"false"];
                UIAlertView* alert = [[UIAlertView alloc]initWithTitle:@"错误" message:[NSString stringWithFormat:@"M币不足，当前M币:%d",g_currentMB] delegate:nil cancelButtonTitle:@"知道了" otherButtonTitles:nil];
                [alert show];
            } else {
                [MobClick event:@"purchaseall" label:@"true"];
                g_currentMB -= 150;
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
    NSDictionary* dict = [[[m_romData objectAtIndex:indexPath.section]objectForKey:@"roms"]objectAtIndex:indexPath.row];
    romPath = [dict objectForKey:@"rom"];
    isUserRom = NO;
    
    [MobClick event:@"openrom" label:romPath];
    
    m_currentSelectRom = romPath;
    
    if (![self isRomPurchase:indexPath notify:YES]) {
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
