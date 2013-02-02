//
//  File: AdMoGoAdapterDoMob.h
//  Project: AdsMOGO iOS SDK
//  Version: 1.2.1
//
//  Copyright 2011 AdsMogo.com. All rights reserved.
//Domob v3.x

#import "AdMoGoAdNetworkAdapter.h"
#import "DMAdView.h"

@interface AdMoGoAdapterDoMob : AdMoGoAdNetworkAdapter <DMAdViewDelegate>{
    NSTimer *timer;
    BOOL isStop;
}
+ (AdMoGoAdNetworkType)networkType;
@end
