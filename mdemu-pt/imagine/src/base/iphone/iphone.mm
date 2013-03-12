#define thisModuleName "base:iphone"

#import "MainApp.h"
#import "EAGLView.h"
#import <dlfcn.h>
#import <unistd.h>

#include <base/interface.h>
#include <base/common/funcs.h>

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import <Foundation/NSPathUtilities.h>
#import "UMFeedback.h"

#import "MobClick.h"
#import "MDGameViewController.h"

#ifdef CONFIG_INPUT
	#include "input.h"
#endif

#ifdef CONFIG_BASE_IOS_ICADE
	#include "ICadeHelper.hh"
#endif

#include <EmuSystem.hh>
EAGLView *glView;
int openglViewIsInit = 0;

namespace Base
{
const char *appPath = 0;
static int pointScale = 1;

static UIWindow *window, *externalWindow = 0;

static EAGLContext *mainContext;
static MainApp *mainApp;
static CADisplayLink *displayLink = 0;
static BOOL displayLinkActive = NO;
static bool isIPad = 0;
static bool usingiOS4 = 0;

#ifdef CONFIG_BASE_IOS_ICADE
static ICadeHelper iCade = { nil };

void ios_setICadeActive(uint active)
{
	iCade.init(glView);
	iCade.setActive(active);
}

uint ios_iCadeActive()
{
	return iCade.isActive();
}
#endif

#if defined(IPHONE_MSG_COMPOSE) || defined(IPHONE_IMG_PICKER)
	static UIViewController *viewCtrl;
#endif

#ifdef CONFIG_INPUT
	#ifdef IPHONE_VKEYBOARD
		//static UITextField *vkbdField;
		static UITextView *vkbdField;
		static uchar inVKeyboard = 0;
		static InputTextCallback vKeyboardTextCallback = NULL;
		static void *vKeyboardTextCallbackUserPtr = NULL;
	#endif
#endif

#ifdef IPHONE_IMG_PICKER
	static UIImagePickerController* imagePickerController;
	static IPhoneImgPickerCallback imgPickCallback = NULL;
	static void *imgPickUserPtr = NULL;
	static NSData *imgPickData[2];
	static uchar imgPickDataElements = 0;
	#include "imagePicker.h"
#endif

#ifdef IPHONE_MSG_COMPOSE
	static MFMailComposeViewController *composeController;
	#include "mailCompose.h"
#endif

#ifdef IPHONE_GAMEKIT
	#include "gameKit.h"
#endif

#ifdef GREYSTRIPE
    #include "greystripe.h"
#endif

static const int USE_DEPTH_BUFFER = 0;
static NSTimer *mainTimer = 0;
static TimerCallbackFunc timerCallbackFunc = 0;
static void *timerCallbackFuncCtx = 0;

void setTimerCallback(TimerCallbackFunc f, void *ctx, int ms)
{
	if(timerCallbackFunc)
	{
		logMsg("canceling callback");
		timerCallbackFunc = 0;
		if(mainTimer)
		{
			[mainTimer invalidate];
			//[mainTimer release];
			mainTimer = 0;
		}
	}
	if(!f)
		return;
	logMsg("setting callback to run in %d ms", ms);
	timerCallbackFunc = f;
	timerCallbackFuncCtx = ctx;
	mainTimer = [NSTimer scheduledTimerWithTimeInterval: (float)ms/1000. target:mainApp selector:@selector(timerCallback:) userInfo:nil repeats: NO];
}

bool isInputDevPresent(uint type)
{
	return 0;
}

void openGLUpdateScreen()
{
	//logMsg("doing swap");
	//glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[Base::mainContext presentRenderbuffer:GL_RENDERBUFFER_OES];
}

void startAnimation()
{
	if(!Base::displayLinkActive)
	{
		displayLink.paused = NO; 
		Base::displayLinkActive = YES;
	}
}

void stopAnimation()
{
	if(Base::displayLinkActive)
	{
		displayLink.paused = YES;
		Base::displayLinkActive = NO;
	}
}

uint appState = APP_RUNNING;

}

// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

@implementation EAGLView

@synthesize context;

// Implement this to override the default layer class (which is [CALayer class]).
// We do this so that our view will be backed by a layer that is capable of OpenGL ES rendering.
+ (Class)layerClass
{
	return [CAEAGLLayer class];
}

-(id)initGLES
{
	// Get our backing layer
	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
	
	eaglLayer.opaque = YES;
	//eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
	//	[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
	//	kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

	context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
	assert(context);
	int ret = [EAGLContext setCurrentContext:context];
	assert(ret);
	/*if (!context || ![EAGLContext setCurrentContext:context])
	{
		[self release];
		return nil;
	}*/
	Base::mainContext = context;
	
	Base::displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView)];
	//displayLink.paused = YES;
	Base::displayLinkActive = YES;
	[Base::displayLink setFrameInterval:1];
	[Base::displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

	return self;
}

#ifdef CONFIG_BASE_IPHONE_NIB
// Init from NIB
- (id)initWithCoder:(NSCoder*)coder
{
	if ((self = [super initWithCoder:coder]))
	{
		self = [self initGLES];
	}
	return self;
}
#endif

// Init from code
-(id)initWithFrame:(CGRect)frame
{
	logMsg("entered initWithFrame");
	if((self = [super initWithFrame:frame]))
	{
		self = [self initGLES];
	}
	logMsg("exiting initWithFrame");
	return self;
}

- (void)drawView
{
	/*TimeSys now;
	now.setTimeNow();
	logMsg("frame time stamp %f, duration %f, now %f", displayLink.timestamp, displayLink.duration, (float)now);*/
	//[EAGLContext setCurrentContext:context];
	//glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    usleep(10);
	if(unlikely(!Base::displayLinkActive))
		return;

	//logMsg("screen update");
	Base::runEngine();
	if(!Base::gfxUpdate)
	{
		Base::stopAnimation();
	}
}

-(void)layoutSubviews
{
    if (openglViewIsInit == 0) {
        [EAGLContext setCurrentContext:context];
        [self destroyFramebuffer];
        [self createFramebuffer];
        Base::engineInit();
        Base::setAutoOrientation(1);
        
        extern std::string g_romPath;
        extern bool g_isUserRom;
        if (!g_romPath.empty()) {
            if(EmuSystem::loadGame(g_romPath.c_str(), false, g_isUserRom))
            {
                startGameFromMenu();
            }
        }
    }
}

extern void startGameFromMenu();
-(void)loadRecentGame
{
    NSString* currentRom = [[NSUserDefaults standardUserDefaults]stringForKey:@"currentRom"];
    if (currentRom == nil || [currentRom length] <= 0) {
        currentRom = @"langrisser2_cn.smd";
        [[NSUserDefaults standardUserDefaults]setObject:currentRom forKey:@"currentRom"];
    }

    // 默认打开
    if(EmuSystem::loadGame([currentRom UTF8String], false, false))
    {
        startGameFromMenu();
    }
}

- (BOOL)createFramebuffer
{
    glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);

	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);

	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);

	if(Base::USE_DEPTH_BUFFER)
	{
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
	}

	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		logMsg("failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	
	openglViewIsInit = 1;
	return YES;
}


- (void)destroyFramebuffer
{
	glDeleteFramebuffersOES(1, &viewFramebuffer);
	viewFramebuffer = 0;
	glDeleteRenderbuffersOES(1, &viewRenderbuffer);
	viewRenderbuffer = 0;

	if(depthRenderbuffer)
	{
		glDeleteRenderbuffersOES(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
	
	openglViewIsInit = 0;
}

- (void)dealloc
{
	if ([EAGLContext currentContext] == context)
	{
		[EAGLContext setCurrentContext:nil];
	}

	[context release];
	[super dealloc];
}

#ifdef CONFIG_INPUT

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	using namespace Base;
	#if defined(IPHONE_VKEYBOARD)
	if(inVKeyboard)
	{
		input_finishKeyboardInput();
		return;
	}
	#endif
	
	for(UITouch* touch in touches)
	{
		iterateTimes((uint)Input::maxCursors, i) // find a free touch element
		{
			if(Input::activeTouch[i] == NULL)
			{
				Input::activeTouch[i] = touch;
				var_copy(p, &Input::m[i]);
				CGPoint startTouchPosition = [touch locationInView:self];
				pointerPos(startTouchPosition.x * pointScale, startTouchPosition.y * pointScale, &p->x, &p->y);
				p->inWin = 1;
				Input::dragStateArr[i].pointerEvent(Input::Pointer::LBUTTON, INPUT_PUSHED, p->x, p->y);
				callSafe(Input::onInputEventHandler, Input::onInputEventHandlerCtx, InputEvent(i, InputEvent::DEV_POINTER, Input::Pointer::LBUTTON, INPUT_PUSHED, p->x, p->y));
				break;
			}
		}
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	using namespace Base;
	for(UITouch* touch in touches)
	{
		iterateTimes((uint)Input::maxCursors, i) // find the touch element
		{
			if(Input::activeTouch[i] == touch)
			{
				var_copy(p, &Input::m[i]);
				CGPoint currentTouchPosition = [touch locationInView:self];
				pointerPos(currentTouchPosition.x * pointScale, currentTouchPosition.y * pointScale, &p->x, &p->y);
				Input::dragStateArr[i].pointerEvent(Input::Pointer::LBUTTON, INPUT_MOVED, p->x, p->y);
				callSafe(Input::onInputEventHandler, Input::onInputEventHandlerCtx, InputEvent(i, InputEvent::DEV_POINTER, Input::Pointer::LBUTTON, INPUT_MOVED, p->x, p->y));
				break;
			}
		}
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	using namespace Base;
	for(UITouch* touch in touches)
	{
		iterateTimes((uint)Input::maxCursors, i) // find the touch element
		{
			if(Input::activeTouch[i] == touch)
			{
				Input::activeTouch[i] = nil;
				var_copy(p, &Input::m[i]);
				p->inWin = 0;
				CGPoint currentTouchPosition = [touch locationInView:self];
				pointerPos(currentTouchPosition.x * pointScale, currentTouchPosition.y * pointScale, &p->x, &p->y);
				Input::dragStateArr[i].pointerEvent(Input::Pointer::LBUTTON, INPUT_RELEASED, p->x, p->y);
				callSafe(Input::onInputEventHandler, Input::onInputEventHandlerCtx, InputEvent(i, InputEvent::DEV_POINTER, Input::Pointer::LBUTTON, INPUT_RELEASED, p->x, p->y));
				break;
			}
		}
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesEnded:touches withEvent:event];
}

#if defined(CONFIG_BASE_IOS_KEY_INPUT) || defined(CONFIG_BASE_IOS_ICADE)
- (BOOL)canBecomeFirstResponder { return YES; }

- (BOOL)hasText { return NO; }

- (void)insertText:(NSString *)text
{
	#ifdef CONFIG_BASE_IOS_ICADE
	if(Base::iCade.isActive())
		Base::iCade.insertText(text);
	#endif
	//logMsg("got text %s", [text cStringUsingEncoding: NSUTF8StringEncoding]);
}

- (void)deleteBackward { }

#ifdef CONFIG_BASE_IOS_ICADE
- (UIView*)inputView
{
	return Base::iCade.dummyInputView;
}
#endif
#endif // defined(CONFIG_BASE_IOS_KEY_INPUT) || defined(CONFIG_BASE_IOS_ICADE)

#endif

@end


@implementation MainApp
static uint iOSOrientationToGfx(UIDeviceOrientation orientation)
{
	switch(orientation)
	{
		case UIDeviceOrientationPortrait: return Gfx::VIEW_ROTATE_0;
		case UIDeviceOrientationLandscapeLeft: return Gfx::VIEW_ROTATE_90;
		case UIDeviceOrientationLandscapeRight: return Gfx::VIEW_ROTATE_270;
		case UIDeviceOrientationPortraitUpsideDown: return Gfx::VIEW_ROTATE_180;
		default : return 255; // TODO: handle Face-up/down
	}
}

UIImageView* pauseImage;
- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    [MobClick startWithAppkey:kUmengAppKey];
    [[DianJinOfferPlatform defaultPlatform] setAppId:kDianjinAppId andSetAppKey:kDianjinAppKey];
	[[DianJinOfferPlatform defaultPlatform] setOfferViewColor:kDJBrownColor];
    [[DianJinOfferPlatform defaultPlatform]setOfferViewAutoRotate:YES];

	using namespace Base;
	NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
	#ifndef NDEBUG
	logMsg("in applicationDidFinishLaunching(), UUID %s", [[[UIDevice currentDevice] uniqueIdentifier] cStringUsingEncoding: NSASCIIStringEncoding]);
	logMsg("iOS version %s", [currSysVer cStringUsingEncoding: NSASCIIStringEncoding]);
	#endif
	mainApp = self;
	
	NSString *reqSysVer = @"4.0";
	if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
	{
		//logMsg("enabling iOS 4 features");
		usingiOS4 = 1;
	}
	
	// TODO: get real DPI if possible
	Gfx::viewMMWidth_ = 50, Gfx::viewMMHeight_ = 75;
	#if !defined(__ARM_ARCH_6K__) && (__IPHONE_OS_VERSION_MAX_ALLOWED >= 30200)
	logMsg("testing for iPad");
	if(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
    	Gfx::viewMMWidth_ = 148, Gfx::viewMMHeight_ = 197;
		isIPad = 1;
		logMsg("running on iPad");
	}
	#endif

	CGRect rect = [[UIScreen mainScreen] bounds];
	newXSize = rect.size.width;
	newYSize = rect.size.height;
	// Create a full-screen window
	window = [[UIWindow alloc] initWithFrame:rect];
	
	#ifdef GREYSTRIPE
	initGS(self);
	#endif
	
	// Create the OpenGL ES view and add it to the window
	glView = [[EAGLView alloc] initWithFrame:rect];
	#if !defined(__ARM_ARCH_6K__)
	if(usingiOS4)
	{
		logMsg("testing for Retina Display");
		if([UIScreen mainScreen].scale == 2.0)
		{
			logMsg("running on Retina Display");
			glView.layer.contentsScale = 2.0;
			pointScale = 2;
			newXSize *= 2;
			newYSize *= 2;
	    }
	}
    #endif
       
    glView.multipleTouchEnabled = YES;

    UIView* view = [[UIView alloc]initWithFrame:rect];
    [view addSubview:glView];
    [[MDGameViewController sharedInstance] setView:view];
    
    if ([[UIDevice currentDevice].systemVersion floatValue] >= 6.0) {
        window.rootViewController = [MDGameViewController sharedInstance];
    } else {
        [window addSubview:[MDGameViewController sharedInstance].view];
    }
//    [window addSubview:[MDGameViewController sharedInstance].view];
    
    CGRect rect1 = glView.bounds;
    pauseImage = [[UIImageView alloc]initWithImage:[UIImage imageNamed:@"pause"]];
    pauseImage.frame = CGRectMake(glView.bounds.size.width - 60, 0, 60, 60);
    pauseImage.alpha = 0.3;
    [glView addSubview:pauseImage];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(orientationChanged:) name:UIDeviceOrientationDidChangeNotification object:nil];

	[UIApplication sharedApplication].idleTimerDisabled = YES;
//  [[MDGameViewController sharedInstance] showGameList];
	[window makeKeyAndVisible];
    [[MDGameViewController sharedInstance] showGameList];
    
    
    
    [UMFeedback checkWithAppkey:kUmengAppKey];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onRecNewMsg:) name:UMFBCheckFinishedNotification object:nil];
    
	#if defined(IPHONE_MSG_COMPOSE) || defined(IPHONE_IMG_PICKER)
	viewCtrl = [[UIViewController alloc] init];
	[viewCtrl setView: glView];
	#endif
	logMsg("exiting applicationDidFinishLaunching");
}

-(void)onRecNewMsg:(NSNotification*)notification
{
    NSArray * newReplies = [notification.userInfo objectForKey:@"newReplies"];
    if (!newReplies) {
        return;
    }
    
    UIAlertView *alertView;
    NSString *title = [NSString stringWithFormat:@"有%d条新回复", [newReplies count]];
    NSMutableString *content = [NSMutableString string];
    for (int i = 0; i < [newReplies count]; i++) {
        NSString * dateTime = [[newReplies objectAtIndex:i] objectForKey:@"datetime"];
        NSString *_content = [[newReplies objectAtIndex:i] objectForKey:@"content"];
        [content appendString:[NSString stringWithFormat:@"%d: %@---%@\n", i+1, _content, dateTime]];
    }
    
    alertView = [[UIAlertView alloc] initWithTitle:title message:content delegate:self cancelButtonTitle:@"确定" otherButtonTitles:nil];
    ((UILabel *) [[alertView subviews] objectAtIndex:1]).textAlignment = NSTextAlignmentLeft ;
    [alertView show];
    
}

- (void)orientationChanged:(NSNotification *)notification
{
    uint o = iOSOrientationToGfx([[UIDevice currentDevice] orientation]);
	if(o == 255)
		return;
    
    if(o != Gfx::VIEW_ROTATE_180)
	{
        extern int g_rotation;
        if (g_rotation) {
            switch (o) {
                case Gfx::VIEW_ROTATE_0:
                case Gfx::VIEW_ROTATE_90:
                case Gfx::VIEW_ROTATE_270:
                    pauseImage.frame = CGRectMake(glView.bounds.size.width - 60, 0, 60, 60);
                    break;
                default:
                    break;
            }
        } else {
            switch (o) {
                case Gfx::VIEW_ROTATE_90:
                    pauseImage.frame = CGRectMake(glView.bounds.size.width - 60, glView.bounds.size.height - 60, 60, 60);
                    break;
                case Gfx::VIEW_ROTATE_270:
                    pauseImage.frame = CGRectMake(0, 0, 60, 60);
                    break;
                case Gfx::VIEW_ROTATE_0:
                    pauseImage.frame = CGRectMake(glView.bounds.size.width - 60, glView.bounds.size.height - 60, 60, 60);
                    break;
                default:
                    break;
            }
        }
        
	}
    
    if (openglViewIsInit == 0) {
        return;
    }

	if(o != Gfx::VIEW_ROTATE_180)
	{
		logMsg("new orientation %s", Gfx::orientationName(o));
		Gfx::preferedOrientation = o;
		Gfx::setOrientation(Gfx::preferedOrientation);
	}
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	logMsg("resign active");
	Base::stopAnimation();
	glFinish();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	using namespace Base;
	logMsg("became active");
	Base::appState = APP_RUNNING;
	if(Base::displayLink)
		Base::startAnimation();
	callSafe(Base::onAppResumeHandler, Base::onAppResumeHandlerCtx, 1);
	#ifdef CONFIG_BASE_IOS_ICADE
	iCade.didBecomeActive();
	#endif
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	using namespace Base;
	logMsg("app exiting");
	//Base::stopAnimation();
	Base::appState = APP_EXITING;
	callSafe(Base::onAppExitHandler, Base::onAppExitHandlerCtx, 0);
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	using namespace Base;
	logMsg("entering background");
	appState = APP_PAUSED;
	Base::stopAnimation();
	callSafe(Base::onAppExitHandler, Base::onAppExitHandlerCtx, 1);
	#ifdef CONFIG_BASE_IOS_ICADE
	iCade.didEnterBackground();
	#endif
	glFinish();
}

- (void)timerCallback:(NSTimer*)timer
{
	using namespace Base;
	if(timerCallbackFunc)
	{
		logMsg("running callback");
		timerCallbackFunc(timerCallbackFuncCtx);
		timerCallbackFunc = 0;
	}
}

- (void)dealloc
{
	[Base::window release];
	//[glView release]; // retained in window
	[super dealloc];
}

#if defined(CONFIG_INPUT) && defined(IPHONE_VKEYBOARD)

/*- (BOOL) textFieldShouldReturn:(UITextField *)textField
{
	[textField resignFirstResponder];
	return YES;
}

- (void)textFieldDidEndEditing:(UITextField *)textField
{
	[vkbdField removeFromSuperview];
	NSString *text = vkbdField.text;
	//logMsg("calling text callback");
	vKeyboardTextCallback(vKeyboardTextCallbackUserPtr, [text cStringUsingEncoding: NSASCIIStringEncoding]);
	vkbdField.text = @"";
	inVKeyboard = 0;
}*/

#endif

@end

namespace Base
{

void ios_setVideoInterval(uint interval)
{
	logMsg("setting frame interval %d", (int)interval);
	assert(interval >= 1);
	[Base::displayLink setFrameInterval:interval];
}

void statusBarHidden(uint hidden)
{
	[[UIApplication sharedApplication] setStatusBarHidden: hidden ? YES : NO animated:YES];
}

void statusBarOrientation(uint o)
{
	switch(o)
	{
		bcase Gfx::VIEW_ROTATE_0:
			[[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationPortrait animated:NO];
		bcase Gfx::VIEW_ROTATE_270:
			[[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationLandscapeLeft animated:NO];
		bcase Gfx::VIEW_ROTATE_90:
			[[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationLandscapeRight animated:NO];
		bcase Gfx::VIEW_ROTATE_180:
			[[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationPortraitUpsideDown animated:NO];
	}
}

static bool autoOrientationState = 0; // Turned on in applicationDidFinishLaunching

void setAutoOrientation(bool on)
{
	if(autoOrientationState == on)
		return;
	autoOrientationState = on;
	if(on)
		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	else
	{
		Gfx::preferedOrientation = Gfx::rotateView;
		[[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
	}
}

void exitVal(int returnVal)
{
	appState = APP_EXITING;
	callSafe(onAppExitHandler, onAppExitHandlerCtx, 0);
	::exit(returnVal);
}
void abort() { ::abort(); }

void displayNeedsUpdate()
{
	generic_displayNeedsUpdate();
	if(appState == APP_RUNNING && Base::displayLinkActive == NO)
	{
		Base::startAnimation();
	}
}

void openURL(const char *url)
{
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:
		[NSString stringWithCString:url encoding:NSASCIIStringEncoding]]];
}

static const char *docPath = 0;

const char *documentsPath()
{
	if(!docPath)
	{
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        docPath = strdup([documentsDirectory cStringUsingEncoding: NSASCIIStringEncoding]);
    }
	return docPath;
}

int runningDeviceType()
{
	return isIPad ? DEV_TYPE_IPAD : DEV_TYPE_GENERIC;
}
}

int main(int argc, char *argv[])
{
	using namespace Base;
	
	doOrExit(logger_init());

	#ifdef CONFIG_FS
	Fs::changeToAppDir(argv[0]);
	#endif

	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	int retVal = UIApplicationMain(argc, argv, nil, @"MainApp");
	[pool release];
	return retVal;
}

#undef thisModuleName
