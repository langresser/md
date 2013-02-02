//
//  AdMoGoView.h
//  mogosdk
//
//  Created by MOGO on 12-5-28.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "AdMoGoDelegateProtocol.h"
#import "AdMoGoWebBrowserControllerUserDelegate.h"
#import "AdViewType.h"
/*
  AdViewType 广告视图类型 不是配置信息请求type
 */

@interface AdMoGoView : UIView<AdMoGoDelegate>{
    
    BOOL isStop;
    
}
@property(nonatomic,assign) id<AdMoGoDelegate> delegate;



@property(nonatomic,assign) id<AdMoGoWebBrowserControllerUserDelegate> adWebBrowswerDelegate;

/*
    ak:开发appkey
    type:广告视图类型
    express:消息模式 是否是快速模式
    delegate:代理对象
 */
- (id) initWithAppKey:(NSString *)ak
                adType:(AdViewType)type
                expressMode:(BOOL)express
                adMoGoViewDelegate:(id<AdMoGoDelegate>) delegate;


/*
    ak:开发appkey
    type:请求广告类型
    delegate:代理对象
 */
- (id) initWithAppKey:(NSString *)ak
               adType:(AdViewType)type
   adMoGoViewDelegate:(id<AdMoGoDelegate>) delegate;





@end


