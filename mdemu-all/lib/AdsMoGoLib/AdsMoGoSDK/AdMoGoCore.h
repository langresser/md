//
//  AdMoGoCore.h
//  mogosdk
//
//  Created by MOGO on 12-5-24.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


#import "AdMoGoAdNetworkAdapter.h"
#import "AdMoGoView.h"

#import "AdMoGoCoreTimerFireDelegate.h"



typedef  enum{
    AdShowSuccess = 0, // 展示成功
    AdShowError, // 展示失败
    AdShowNull   // 展示轮空
}AdShowStatus;

@class AdMoGoAdNetworkAdapter;

@interface AdMoGoCore : NSObject
{
    /*
     用户配置信息key
     */
    NSString  *config_key;
    
    id<AdMoGoWebBrowserControllerUserDelegate> adWebBrowswerDelegate;
}
@property(nonatomic,assign) AdMoGoView *mainView;
@property(nonatomic,retain) AdMoGoAdNetworkAdapter *adapter;
@property(nonatomic,retain) NSTimer *timer;
@property(nonatomic,copy) NSString *config_key;
@property(nonatomic,assign) id<AdMoGoCoreTimerFireDelegate> timerFireDelegate;
@property(nonatomic,assign) BOOL isStop;

@property(nonatomic,assign) id<AdMoGoWebBrowserControllerUserDelegate> adWebBrowswerDelegate;

/*
 轮换广告启动
 */
- (void) core;
- (void) core:(NSNumber *)number;
/*
    无法获取广告
    调用此方法
 */
- (void) showError;

/*
    成功获取广告
    调用此方法
 */
- (void) showSuccess;

/*
 添加广告视图
 */
- (void) pushView:(UIView *) subview;

/*
    停止定时器线程
 */
- (void) stopTimer;

/*
    重新启动定时器线程
 */
- (void) fireTimer;

/*
    立即触发定时器
 */
- (void) immediatelyFireTimer;

- (void) stopCurrentThread;
 

#pragma mark Adapter callbacks old code



- (void)adRequestReturnsForAdapter:(AdMoGoAdNetworkAdapter *)adapter;

- (void)adapter:(AdMoGoAdNetworkAdapter *)adapter didGetAd:(NSString *)adType;

- (void)adapter:(AdMoGoAdNetworkAdapter *)adapter didReceiveAdView:(UIView *)view ;

- (void)adapter:(AdMoGoAdNetworkAdapter *)adapter didFailAd:(NSError *)error ;

- (void)adapterDidFinishAdRequest:(AdMoGoAdNetworkAdapter *)adapter;
/*
    发送点击计数
 */
- (void)sendRecordNum;

/*
    特殊发送
 */
- (void)specialSendRecordNum;


- (void)adDidStartRequestAd;

//- (void)adMoGoDidReceiveAd;

/*
    添加百度广告
 */
- (void)baiduAddAdViewAdapter:(AdMoGoAdNetworkAdapter *)adapter didReceiveAdView:(UIView *)view;

/*
    添加百度广告
 */
- (void)baiduSendRIB;

/*
    特殊适配器请求回调
 */
- (void)specialAdapter:(AdMoGoAdNetworkAdapter *)adapter didGetAd:(NSString *)adType;
/*
    特殊适配器请求成功回调
 */
- (void)specialAdapter:(AdMoGoAdNetworkAdapter *)adapter didReceiveAdView:(UIView *)view ;
/*
    特殊适配器请求失败回调
 */
- (void)specialAdapter:(AdMoGoAdNetworkAdapter *)adapter didFailAd:(NSError *)error ;



@end
