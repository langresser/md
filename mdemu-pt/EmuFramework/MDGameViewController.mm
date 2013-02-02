//
//  SettingView.m
//  MD
//
//  Created by 王 佳 on 12-8-20.
//  Copyright (c) 2012年 Gingco.Net New Media GmbH. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MDGameViewController.h"
#import "EAGLView.h"
#import "iOS.hh"
#include "gfx/interface.h"
#import <QuartzCore/QuartzCore.h>
#include "EmuSystem.hh"
#import "UIDevice+Util.h"

MDGameViewController* g_delegate = nil;

@implementation MDGameViewController
@synthesize settingVC, popoverVC;
@synthesize gameListVC;

+(MDGameViewController*)sharedInstance
{
    if (g_delegate == nil) {
        g_delegate = [[MDGameViewController alloc]init];
    }   
    
    return g_delegate;
}

-(void)showSettingPopup
{
    if (isPad()) {
        if (popoverVC == nil) {
            settingVC = [[SettingViewController alloc]initWithNibName:nil bundle:nil];
            popoverVC = [[UIPopoverController alloc] initWithContentViewController:settingVC];       
            popoverVC.delegate = self;
        }

        CGRect rect;
        switch (Gfx::preferedOrientation) {
            case Gfx::VIEW_ROTATE_0:
                rect = CGRectMake(750, 60, 10, 10);
                break;
            case Gfx::VIEW_ROTATE_270:
                rect = CGRectMake(0, 60, 10, 10);
                break;
            case Gfx::VIEW_ROTATE_90:
                rect = CGRectMake(750, 960, 10, 10);
                break;
            default:
                rect = CGRectMake(750, 60, 10, 10);
                break;
        }
        [popoverVC presentPopoverFromRect:rect inView:self.view permittedArrowDirections:UIPopoverArrowDirectionUp animated:YES];
    } else {
        if (settingVC == nil) {
            settingVC = [[SettingViewController alloc]initWithNibName:nil bundle:nil];
        }

        [self presentModalViewController:settingVC animated:YES];
    }
}

-(void)showGameList
{
    if (gameListVC == nil) {
        gameListVC = [[GameListViewController alloc]init];
    }
    
    [self presentModalViewController:gameListVC animated:NO];
}

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController
{
    EmuSystem::start();
    
    extern void onViewChange(void * = 0, Gfx::GfxViewState * = 0);
    onViewChange();

    Base::displayNeedsUpdate();
}
@end


void showSetting()
{
    EmuSystem::pause();
    [[MDGameViewController sharedInstance] showSettingPopup];
}
