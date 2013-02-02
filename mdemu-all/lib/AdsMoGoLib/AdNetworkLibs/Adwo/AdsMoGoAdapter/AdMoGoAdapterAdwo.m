//
//  File: AdMoGoAdapterAdwo.m
//  Project: AdsMOGO iOS SDK
//  Version: 1.1.9
//
//  Copyright 2011 AdsMogo.com. All rights reserved.
//

#import "AdMoGoAdapterAdwo.h"
//#import "AdMoGoView.h"
#import "AdMoGoAdNetworkRegistry.h"
#import "AdMoGoAdNetworkAdapter+Helpers.h"
#import "AdMoGoAdNetworkConfig.h" 
#import "AdMoGoConfigDataCenter.h"
#import "AdMoGoConfigData.h"
#import "AdMoGoDeviceInfoHelper.h"

@implementation AdMoGoAdapterAdwo

+ (AdMoGoAdNetworkType)networkType {
	return AdMoGoAdNetworkTypeAdwo;
}

+ (void)load {
	[[AdMoGoAdNetworkRegistry sharedRegistry] registerClass:self];
}

- (void)getAd{
    
    isStop = NO;
    
    [adMoGoCore adDidStartRequestAd];
    [adMoGoCore adapter:self didGetAd:@"adwo"];
    
    NSInteger isTest = 1;
    
    
    AdMoGoConfigDataCenter *configDataCenter = [AdMoGoConfigDataCenter singleton];
    
    AdMoGoConfigData *configData = [configDataCenter.config_dict objectForKey:adMoGoCore.config_key];
    
    if ([configData istestMode]) {
        isTest = 0;
    }
    
    timer = [[NSTimer scheduledTimerWithTimeInterval:10 target:self selector:@selector(loadAdTimeOut:) userInfo:nil repeats:NO] retain];
    
    AdViewType type =[configData.ad_type intValue];
    
    if (type == AdViewTypeNormalBanner) {
        AWAdView *awAdView = [[AWAdView alloc] initWithAdwoPid:[self.ration objectForKey:@"key"]  adIdType:1 adTestMode:isTest adSizeForPad:ADWO_ADS_BANNER_SIZE_FOR_IPAD_320x50];
        awAdView.delegate = self;
        awAdView.adRequestTimeIntervel = 600;
        awAdView.userGpsEnabled = NO;
        [awAdView loadAd];
        self.adNetworkView = awAdView;
        [awAdView release];
    }
    else if (type == AdViewTypeLargeBanner) {
        AWAdView *awAdView = [[AWAdView alloc] initWithAdwoPid:[self.ration objectForKey:@"key"] adIdType:1 adTestMode:isTest adSizeForPad:ADWO_ADS_BANNER_SIZE_FOR_IPAD_720x110];
        awAdView.delegate = self;
        awAdView.adRequestTimeIntervel = 600;
        awAdView.userGpsEnabled = NO;
        [awAdView loadAd];
        self.adNetworkView = awAdView;
        [awAdView release];
    }
}

- (void)stopBeingDelegate {
	AWAdView *adView = (AWAdView *)self.adNetworkView;
    if(adView != nil)
    {
        adView.delegate = nil;
        [adView killTimer];
    }
}

- (void)stopAd{
    [self stopBeingDelegate];
    isStop = YES;
    [self stopTimer];
}

- (void)stopTimer {
    if (timer) {
        [timer invalidate];
        [timer release];
        timer = nil;
    }
}

- (void)dealloc {
    if (timer) {
        [timer invalidate];
        [timer release];
        timer = nil;
    }
	[super dealloc];
}

#pragma mark implement AWDelegate method
- (UIViewController *)viewControllerForPresentingModalView{
    
    if (isStop) {
        return nil;
    }
    
    return [adMoGoDelegate viewControllerForPresentingModalView];
}

- (void)adViewDidFailToLoadAd:(AWAdView *)view{
    
    if (isStop) {
        return;
    }
    
    if (timer) {
        [timer invalidate];
        [timer release];
        timer = nil;
    }
    [adMoGoCore adapter:self didFailAd:nil];
}

- (void)adViewDidLoadAd:(AWAdView *)view{
    if (isStop) {
        return;
    }
    if (timer) {
        [timer invalidate];
        [timer release];
        timer = nil;
    }
    [adMoGoCore adapter:self didReceiveAdView:view];
}

- (void)willPresentModalViewForAd:(AWAdView *)view{
    if (isStop) {
        return;
    }
    [self helperNotifyDelegateOfFullScreenModal];
}

- (void)didDismissModalViewForAd:(AWAdView *)view{
    if (isStop) {
        return;
    }
    [self helperNotifyDelegateOfFullScreenModalDismissal];
}

- (void)loadAdTimeOut:(NSTimer*)theTimer {
    if (isStop) {
        return;
    }
    if (timer) {
        [timer invalidate];
        [timer release];
        timer = nil;
    }
    [self stopBeingDelegate];
    [adMoGoCore adapter:self didFailAd:nil];
}
@end