#pragma once

#ifdef CONFIG_INPUT
	#include <input/interface.h>
#endif

#ifdef SUPPORT_ANDROID_DIRECT_TEXTURE
	#include <dlfcn.h>
	#include <base/android/libhardwarePrivate.h>
	#include <base/android/sdk.hh>
	#if CONFIG_ENV_ANDROID_MINSDK < 9
		#include <base/android/EGLPrivate.h>
		static EGLDisplay eglDisplay = 0;
		static EGLImageKHR (*eglCreateImageKHR)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list) = 0;
		static EGLBoolean (*eglDestroyImageKHR)(EGLDisplay dpy, EGLImageKHR image) = 0;
		static EGLDisplay (*eglGetDisplay)(EGLNativeDisplayType display_id);
		namespace Base
		{
			static EGLDisplay getAndroidEGLDisplay() { return eglDisplay; }
		}
	#else
		#include <EGL/egl.h>
		#define EGL_EGLEXT_PROTOTYPES
		#include <EGL/eglext.h>
		namespace Base
		{
			EGLDisplay getAndroidEGLDisplay();
		}
	#endif
#endif

#ifdef CONFIG_BASE_ANDROID
	bool glSyncHackEnabled = 0, glSyncHackBlacklisted = 0;
#endif

//static int usingFixedAspectRatio = 0;
//static float aspectRatio = 4.0/3.0;

namespace Gfx
{
TextureSizeSupport textureSizeSupport =
{
	0, // nonPow2
	1, // nonSquare
	1, // filtering
	0, 0, // minXSize, minYSize
	0, 0 // maxXSize, maxYSize
};

static float zRange = 1000.0;

static void resizeGLScene(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);

	if(height == 0 || width == 0)
		return; // view is invisible, do nothing

	glcMatrixMode(GL_PROJECTION);

	//fovy = 45.0;
	GC fovy = M_PI/4.0;

	bool isSideways = rotateView == VIEW_ROTATE_90 || rotateView == VIEW_ROTATE_270;
	Gfx::proj.aspectRatio = isSideways ? (GC)height / (GC)width : (GC)width / (GC)height;
	//glLoadIdentity();
	//gluPerspective(45.0f, viewAspectRatio, 0.1f, 1000.0f);
	Matrix4x4<GC> mat, rMat;
	Gfx::proj.focal = 1.0;
	mat.perspectiveFovLH(fovy, Gfx::proj.aspectRatio, 1.0, zRange);
	/*Gfx::proj.focal = -100;
	mat.perspectiveFrustumWithView(isSideways ? height : width, isSideways ? width : height,
			1, 200, -Gfx::proj.focal);*/
	viewPixelWidth_ = width;
	viewPixelHeight_ = height;
	/*if(isSideways)
	{
		IG::swap(viewPixelWidth, viewPixelHeight);
	}*/
	Gfx::proj.setMatrix(mat, isSideways);
	setupScreenSize();
	if(animateOrientationChange)
	{
		logMsg("animated rotation %f", (double)IG::toDegrees(projAngleM.now));
		rMat.zRotationLH(projAngleM.now);
		mat = Matrix4x4<GC>::mult(mat, rMat);
	}
	else if(rotateView != VIEW_ROTATE_0)
	{
		logMsg("fixed rotation %f", (double)orientationToGC(rotateView));
		rMat.zRotationLH(IG::toRadians(orientationToGC(rotateView)));
		mat = Matrix4x4<GC>::mult(mat, rMat);
	}
	else
	{
		logMsg("no rotation");
	}
	glLoadMatrixf((GLfloat *)&mat.v[0]);
	//mat.print();
	/*glGetFloatv (GL_PROJECTION_MATRIX, (GLfloat*)&proj);
	logMsg("projection matrix set with fovy %f aspect ratio %f and z range %f - %f", (float)Gfx::proj.fovy, (float)Gfx::proj.aspectRatio, 1.0, (float)zRange);
	logMsg("view space half %f x %f, inverse %f x %f", 1.0f/proj._11, 1.0f/proj._22, proj._11, proj._22);*/
	//gfx_setupSpace(proj._11, proj._22, width, height);
	/*{
		GC x = 0,y = 0,z = Gfx::proj.focal;
		Gfx::proj.project(x,y,z);
		logMsg("projected to %f %f %f",x,y,z);
		x = 0;y = 0;z = 0.5;
		Gfx::proj.unProject(x,y,z);
		logMsg("unprojected to %f %f %f",x,y,z);
	}*/
}

void resizeDisplay(uint x, uint y)
{
	Gfx::GfxViewState oldState =
	{
		proj.w, proj.h, proj.aspectRatio,
		viewPixelWidth_, viewPixelHeight_
	};
	logMsg("resizing viewport to %dx%d", x, y);
	resizeGLScene(x, y);

	//logMsg("calling view space callback %p", viewSpaceCallback.func);
	callSafe(viewChangeHandler, viewChangeHandlerCtx, &oldState);
}

#ifdef CONFIG_GFX_SOFT_ORIENTATION

#ifdef CONFIG_INPUT
void configureInputForOrientation()
{
	input_xPointerTransform(rotateView == VIEW_ROTATE_0 || rotateView == VIEW_ROTATE_90 ? INPUT_POINTER_NORMAL : INPUT_POINTER_INVERT);
	input_yPointerTransform(rotateView == VIEW_ROTATE_0 || rotateView == VIEW_ROTATE_270 ? INPUT_POINTER_NORMAL : INPUT_POINTER_INVERT);
	input_pointerAxis(rotateView == VIEW_ROTATE_0 || rotateView == VIEW_ROTATE_180 ? INPUT_POINTER_NORMAL : INPUT_POINTER_INVERT);
}
#endif

uint setOrientation(uint o)
{
	assert(o == VIEW_ROTATE_0 || o == VIEW_ROTATE_90 || o == VIEW_ROTATE_180 || o == VIEW_ROTATE_270);

	if((validOrientations & o) && rotateView != o)
	{
		rotateView = o;
		if(animateOrientationChange)
		{
			projAngleM.initLinear(projAngleM.now, IG::toRadians(orientationToGC(rotateView)), 10);
			Base::displayNeedsUpdate();
		}
		resizeDisplay(viewPixelWidth_, viewPixelHeight_);
		#ifdef CONFIG_INPUT
			configureInputForOrientation();
		#endif
		Base::statusBarOrientation(o);
		return 1;
	}
	else
		return 0;
}
#endif

}

static void vsyncEnable()
{
	#ifdef CONFIG_BASE_WIN32
		#define WGL_VSYNC_ON_INTERVAL 1
		if (wglewIsSupported("WGL_EXT_swap_control"))
		{
			wglSwapIntervalEXT(WGL_VSYNC_ON_INTERVAL);
			logMsg("vsync enabled via WGL_EXT_swap_control");
		}
		else
		{
			logWarn("WGL_EXT_swap_control is not supported");
		}
	#elif defined(__APPLE__) && !defined(CONFIG_BASE_IOS)
		GLint sync = 1;
		CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &sync);
	#endif
}

static GLfloat anisotropy = 0, forceAnisotropy = 0;
static bool useAnisotropicFiltering = 0;

static void checkForAnisotropicFiltering()
{
	#ifndef CONFIG_GFX_OPENGL_ES
	if(!forceNoAnisotropicFiltering && GLEW_EXT_texture_filter_anisotropic)
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnisotropy);
		logMsg("anisotropic filtering supported, max value: %f", maximumAnisotropy);
		useAnisotropicFiltering = 1;

		if(forceAnisotropy) // force a specific anisotropy value
			anisotropy = forceAnisotropy;
		else anisotropy = maximumAnisotropy;
		assert(anisotropy <= maximumAnisotropy);
	}
	#endif
}

static uchar useAutoMipmapGeneration = 0;

static void checkForAutoMipmapGeneration()
{
	#ifndef CONFIG_GFX_OPENGL_ES
	if(!forceNoAutoMipmapGeneration && GLEW_SGIS_generate_mipmap)
	{
		logMsg("automatic mipmap generation supported");
		useAutoMipmapGeneration = 1;

		//glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
	}
	#else
		useAutoMipmapGeneration = 0;
	#endif
}

static uchar useMultisample = 0;
static uchar forceNoMultisample = 1;
static uchar forceNoMultisampleHint = 0;

static void checkForMultisample()
{
	#ifndef CONFIG_GFX_OPENGL_ES
	if(!forceNoMultisample && GLEW_ARB_multisample)
	{
		logMsg("multisample antialiasing supported");
		useMultisample = 1;
		if(!forceNoMultisampleHint && GLEW_NV_multisample_filter_hint)
		{
			logMsg("multisample hints supported");
			glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
		}
		glcEnable(GL_MULTISAMPLE_ARB);
	}
	#endif
}

static void checkForVertexArrays()
{
	#ifndef CONFIG_GFX_OPENGL_ES
	if(!GLEW_VERSION_1_1)
	{
		logErr("OpenGL 1.1 vertex arrays not supported");
		Base::exit();
	}
	#endif
}

static uchar forceNoNonPow2Textures = 0;

static void checkForNonPow2Textures()
{
	if(forceNoNonPow2Textures)
		return;

	#if defined(CONFIG_BASE_PS3)
	if(1)
	#elif defined(CONFIG_GFX_OPENGL_ES) && defined(CONFIG_BASE_IOS)
	if(strstr(extensions, "GL_APPLE_texture_2D_limited_npot"))
	#elif defined(CONFIG_GFX_OPENGL_ES)
	if(strstr(extensions, "GL_IMG_texture_npot") || strstr(extensions, "GL_NV_texture_npot_2D_mipmap") || strstr(extensions, "GL_APPLE_texture_2D_limited_npot"))
	#else
	if(GLEW_ARB_texture_non_power_of_two)
	#endif
	{
		Gfx::textureSizeSupport.nonPow2 = 1;
		logMsg("Non-Power-of-2 textures are supported");
	}
}

static uchar supportBGRPixels = 0;
static uchar useBGRPixels = 0;

namespace Gfx
{
bool preferBGRA = 0, preferBGR = 0;

#if defined(CONFIG_BASE_ANDROID)
	// Android is missing GL_BGRA in ES 1 headers
	#define GL_BGRA 0x80E1
#elif defined(CONFIG_GFX_OPENGL_ES) && defined(CONFIG_BASE_X11)
	// Mesa uses GL_BGRA_EXT
	#define GL_BGRA GL_BGRA_EXT
#endif

static void checkForBGRPixelSupport()
{
	#ifdef CONFIG_GFX_OPENGL_ES
		#ifdef CONFIG_BASE_IOS
			if(strstr(extensions, "GL_APPLE_texture_format_BGRA8888") != NULL)
		#elif defined(CONFIG_BASE_PS3)
			if(0)
		#else
			if(strstr(extensions, "GL_EXT_texture_format_BGRA8888") != NULL)
		#endif
	#else
		if(!forceNoBGRPixels && GLEW_VERSION_1_2)
	#endif
	{
		supportBGRPixels = 1;
		useBGRPixels = 1;

		#if !defined(CONFIG_GFX_OPENGL_ES) || defined(CONFIG_BASE_PS3)
		preferBGR = 1;
		#endif
		preferBGRA = 1;

		logMsg("BGR pixel types are supported");
	}
}

}

static uchar useTextureClampToEdge = 0;

static void checkForTextureClampToEdge()
{
	#ifndef CONFIG_GFX_OPENGL_ES
	if(!forceNoTextureClampToEdge && GLEW_VERSION_1_2)
	{
		useTextureClampToEdge = 1;
		logMsg("Texture clamp to edge mode supported");
	}
	#else
	useTextureClampToEdge = 1;
	#endif
}

static void checkForCompressedTexturesSupport()
{
	#ifndef CONFIG_GFX_OPENGL_ES
	if(!forceNoCompressedTextures && GLEW_VERSION_1_3)
	{
		supportCompressedTextures = 1;
		useCompressedTextures = 1;
		logMsg("Compressed textures are supported");
	}
	#endif
}

static uchar useFBOFuncs = 0;

static void checkForFBOFuncs()
{
	#ifndef CONFIG_GFX_OPENGL_ES
	if(!forceNoFBOFuncs && GLEW_EXT_framebuffer_object)
	{
		useFBOFuncs = 1;
		logMsg("FBO functions are supported");
	}
	#endif
}

static uchar useVBOFuncs = 0;
static uchar forceNoVBOFuncs = 1;

static void checkForVBO()
{
	if(!forceNoVBOFuncs &&
	#ifndef CONFIG_GFX_OPENGL_ES
		GLEW_VERSION_1_5
	#else
		strstr(version, " 1.0") == NULL // make sure OpenGL-ES is not 1.0
	#endif
	)
	{
		useVBOFuncs = 1;
		logMsg("VBOs are supported");
	}
}

#if defined(CONFIG_GFX_OPENGL_ES) && !defined(CONFIG_BASE_PS3)

static bool useDrawTex = 0;
static bool forceNoDrawTex = 0;

static void checkForDrawTexture()
{
	// Limited usefulness due to no 90deg rotation support
	if(!forceNoDrawTex && strstr(extensions, "GL_OES_draw_texture") != 0)
	{
		useDrawTex = 1;
		logMsg("Draw Texture supported");
	}
}

#endif

#ifdef SUPPORT_ANDROID_DIRECT_TEXTURE
static bool useEGLImageKHR = 0, supportEGLImageKHR = 0, whitelistedEGLImageKHR = 0;
static const EGLint eglImgAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE, EGL_NONE };
static GLenum directTextureTarget = GL_TEXTURE_2D;

static int (*hw_get_module)(const char *id, const struct hw_module_t **module) = 0;

static gralloc_module_t const *grallocMod = 0;
static alloc_device_t *allocDev = 0;

static android_native_buffer_t eglBuf;
static EGLImageKHR eglImg;

static void dummyIncRef(struct android_native_base_t* base)
{
	logMsg("called incRef");
}

static void dummyDecRef(struct android_native_base_t* base)
{
	logMsg("called decRef");
}

static void setupAndroidNativeBuffer(android_native_buffer_t &eglBuf, int x, int y, int format, int usage)
{
	mem_zero(eglBuf);
	eglBuf.common.magic = ANDROID_NATIVE_BUFFER_MAGIC;
	eglBuf.common.version = sizeof(android_native_buffer_t);
	//memset(eglBuf.common.reserved, 0, sizeof(eglBuf.common.reserved));
	eglBuf.common.incRef = dummyIncRef;
	eglBuf.common.decRef = dummyDecRef;
	//memset(eglBuf.reserved, 0, sizeof(eglBuf.reserved));
	//memset(eglBuf.reserved_proc, 0, sizeof(eglBuf.reserved_proc));
	eglBuf.width = x;
	eglBuf.height = y;
	//eglBuf.stride = 0;
	eglBuf.format = format;
	//eglBuf.handle = 0;
	eglBuf.usage = usage;
}

static int testEGLImageKHR(int x = 256, int y = 256)
{
	uint ref = 0;
	int ret = ANDROID_DT_SUCCESS
			, glErr;
	EGLImageKHR eglImg = 0;
	var_copy(eglDisplay, Base::getAndroidEGLDisplay());
	glGenTextures(1, &ref);
	glcBindTexture(GL_TEXTURE_2D, ref);

	setupAndroidNativeBuffer(eglBuf, x, y, HAL_PIXEL_FORMAT_RGB_565,
			GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_HW_TEXTURE);
	if(allocDev->alloc(allocDev, eglBuf.width, eglBuf.height, eglBuf.format,
			eglBuf.usage, &eglBuf.handle, &eglBuf.stride) != 0)
	{
		logMsg("error in alloc buffer");
		ret = ANDROID_DT_ERR_TEST_ALLOC; goto CLEANUP;
	}

	void *data;
	if(grallocMod->lock(grallocMod, eglBuf.handle, GRALLOC_USAGE_SW_WRITE_OFTEN, 0, 0, x, y, &data) != 0)
	{
		logMsg("error locking");
		ret = ANDROID_DT_ERR_TEST_LOCK; goto CLEANUP;
	}

	if(grallocMod->unlock(grallocMod, eglBuf.handle) != 0)
	{
		logMsg("error unlocking");
		ret = ANDROID_DT_ERR_TEST_UNLOCK; goto CLEANUP;
	}

	eglImg = eglCreateImageKHR(eglDisplay, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, (EGLClientBuffer)&eglBuf, eglImgAttrs);
	if(eglImg == EGL_NO_IMAGE_KHR)
	{
		logMsg("error creating EGL image");
		ret = ANDROID_DT_ERR_TEST_IMG_CREATE; goto CLEANUP;
	}

	while (glGetError() != GL_NO_ERROR) { }
	glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, (GLeglImageOES)eglImg);
	glErr = glGetError();
	if(glErr != GL_NO_ERROR)
	{
		logErr("%s in glEGLImageTargetTexture2DOES", glErrorToString(glErr));
		ret = ANDROID_DT_ERR_TEST_TARGET_TEXTURE; goto CLEANUP;
	}

	logMsg("all tests passed");

	CLEANUP:

	if(eglImg != EGL_NO_IMAGE_KHR)
	{
		logMsg("calling eglDestroyImageKHR");
		eglDestroyImageKHR(eglDisplay, eglImg);
	}
	if(eglBuf.handle)
	{
		if(allocDev->free)
		{
			logMsg("calling free %p on handle %p", allocDev->free, eglBuf.handle);
			allocDev->free(allocDev, eglBuf.handle);
		}
		else
			logMsg("no free function for handle");
	}
	glcDeleteTextures(1, &ref);
	return ret;
}

static int enableEGLImageKHR()
{
	logMsg("attempting to setup EGLImageKHR support");

	int ret;

	if(strstr(extensions, "GL_OES_EGL_image") == 0)
	{
		logWarn("no GL_OES_EGL_image");
		return ANDROID_DT_ERR_NO_EGL_IMG_EXT;
	}

	/*if(strstr(extensions, "GL_OES_EGL_image_external") || strstr(rendererName, "Adreno"))
	{
		logMsg("uses GL_OES_EGL_image_external");
		directTextureTarget = GL_TEXTURE_EXTERNAL_OES;
	}*/


	void *libegl = 0, *libhardware = 0;

	#if CONFIG_ENV_ANDROID_MINSDK < 9
	if((libegl = dlopen("/system/lib/libEGL.so", RTLD_LOCAL | RTLD_LAZY)) == 0)
	{
		logWarn("unable to dlopen libEGL.so");
		ret = ANDROID_DT_ERR_NO_LIBEGL; goto FAIL;
	}

	//char const *(*eglQueryString)(EGLDisplay, EGLint) = (char const *(*)(EGLDisplay, EGLint))dlsym(libegl, "eglQueryString");
	//logMsg("EGL Extensions: %s", eglQueryString((EGLDisplay)1, EGL_EXTENSIONS));

	//logMsg("eglCreateImageKHR @ %p", eglCreateImageKHR);
	if((eglCreateImageKHR = (EGLImageKHR(*)(EGLDisplay, EGLContext, EGLenum, EGLClientBuffer, const EGLint *))dlsym(libegl, "eglCreateImageKHR"))
		== 0)
	{
		logWarn("eglCreateImageKHR not found");
		ret = ANDROID_DT_ERR_NO_CREATE_IMG; goto FAIL;
	}

	//logMsg("eglDestroyImageKHR @ %p", eglCreateImageKHR);
	if((eglDestroyImageKHR = (EGLBoolean(*)(EGLDisplay, EGLImageKHR))dlsym(libegl, "eglDestroyImageKHR")) == 0)
	{
		logWarn("eglDestroyImageKHR not found");
		ret = ANDROID_DT_ERR_NO_DESTROY_IMG; goto FAIL;
	}
	/*eglGetCurrentDisplay = (EGLDisplay(*)())dlsym(libegl, "eglGetCurrentDisplay");
	logMsg("eglGetCurrentDisplay @ %p", eglGetCurrentDisplay);*/


	//logMsg("eglGetDisplay @ %p", eglGetDisplay);
	if((eglGetDisplay = (EGLDisplay(*)(EGLNativeDisplayType))dlsym(libegl, "eglGetDisplay")) == 0)
	{
		logWarn("eglGetDisplay not found");
		ret = ANDROID_DT_ERR_NO_GET_DISPLAY; goto FAIL;
	}


	if((eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
	{
		logWarn("failed to get EGL display");
		ret = ANDROID_DT_ERR_GET_DISPLAY; goto FAIL;
	}
	logMsg("got EGL display: %d", (int)eglDisplay);
	#endif

	if((libhardware = dlopen("/system/lib/libhardware.so", RTLD_LOCAL | RTLD_LAZY)) == 0)
	{
		logWarn("unable to dlopen libhardware.so");
		ret = ANDROID_DT_ERR_NO_LIBHARDWARE; goto FAIL;
	}

	//logMsg("hw_get_module @ %p", hw_get_module);
	if((hw_get_module = (int(*)(const char *, const struct hw_module_t **))dlsym(libhardware, "hw_get_module")) == 0)
	{
		logWarn("hw_get_module not found");
		ret = ANDROID_DT_ERR_NO_HW_GET_MODULE; goto FAIL;
	}

	if(hw_get_module(GRALLOC_HARDWARE_MODULE_ID, (hw_module_t const**)&grallocMod) != 0)
	{
		logWarn("error loading gralloc module");
		ret = ANDROID_DT_ERR_NO_GRALLOC_MODULE; goto FAIL;
	}

	gralloc_open((const hw_module_t*)grallocMod, &allocDev);
	if(!allocDev)
	{
		logWarn("error getting alloc device");
		ret = ANDROID_DT_ERR_NO_ALLOC_DEVICE; goto FAIL;
	}
	logMsg("alloc device @ %p", allocDev);

	ret = testEGLImageKHR();
	if(ret != ANDROID_DT_SUCCESS)
	{
		goto FAIL;
	}

	useEGLImageKHR = 1;
	logMsg("using EGLImageKHR");
	return ret;

	FAIL:
	if(libegl)
	{
		dlclose(libegl);
		libegl = 0;
	}
	if(libhardware)
	{
		dlclose(libhardware);
		libhardware = 0;
	}
	//TODO: free allocDev if needed

	return ret;
}

static int errorEGLImageKHR;
bool gfx_androidDirectTextureSupported() { return supportEGLImageKHR; }
bool gfx_androidDirectTextureSupportWhitelisted() { return whitelistedEGLImageKHR; }
int gfx_androidDirectTextureError() { return errorEGLImageKHR; }

static void checkForEGLImageKHR()
{
	if(strstr(rendererName, "SGX 530")) // enable on PowerVR SGX 530, though it should work on other models
	{
		logMsg("white-listed for EGLImageKHR");
		whitelistedEGLImageKHR = 1;
	}

	if(strstr(rendererName, "NVIDIA")) // disable on Tegra, unneeded and causes lock-ups currently
	{
		logMsg("force-disabling EGLImageKHR on Tegra");
		errorEGLImageKHR = ANDROID_DT_ERR_FORCE_DISABLE;
	}
	else
		errorEGLImageKHR = enableEGLImageKHR();
	supportEGLImageKHR = errorEGLImageKHR == ANDROID_DT_SUCCESS;
}

void gfx_setAndroidDirectTexture(bool on)
{
	if(supportEGLImageKHR)
		useEGLImageKHR = on;
}

#endif


static uchar usingMipmaping()
{
	if(useAutoMipmapGeneration
		#ifndef CONFIG_GFX_OPENGL_ES
		|| useFBOFuncs
		#endif
	  )
		return 1;
	else return 0;
}

#ifdef CONFIG_BASE_ANDROID

namespace Gfx
{

void setDither(uint on)
{
	if(on)
		glEnable(GL_DITHER);
	else
	{
		logMsg("disabling dithering");
		glDisable(GL_DITHER);
	}
}

}

#endif
