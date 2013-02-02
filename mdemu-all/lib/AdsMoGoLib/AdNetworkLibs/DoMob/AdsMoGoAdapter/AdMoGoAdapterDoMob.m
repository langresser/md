//
//  File: AdMoGoAdapterDoMob.m
//  Project: AdsMOGO iOS SDK
//  Version: 1.1.9
//
//  Copyright 2011 AdsMogo.com. All rights reserved.
//

#import "AdMoGoAdapterDoMob.h"
#import "AdMoGoAdNetworkRegistry.h"
#import "AdMoGoAdNetworkAdapter+Helpers.h"
#import "AdMoGoAdNetworkConfig.h" 

#import "AdMoGoConfigDataCenter.h"
#import "AdMoGoConfigData.h"
#import "AdMoGoDeviceInfoHelper.h"
@implementation AdMoGoAdapterDoMob


+ (AdMoGoAdNetworkType)networkType{
    return AdMoGoAdNetworkTypeDoMob;
}

+ (void)load{
    [[AdMoGoAdNetworkRegistry sharedRegistry] registerClass:self];
}

- (void)getAd{
    
    isStop = NO;
    
    [adMoGoCore adapter:self didGetAd:@"domob"];
    [adMoGoCore adDidStartRequestAd];
//    AdViewType type = adMoGoView.adType;
    /*
     获取广告类型
     原来代码：AdViewType type = adMoGoView.adType;
     */
    AdMoGoConfigDataCenter *configDataCenter = [AdMoGoConfigDataCenter singleton];
    
    AdMoGoConfigData *configData = [configDataCenter.config_dict objectForKey:adMoGoCore.config_key];
    AdViewType type = [configData.ad_type intValue];
    
    CGSize size = CGSizeZero;
    switch (type) {
        case AdViewTypeNormalBanner:
        case AdViewTypeiPadNormalBanner:
            size = DOMOB_AD_SIZE_320x50;
            break;
        case AdViewTypeMediumBanner:
            size = DOMOB_AD_SIZE_488x80;
            break;
        case AdViewTypeLargeBanner:
            size = DOMOB_AD_SIZE_728x90;
            break;
        default:
            break;
    }
    
    

    DMAdView *adview = [[DMAdView alloc] initWithPublisherId:[self.ration objectForKey:@"key"] size:size autorefresh:NO];
    
    adview.rootViewController = [adMoGoDelegate viewControllerForPresentingModalView];
    adview.delegate = self;
    [adview loadAd];
    self.adNetworkView = adview;
   
    [adview release];
}

- (void)stopBeingDelegate{
    DMAdView *domobView = (DMAdView *)self.adNetworkView;
    if(domobView != nil) {
        domobView.delegate = nil;
    }
}

- (void)stopAd{
    [self stopBeingDelegate];
    isStop = YES;
    
}



- (void)dealloc {
    [super dealloc];
}

- (void)rotateToOrientation:(UIInterfaceOrientation)orientation{
    
}

#pragma mark DoMob Delegate
- (void)dmAdViewSuccessToLoadAd:(DMAdView *)adView {
    
    if (isStop) {
        return;
    }
    
//    if (timer) {
//        [timer invalidate];
//        [timer release];
//        timer = nil;
//    }
    [adMoGoCore adapter:self didReceiveAdView:adView];
}
- (void)dmAdViewFailToLoadAd:(DMAdView *)adView withError:(NSError *)error {
    
    
    if (isStop) {
        return;
    }
    
//    if (timer) {
//        [timer invalidate];
//        [timer release];
//        timer = nil;
//    }
    [adMoGoCore adapter:self didFailAd:nil];
}

/*
- (void)dmWillPresentModalViewFromAd:(DMAdView *)adView {

}
- (void)dmDidDismissModalViewFromAd:(DMAdView *)adView {
    
}


- (void)dmApplicationWillEnterBackgroundFromAd:(DMAdView *)adView {
    
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
  */
@end