//
//  File: AdMoGoAdNetworkAdapter.h
//  Project: AdsMOGO iOS SDK
//  Version: 1.1.7
//
//  Copyright 2011 AdsMogo.com. All rights reserved.
//

/*
    所有适配器父类 old code
 */
#import "AdMoGoDelegateProtocol.h"
#import "AdMoGoCore.h"
#import "AdMoGoWebBrowserControllerUserDelegate.h"

typedef enum {
	AdMoGoAdNetworkTypeAdMob             = 1,
	AdMoGoAdNetworkTypeJumpTap           = 2,
	AdMoGoAdNetworkTypeVideoEgg          = 3,
	AdMoGoAdNetworkTypeMedialets         = 4,
	AdMoGoAdNetworkTypeLiveRail          = 5,
	AdMoGoAdNetworkTypeMillennial        = 6,
	AdMoGoAdNetworkTypeGreyStripe        = 7,
	AdMoGoAdNetworkTypeQuattro           = 8,
	AdMoGoAdNetworkTypeCustom            = 9,
	AdMoGoAdNetworkTypeAdMoGo10          = 10,
	AdMoGoAdNetworkTypeMobClix           = 11,
	AdMoGoAdNetworkTypeMdotM             = 12,
	AdMoGoAdNetworkTypeAdMoGo13          = 13,
	AdMoGoAdNetworkTypeGoogleAdSense     = 14,
	AdMoGoAdNetworkTypeGoogleDoubleClick = 15,
	AdMoGoAdNetworkTypeGeneric           = 16,
	AdMoGoAdNetworkTypeEvent             = 17,
	AdMoGoAdNetworkTypeInMobi            = 18,
	AdMoGoAdNetworkTypeIAd               = 19,
	AdMoGoAdNetworkTypeZestADZ           = 20,
	
	AdMoGoAdNetworkTypeAdChina      = 21,
	AdMoGoAdNetworkTypeWiAd         = 22,
	AdMoGoAdNetworkTypeWooboo       = 23,
	AdMoGoAdNetworkTypeYouMi        = 24,
	AdMoGoAdNetworkTypeCasee        = 25,
	AdMoGoAdNetworkTypeSmartMAD     = 26,
	AdMoGoAdNetworkTypeMoGo         = 27,
    AdMoGoAdNetworkTypeAdTouch      = 28,
    AdMoGoAdNetworkTypeDoMob        = 29,
    AdMoGoAdNetworkTypeAdOnCN       = 30,
    AdMoGoAdNetworkTypeMobiSage     = 31,
    AdMoGoAdNetworkTypeAirAd        = 32,
    AdMoGoAdNetworkTypeAdwo         = 33,
    AdMoGoAdNetworkTypeSmaato       = 35,
    AdMoGoAdNetworkTypeIZP          = 40,
    AdMoGoAdNetworkTypeBaiduMobAd   = 44,
    AdMoGoAdNetworkTypeExchange     = 45,
    AdMoGoAdNetworkTypeLMMOB        = 46,
    AdMoGoAdNetworkTypePremiumAD    = 48,
    AdMoGoAdNetworkTypeAdFractal    = 50,
    AdMoGoAdNetworkTypeSuiZong      = 51,
    AdMoGoAdNetworkTypeWinsAd       = 52,
    AdMoGoAdNetworkTypeMobWinAd     = 53,
    AdMoGoAdNetworkTypeRecommendAD  = 54,
    AdMoGoAdNetworkTypeUM           = 55,
    AdMoGoAdNetworkTypeWQ           = 56,
    AdMoGoAdNetworkTypeAdermob      = 57,
    AdMoGoAdNetworkTypeAllyesi      = 59,
    AdMoGoAdNetworkTypeAduu         = 60,
    AdMoGoAdNetworkTypeUMAppUnion   = 62,
    AdMoGoAdNetworkTypeMiidi        = 63,
    AdMoGoAdNetworkTypeAdChinaFullAd = 2100,
    AdMoGoAdNetworkTypeYouMiFullAd  = 2400,
    AdMoGoAdNetworkTypeAdFractalFullAd = 5000,
    
    AdMoGoAdNetworkTypeInMobiSDK = 180,
    AdMoGoAdNetworkTypeAdFractalSDK = 500,
    AdMoGoAdNetworkTypeWQSDK = 560,
    AdMoGoAdNetworkTypeWiAdFullAd  = 2200,
    AdMoGoAdNetworkTypeWoobooFullAd = 2300,
    AdMoGoAdNetworkTypeDoMobFullAd = 2900,
    AdMoGoAdNetworkTypeMobiSageFullAd = 3100,
    AdMoGoAdNetworkTypeSuiZongFullAd = 5100,
    
    AdMoGoAdNetworkTypeAdFractalFullScreenSDK = 50000,

} AdMoGoAdNetworkType;

@class AdMoGoView;
//@class AdMoGoConfig;
@class AdMoGoCore;
@class AdMoGoAdNetworkConfig;

@interface AdMoGoAdNetworkAdapter : NSObject {
	id<AdMoGoDelegate> adMoGoDelegate;
	AdMoGoView *adMoGoView;
//	AdMoGoConfig *adMoGoConfig;
    AdMoGoCore *adMoGoCore;
//	AdMoGoAdNetworkConfig *networkConfig;
	UIView *adNetworkView;
    NSDictionary *ration;
    id<AdMoGoWebBrowserControllerUserDelegate> adWebBrowswerDelegate;
    /*
        2012-9-11 特殊id
     */
    NSString *specialID;
}

/*
- (id)initWithAdMoGoDelegate:(id<AdMoGoDelegate>)delegate
						view:(AdMoGoView *)view
					   core:(AdMoGoCore *)core
			   networkConfig:(AdMoGoAdNetworkConfig *)netConf;
*/

- (id)initWithAdMoGoDelegate:(id<AdMoGoDelegate>)delegate
                        view:(AdMoGoView *)view
                        core:(AdMoGoCore *)core
               networkConfig:(NSDictionary *)netConf;

/*
- (id)initWithAdMoGoDelegateview:(AdMoGoView *)view
					  config:(AdMoGoConfig *)config
                   networkConfig:(AdMoGoAdNetworkConfig *)netConf;
 */
- (void)getAd;
- (void)stopBeingDelegate;
- (void)stopTimer;

- (BOOL)shouldSendExMetric;
- (BOOL)shouldSendExRequest;

- (void)stopAd;

- (void)rotateToOrientation:(UIInterfaceOrientation)orientation;
/*
- (BOOL)isBannerAnimationOK:(AMBannerAnimationType)animType;
*/
@property (nonatomic,assign) id<AdMoGoDelegate> adMoGoDelegate;

@property (nonatomic,assign) AdMoGoView *adMoGoView;
@property (nonatomic,assign) AdMoGoCore *adMoGoCore;
@property (nonatomic,retain) AdMoGoAdNetworkConfig *networkConfig;
@property (nonatomic,retain) UIView *adNetworkView;
@property (nonatomic,retain) NSDictionary *ration;

@property (nonatomic,assign) id<AdMoGoWebBrowserControllerUserDelegate> adWebBrowswerDelegate;
/*
 2012-9-11 特殊id
 */
@property (nonatomic,retain) NSString *specialID;
@end