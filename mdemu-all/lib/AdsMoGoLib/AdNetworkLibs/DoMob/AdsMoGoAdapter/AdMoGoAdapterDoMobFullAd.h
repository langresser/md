//
//  AdMoGoAdapterDoMobFullAd.h
//  AdsMogo
//
//  Created by MOGO on 12-8-30.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "AdMoGoAdNetworkAdapter.h"
#import "DMInterstitialAdController.h"

@interface AdMoGoAdapterDoMobFullAd : AdMoGoAdNetworkAdapter<DMInterstitialAdControllerDelegate>{
    DMInterstitialAdController *_dmInterstitial;
    BOOL isStop;
}

@end
