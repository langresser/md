//
//  SettingViewController.m
//  MD
//
//  Created by 王 佳 on 12-9-5.
//  Copyright (c) 2012年 Gingco.Net New Media GmbH. All rights reserved.
//

#import "SettingViewController.h"
#import "iOS.hh"
#include "gfx/interface.h"
#import <QuartzCore/QuartzCore.h>
#include "EmuSystem.hh"
#include "Option.hh"
#import "UIDevice+Util.h"
#import "MDGameViewController.h"
#import "UIGlossyButton.h"
#import "UMFeedback.h"


int g_currentMB = 0;
int g_rotation = 0;

@implementation SettingViewController

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
    settingView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, 320, 480)];
    settingView.backgroundColor = [UIColor colorWithRed:240.0 / 255 green:248.0 / 255 blue:1.0 alpha:1.0];
    m_tableView = [[UITableView alloc]initWithFrame:CGRectMake(0, 50, 320, 430) style:UITableViewStyleGrouped];
    m_tableView.delegate = self;
    m_tableView.dataSource = self;
    m_tableView.backgroundColor = [UIColor colorWithRed:240.0 / 255 green:248.0 / 255 blue:1.0 alpha:1.0];
    [m_tableView setBackgroundView:nil];
    [settingView addSubview:m_tableView];

    UIGlossyButton* btnBack = [[UIGlossyButton alloc]initWithFrame:CGRectMake(220, 10, 80, 30)];
    [btnBack setTitle:@"返回游戏" forState:UIControlStateNormal];
    [btnBack addTarget:self action:@selector(onClickBack) forControlEvents:UIControlEventTouchUpInside];

	[btnBack useWhiteLabel: YES];
    btnBack.tintColor = [UIColor brownColor];
	[btnBack setShadow:[UIColor blackColor] opacity:0.8 offset:CGSizeMake(0, 1) blurRadius: 4];
    [btnBack setGradientType:kUIGlossyButtonGradientTypeLinearSmoothBrightToNormal];
//    btnBack.alpha = 0.7;
    [settingView addSubview:btnBack];
    
    UIGlossyButton* btnBackList = [[UIGlossyButton alloc]initWithFrame:CGRectMake(20, 10, 80, 30)];
    [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [btnBackList setTitle:@"游戏列表" forState:UIControlStateNormal];
    [btnBackList addTarget:self action:@selector(onClickBackList) forControlEvents:UIControlEventTouchUpInside];
    
//    btnBackList.alpha = 0.7;
    [btnBackList useWhiteLabel: YES];
    btnBackList.tintColor = [UIColor brownColor];
	[btnBackList setShadow:[UIColor blackColor] opacity:0.8 offset:CGSizeMake(0, 1) blurRadius: 4];
    [btnBackList setGradientType:kUIGlossyButtonGradientTypeLinearSmoothBrightToNormal];
    [settingView addSubview:btnBackList];
    
    g_rotation = [[NSUserDefaults standardUserDefaults]integerForKey:@"rotation"];
    
    [self setView:settingView];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.contentSizeForViewInPopover = CGSizeMake(320, 480);
	// Do any additional setup after loading the view.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return interfaceOrientation == UIInterfaceOrientationPortrait;
}

-(void)adjustView:(UIInterfaceOrientation)toInterfaceOrientation
{
    float width = self.view.bounds.size.width > self.view.bounds.size.height ? self.view.bounds.size.width : self.view.bounds.size.height;
    float height = self.view.bounds.size.width > self.view.bounds.size.height ? self.view.bounds.size.height : self.view.bounds.size.width;
    
    if (UIInterfaceOrientationIsLandscape(toInterfaceOrientation)) {
        settingView.frame = CGRectMake(0, 0, width, height);
        m_tableView.frame = CGRectMake(0, 50, width, height - 50);
    } else {
        settingView.frame = CGRectMake(0, 0, height, width);
        m_tableView.frame = CGRectMake(0, 50, height, width - 50);
    }
}

-(BOOL)shouldAutorotate
{
    return NO;
}

- (NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAll;
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
    [self adjustView:toInterfaceOrientation];
    [m_tableView reloadData];
}

-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    g_currentMB = [[NSUserDefaults standardUserDefaults]integerForKey:@"MB"];
    [m_tableView reloadData];
}

-(void)onClickBackList
{
    EmuSystem::saveAutoState(1);
    EmuSystem::saveBackupMem();
    EmuSystem::resetAutoSaveStateTime();
    

    if (isPad()) {
        [[MDGameViewController sharedInstance].popoverVC dismissPopoverAnimated:YES];
    } else {
        [self dismissModalViewControllerAnimated:NO];
    }
    
    [[MDGameViewController sharedInstance]showGameList];
}

-(void)onClickBack
{
    if (isPad()) {
        [[MDGameViewController sharedInstance].popoverVC dismissPopoverAnimated:YES];
    } else {
        [self dismissModalViewControllerAnimated:YES];
    }
    
    EmuSystem::start();
    
    extern void onViewChange(void * = 0, Gfx::GfxViewState * = 0);
    onViewChange();
    Base::displayNeedsUpdate();
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 2;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    if (section == 0) {
        return @"系统设置";
    } else if (section == 1) {
        return @"其他";
    }
    return @"";
}

- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
{
    return @"";
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if (section == 0) {
        return 7;
    } else if (section == 1) {
        return 4;
    }
    return 0;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
{
    return 40;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 44;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell* cell = nil;
    
    if (indexPath.section == 0) {
        if (indexPath.row == 2) {
            static NSString *CellIdentifier = @"PickCell";
            CPPickerViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
            
            if (cell == nil) {
                cell = [[CPPickerViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
            }
            
            cell.textLabel.text = @"存档位置";
            cell.textLabel.font = [UIFont systemFontOfSize:17];
            cell.dataSource = self;
            cell.delegate = self;
            cell.currentIndexPath = indexPath;
            cell.showGlass = YES;
            cell.peekInset = UIEdgeInsetsMake(0, 35, 0, 35);
            
            [cell reloadData];            
            [cell selectItemAtIndex:EmuSystem::saveStateSlot + 1 animated:NO];
            
            return cell;
        } else if (indexPath.row == 3) {
            static NSString *CellIdentifier = @"PickCell2";
            CPPickerViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
            
            if (cell == nil) {
                cell = [[CPPickerViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
            }
            
            cell.textLabel.text = @"手柄样式";
            cell.textLabel.font = [UIFont systemFontOfSize:17];
            cell.dataSource = self;
            cell.delegate = self;
            cell.currentIndexPath = indexPath;
            cell.showGlass = YES;
            cell.peekInset = UIEdgeInsetsMake(0, 35, 0, 35);
            
            [cell reloadData];
            extern BasicByteOption option6BtnPad;
            [cell selectItemAtIndex:option6BtnPad ? 1 : 0 animated:NO];
            
            return cell;
        } else {
            static NSString* cellIdent = @"MyCellMy";
            cell = [tableView dequeueReusableCellWithIdentifier:cellIdent];
            if (!cell) {
                cell = [[UITableViewCell alloc]initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdent];
                cell.textLabel.font = [UIFont systemFontOfSize:17.0];
                cell.selectionStyle = UITableViewCellSelectionStyleGray;
                cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
            }
            
            FsSys::cPath saveStr;
            if (EmuSystem::saveStateSlot == -1) {
                snprintf(saveStr, 256, "%s/save/%s.0%c.gp", Base::documentsPath(), EmuSystem::gameName, 'A');
            } else {
                snprintf(saveStr, 256, "%s/save/%s.0%c.gp", Base::documentsPath(), EmuSystem::gameName, '0' + EmuSystem::saveStateSlot);
            }
            
            NSString* saveStatus;
            
            NSFileManager* fm = [NSFileManager defaultManager];
            
            if ([fm fileExistsAtPath:[NSString stringWithUTF8String:saveStr]]) {
                NSDictionary* attr = [fm attributesOfItemAtPath:[NSString stringWithUTF8String:saveStr] error:nil];
                NSDate* time = [attr objectForKey:NSFileModificationDate];
                
                static NSDateFormatter *dateFormatter = nil;
                if (dateFormatter == nil) {
                    dateFormatter = [[NSDateFormatter alloc] init];
                    [dateFormatter setDateStyle:NSDateFormatterShortStyle];
                    [dateFormatter setTimeStyle:NSDateFormatterShortStyle];
                }
                
                NSString* timestamp = [dateFormatter stringFromDate:time];

                if (EmuSystem::saveStateSlot == -1) {
                    saveStatus = [NSString stringWithFormat:@"自动存档: %@", timestamp];
                } else {
                    saveStatus = [NSString stringWithFormat:@"%d 存档位: %@", EmuSystem::saveStateSlot, timestamp];
                }
                
            } else {
                if (EmuSystem::saveStateSlot == -1) {
                    saveStatus = @"自动存档: 空白";
                } else {
                    saveStatus = [NSString stringWithFormat:@"%d 存档位: 空白", EmuSystem::saveStateSlot];
                }
            }
            
            switch (indexPath.row) {
                case 0:
                    cell.textLabel.text = @"读档";
                    cell.detailTextLabel.text = saveStatus;
                    cell.accessoryType = UITableViewCellAccessoryNone;
                    break;
                case 1:
                    cell.textLabel.text = @"存档";
                    cell.detailTextLabel.text = saveStatus;
                    cell.accessoryType = UITableViewCellAccessoryNone;
                    break;
                case 4:
                    cell.textLabel.text = @"重置游戏";
                    cell.detailTextLabel.text = @"";
                    cell.accessoryType = UITableViewCellAccessoryNone;
                    break;
                case 5:
                {
                    cell.textLabel.text = @"竖屏显示";
                    cell.detailTextLabel.text = @"";
                    cell.accessoryType = g_rotation != 0 ? UITableViewCellAccessoryCheckmark : UITableViewCellAccessoryNone;
                }
                    break;
                case 6:
                    cell.textLabel.text = @"高级设置";
                    cell.detailTextLabel.text = @"";
                    cell.accessoryType = UITableViewCellAccessoryNone;
                    break;
                default:
                    break;
            }
        }
    } else if (indexPath.section == 1) {
        static NSString* cellIdent = @"MyCellGongl";
        cell = [tableView dequeueReusableCellWithIdentifier:cellIdent];
        if (!cell) {
            cell = [[UITableViewCell alloc]initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdent];
            cell.textLabel.font = [UIFont systemFontOfSize:17.0];
            cell.selectionStyle = UITableViewCellSelectionStyleGray;
            cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        }
        
        switch (indexPath.row) {
            case 0:
                cell.textLabel.text = @"意见反馈";
                cell.detailTextLabel.text = @"此非即时通讯，您可以通过微博与我们联系";
                break;
            case 1:
                cell.textLabel.text = @"新浪微博(@BananaStudio)";
                cell.detailTextLabel.text = @"关注我们，了解最新游戏发布及版本更新";
                break;
            case 2:
                cell.textLabel.text = @"腾讯微博(@BananaStudio)";
                cell.detailTextLabel.text = @"关注我们，了解最新游戏发布及版本更新";
                break;
            case 3:
                cell.textLabel.text = @"精品推荐";
                cell.detailTextLabel.text = @"";
                break;
            default:
                break;
        }
    }
    
    return cell;
}

// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return NO;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    if (indexPath.section == 0) {
        switch (indexPath.row) {
            case 0:
                [self onClickBack];
                EmuSystem::loadState();
                break;
            case 1:
                [self onClickBack];
                EmuSystem::saveState();
                break;
            case 4:
            {
                UIAlertView* alert = [[UIAlertView alloc]initWithTitle:@"" message:@"是否重置游戏?" delegate:self cancelButtonTitle:@"取消" otherButtonTitles:@"确定", nil];
                alert.tag = 100;
                [alert show];
            }
                break;
            case 5:
            {
                g_rotation = g_rotation ? 0 : 1;
                [[NSUserDefaults standardUserDefaults]setInteger:g_rotation forKey:@"rotation"];
                [[NSUserDefaults standardUserDefaults]synchronize];
                [m_tableView reloadData];
                
                extern void resetOrientation();
                resetOrientation();
            }
                break;
            case 6:
                [self onClickBack];
                extern void restoreMenuFromGame();
                restoreMenuFromGame();
                break;
            default:
                break;
        }
    } else if (indexPath.section == 1) {
        switch (indexPath.row) {
            case 0:
                [UMFeedback showFeedback:self withAppkey:kUmengAppKey];
                break;
            case 1:
                openWebsite("http://weibo.com/bananastudi0");
                break;
            case 2:
                openWebsite("http://t.qq.com/BananaStudio");
                break;
            case 3:
                [[DianJinOfferPlatform defaultPlatform]showOfferWall: self delegate:self];
                break;
                break;
            default:
                break;
        }
    }
}

#pragma mark - CPPickerViewCell DataSource

- (NSInteger)numberOfItemsInPickerViewAtIndexPath:(NSIndexPath *)pickerPath {
    if (pickerPath.row == 2) {
        return 11;
    } else if (pickerPath.row == 3) {
        return 2;
    }
    
    return 0;
}

- (NSString *)pickerViewAtIndexPath:(NSIndexPath *)pickerPath titleForItem:(NSInteger)item {
    if (pickerPath.row == 2) {
        if (item == 0) {
            return @"自动";
        } else {
            return [NSString stringWithFormat:@"%d", item - 1];
        }
    } else if (pickerPath.row == 3) {
        if (item == 0) {
            return @"3按钮";
        } else {
            return @"6按钮";
        }
    }
    
    
    return nil;
}

#pragma mark - CPPickerViewCell Delegate

- (void)pickerViewAtIndexPath:(NSIndexPath *)pickerPath didSelectItem:(NSInteger)item {
    
    if (pickerPath.row == 2) {
        EmuSystem::saveStateSlot = (item - 1);
    } else if (pickerPath.row == 3) {
        extern void setupMDInput();
        extern BasicByteOption option6BtnPad;
        if (item == 0) {
            option6BtnPad = 0;
            setupMDInput();
        } else if (item == 1) {
            option6BtnPad = 1;
            setupMDInput();
        }
    }

    [m_tableView reloadSections:[NSIndexSet indexSetWithIndex:0] withRowAnimation:UITableViewRowAnimationNone];
}


- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex;
{
    if (alertView.tag == 100) {
        if (buttonIndex == 1) {
            [self onClickBack];
            EmuSystem::resetGame();
        }
    } else {
        if (buttonIndex == 1) {
            [[DianJinOfferPlatform defaultPlatform]showOfferWall: self delegate:self];
        }
    }
    
}

@end
