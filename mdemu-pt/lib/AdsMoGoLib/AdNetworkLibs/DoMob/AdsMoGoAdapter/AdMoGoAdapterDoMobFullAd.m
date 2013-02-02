//
//  AdMoGoAdapterDoMobFullAd.m
//  AdsMogo
//
//  Created by MOGO on 12-8-30.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "AdMoGoAdapterDoMobFullAd.h"
#import "AdMoGoAdNetworkRegistry.h"
#import "AdMoGoAdNetworkAdapter+Helpers.h"
#import "AdMoGoAdNetworkConfig.h" 
#import "AdMoGoConfigDataCenter.h"
#import "AdMoGoConfigData.h"
#import "AdMoGoDeviceInfoHelper.h"
@implementation AdMoGoAdapterDoMobFullAd

+ (AdMoGoAdNetworkType)networkType{
    return AdMoGoAdNetworkTypeDoMobFullAd;
}

+ (void)load{
    [[AdMoGoAdNetworkRegistry sharedRegistry] registerClass:self];
}

- (void)getAd{
    isStop = NO;
    
    //获取用于展示插屏光的UIViewController
    UIViewController *uiViewController = [self.adMoGoDelegate viewControllerForPresentingModalView];
    if(uiViewController){
        
    //　初始化插屏广告
    _dmInterstitial = [[DMInterstitialAdController alloc] 
                       initWithPublisherId:[self.ration objectForKey:@"key"]
                       rootViewController:uiViewController];
    // 设置插屏广告的Delegate
    _dmInterstitial.delegate = self;
    //开始加载广告
    [_dmInterstitial loadAd];
    }
}
-(void)stopBeingDelegate{
    if(_dmInterstitial){
        _dmInterstitial.delegate = nil;
        [_dmInterstitial release],_dmInterstitial = nil;
    }
}

- (void)stopAd{
    [self stopBeingDelegate];
    isStop = YES;
    
}



- (void)dealloc {
    
    if(_dmInterstitial){
        _dmInterstitial.delegate = nil;
        [_dmInterstitial release],_dmInterstitial = nil;
    }
    
    [super dealloc];
}

#pragma mark DoMob Delegate
// 当插屏广告被成功加载后，回调该方法
- (void)dmInterstitialSuccessToLoadAd:(DMInterstitialAdController *)dmInterstitial{
    NSLog(@"%s",__FUNCTION__);
    if (isStop) {
        return;
    }
    [dmInterstitial present];
    
    [self helperNotifyDelegateOfFullScreenAdModal];
    [adMoGoCore adapter:self didReceiveAdView:nil];
}
// 当插屏广告加载失败后，回调该方法
- (void)dmInterstitialFailToLoadAd:(DMInterstitialAdController *)dmInterstitial withError:(NSError *)err{
    NSLog(@"%s",__FUNCTION__);
    if(isStop){
        return;
    }
    
    [adMoGoCore adapter:self didFailAd:nil];
    
}

// 当插屏广告要被呈现出来前，回调该方法
- (void)dmInterstitialWillPresentScreen:(DMInterstitialAdController *)dmInterstitial{
    NSLog(@"%s",__FUNCTION__);
}
// 当插屏广告被关闭后，回调该方法
- (void)dmInterstitialDidDismissScreen:(DMInterstitialAdController *)dmInterstitial{
    NSLog(@"%s",__FUNCTION__);
    
}

// 当将要呈现出 Modal View 时，回调该方法。如打开内置浏览器。
- (void)dmInterstitialWillPresentModalView:(DMInterstitialAdController *)dmInterstitial{
    NSLog(@"%s",__FUNCTION__);
}
// 当呈现的 Modal View 被关闭后，回调该方法。如内置浏览器被关闭。
- (void)dmInterstitialDidDismissModalView:(DMInterstitialAdController *)dmInterstitial{
    NSLog(@"%s",__FUNCTION__);
}
// 当因用户的操作（如点击下载类广告，需要跳转到Store），需要离开当前应用时，回调该方法
- (void)dmInterstitialApplicationWillEnterBackground:(DMInterstitialAdController *)dmInterstitial{
    NSLog(@"%s",__FUNCTION__);
}

@end
