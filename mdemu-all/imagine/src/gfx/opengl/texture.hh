#pragma once

#include <gfx/GfxBufferImage.hh>

static uint setUnpackAlignForPitch(uint pitch)
{
	uint alignment = 1;
	if(!Config::envIsPS3 && pitch % 8 == 0) alignment = 8;
	else if(pitch % 4 == 0) alignment = 4;
	else if(pitch % 2 == 0) alignment = 2;
	//logMsg("setting unpack alignment %d", alignment);
	glcPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	return alignment;
}

#ifdef CONFIG_GFX_OPENGL_ES
#define GL_MIRRORED_REPEAT 0x8370
#endif

static int bestClampMode(bool textured)
{
	#ifndef CONFIG_GFX_OPENGL_ES
		return useTextureClampToEdge ? GL_CLAMP_TO_EDGE : GL_CLAMP;
	#else
		return GL_CLAMP_TO_EDGE;
		//return GL_MIRRORED_REPEAT;
	#endif
}

static GLenum pixelFormatToOGLDataType(const PixelFormatDesc &format)
{
	switch(format.id)
	{
		case PIXEL_RGBA8888:
		case PIXEL_ABGR8888:
		#if !defined(CONFIG_GFX_OPENGL_ES) || defined(CONFIG_BASE_PS3)
			return GL_UNSIGNED_INT_8_8_8_8;
		#endif
		case PIXEL_ARGB8888:
		case PIXEL_BGRA8888:
		#if !defined(CONFIG_GFX_OPENGL_ES) || defined(CONFIG_BASE_PS3)
			return GL_UNSIGNED_INT_8_8_8_8_REV;
		#endif
		case PIXEL_RGB888:
		case PIXEL_BGR888:
		case PIXEL_I8:
		case PIXEL_IA88:
			return GL_UNSIGNED_BYTE;
		case PIXEL_RGB565:
			return GL_UNSIGNED_SHORT_5_6_5;
		case PIXEL_ARGB1555:
			return GL_UNSIGNED_SHORT_5_5_5_1;
		case PIXEL_ARGB4444:
			return GL_UNSIGNED_SHORT_4_4_4_4;
		#if !defined(CONFIG_GFX_OPENGL_ES) || defined(CONFIG_BASE_PS3)
		case PIXEL_BGRA4444:
			return GL_UNSIGNED_SHORT_4_4_4_4_REV;
		case PIXEL_ABGR1555:
			return GL_UNSIGNED_SHORT_1_5_5_5_REV;
		#endif
		default: bug_branch("%d", format.id); return 0;
	}
}

static GLenum pixelFormatToOGLFormat(const PixelFormatDesc &format)
{
	#if defined(CONFIG_BASE_PS3)
		if(format.id == PIXEL_ARGB8888)
			return GL_BGRA;
	#endif
	if(format.isGrayscale())
	{
		if(format.aBits)
			return GL_LUMINANCE_ALPHA;
		else return GL_LUMINANCE;
	}
	#if !defined(CONFIG_GFX_OPENGL_ES) || defined(CONFIG_BASE_PS3)
	else if(format.isBGROrder())
	{
		assert(supportBGRPixels);
		if(format.aBits)
		{
			return GL_BGRA;
		}
		else return GL_BGR;
	}
	#else
	else if(format.isBGROrder() && format.aBits)
	{
		assert(supportBGRPixels);
		return GL_BGRA;
	}
	#endif
	else if(format.aBits)
	{
		return GL_RGBA;
	}
	else return GL_RGB;
}

static int pixelToOGLInternalFormat(const PixelFormatDesc &format)
{
	#if defined(CONFIG_GFX_OPENGL_ES) && !defined(CONFIG_BASE_PS3)
		#ifdef CONFIG_BASE_IOS
			if(format.id == PIXEL_BGRA8888) // Apple's BGRA extension loosens the internalformat match requirement
				return GL_RGBA;
		#endif
		return pixelFormatToOGLFormat(format); // OpenGL ES manual states internalformat always equals format
	#else

	#if !defined(CONFIG_BASE_PS3)
	if(useCompressedTextures)
	{
		switch(format.id)
		{
			case PIXEL_RGBA8888:
			case PIXEL_BGRA8888:
				return GL_COMPRESSED_RGBA;
			case PIXEL_RGB888:
			case PIXEL_BGR888:
			case PIXEL_RGB565:
			case PIXEL_ARGB1555:
			case PIXEL_ARGB4444:
			case PIXEL_BGRA4444:
				return GL_COMPRESSED_RGB;
			case PIXEL_I8:
				return GL_COMPRESSED_LUMINANCE;
			case PIXEL_IA88:
				return GL_COMPRESSED_LUMINANCE_ALPHA;
			default: bug_branch("%d", format.id); return 0;
		}
	}
	else
	#endif
	{
		switch(format.id)
		{
			case PIXEL_BGRA8888:
			#if defined(CONFIG_BASE_PS3)
				return GL_BGRA;
			#endif
			case PIXEL_ARGB8888:
			case PIXEL_ABGR8888:
			#if defined(CONFIG_BASE_PS3)
				return GL_ARGB_SCE;
			#endif
			case PIXEL_RGBA8888:
				return GL_RGBA8;
			case PIXEL_RGB888:
			case PIXEL_BGR888:
				return GL_RGB8;
			case PIXEL_RGB565:
				return GL_RGB5;
			case PIXEL_ABGR1555:
			case PIXEL_ARGB1555:
				return GL_RGB5_A1;
			case PIXEL_ARGB4444:
			case PIXEL_BGRA4444:
				return GL_RGBA4;
			case PIXEL_I8:
				return GL_LUMINANCE8;
			case PIXEL_IA88:
				return GL_LUMINANCE8_ALPHA8;
			default: bug_branch("%d", format.id); return 0;
		}
	}

	#endif
}

static const char *glImageFormatToString(int format)
{
	switch(format)
	{
		#if defined(CONFIG_BASE_PS3)
		case GL_ARGB_SCE: return "ARGB_SCE";
		#endif
		#if !defined(CONFIG_GFX_OPENGL_ES) || defined(CONFIG_BASE_PS3)
		case GL_RGBA8: return "RGBA8";
		case GL_RGB8: return "RGB8";
		case GL_RGB5_A1: return "RGB5_A1";
		case GL_RGB5: return "RGB5";
		case GL_RGBA4: return "RGBA4";
		case GL_BGR: return "BGR";
		case GL_LUMINANCE8: return "LUMINANCE8";
		case GL_LUMINANCE8_ALPHA8: return "LUMINANCE8_ALPHA8";
		#endif
		#if !defined(CONFIG_GFX_OPENGL_ES)
		case GL_COMPRESSED_RGBA: return "COMPRESSED_RGBA";
		case GL_COMPRESSED_RGB: return "COMPRESSED_RGB";
		case GL_COMPRESSED_LUMINANCE: return "COMPRESSED_LUMINANCE";
		case GL_COMPRESSED_LUMINANCE_ALPHA: return "COMPRESSED_LUMINANCE_ALPHA";
		#endif
		case GL_RGBA: return "RGBA";
		//#if defined(CONFIG_BASE_PS3) || defined(CONFIG_ENV_WEBOS) || !defined(CONFIG_GFX_OPENGL_ES)
		case GL_BGRA: return "BGRA";
		/*#else
		case GL_BGRA_EXT: return "BGRA";
		#endif*/
		case GL_RGB: return "RGB";
		case GL_LUMINANCE: return "LUMINANCE";
		case GL_LUMINANCE_ALPHA: return "LUMINANCE_ALPHA";
		default: bug_branch("%d", format); return NULL;
	}
}

static const char *glDataTypeToString(int format)
{
	switch(format)
	{
		case GL_UNSIGNED_BYTE: return "B";
		#if !defined(CONFIG_GFX_OPENGL_ES) || defined(CONFIG_BASE_PS3)
		case GL_UNSIGNED_INT_8_8_8_8: return "I8888";
		case GL_UNSIGNED_INT_8_8_8_8_REV: return "I8888R";
		case GL_UNSIGNED_SHORT_1_5_5_5_REV: return "S1555";
		case GL_UNSIGNED_SHORT_4_4_4_4_REV: return "S4444R";
		#endif
		case GL_UNSIGNED_SHORT_5_6_5: return "S565";
		case GL_UNSIGNED_SHORT_5_5_5_1: return "S5551";
		case GL_UNSIGNED_SHORT_4_4_4_4: return "S4444";
		default: bug_branch("%d", format); return NULL;
	}
}

enum { MIPMAP_NONE, MIPMAP_LINEAR, MIPMAP_NEAREST };
static GLint openGLFilterType(uint imgFilter, uchar mipmapType)
{
	if(imgFilter == GfxBufferImage::nearest)
	{
		return mipmapType == MIPMAP_NEAREST ? GL_NEAREST_MIPMAP_NEAREST :
			mipmapType == MIPMAP_LINEAR ? GL_NEAREST_MIPMAP_LINEAR :
			GL_NEAREST;
	}
	else
	{
		return mipmapType == MIPMAP_NEAREST ? GL_LINEAR_MIPMAP_NEAREST :
			mipmapType == MIPMAP_LINEAR ? GL_LINEAR_MIPMAP_LINEAR :
			GL_LINEAR;
	}
}

static void setDefaultImageTextureParams(uint imgFilter, uchar mipmapType, int xWrapType, int yWrapType, uint usedX, uint usedY)
{
	//mipmapType = MIPMAP_NONE;
	GLint filter = openGLFilterType(imgFilter, mipmapType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, xWrapType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, yWrapType);
	if(filter != GL_LINEAR) // GL_LINEAR is the default
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	#ifndef CONFIG_ENV_WEBOS
	if(useAnisotropicFiltering)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	#endif
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//float col[] = {1, 0.5, 0.5, 1};
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, col);

	#if defined(CONFIG_GFX_OPENGL_ES) && !defined(CONFIG_BASE_PS3)
	if(useDrawTex)
	{
		GLint coords [] = {0, 0, usedX, usedY};
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, coords);
	}
	#endif
}

static uint writeGLTexture(Pixmap *pix, bool includePadding)
{
	//logMsg("writeGLTexture");
	uint alignment = setUnpackAlignForPitch(pix->pitch);
	assert((ptrsize)pix->data % (ptrsize)alignment == 0);
	GLenum format = pixelFormatToOGLFormat(*pix->format);
	GLenum dataType = pixelFormatToOGLDataType(*pix->format);
	uint xSize = includePadding ? pix->pitchPixels() : pix->x;
	#ifndef CONFIG_GFX_OPENGL_ES
		glcPixelStorei(GL_UNPACK_ROW_LENGTH, (!includePadding && pix->isPadded()) ? pix->pitchPixels() : 0);
		//logMsg("writing %s %dx%d to %dx%d, xline %d", glImageFormatToString(format), 0, 0, pix->x, pix->y, pix->pitch / pix->format->bytesPerPixel);
		clearGLError();
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0,
				xSize, pix->y, format, dataType, pix->data);
		glErrorCase(err)
		{
			logErr("%s in glTexSubImage2D", glErrorToString(err));
			return 0;
		}
	#else
		clearGLError();
		if(includePadding || pix->pitch == pix->x * pix->format->bytesPerPixel)
		{
			//logMsg("pitch equals x size optimized case");
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
					xSize, pix->y, format, dataType, pix->data);
			glErrorCase(err)
			{
				logErr("%s in glTexSubImage2D", glErrorToString(err));
				return 0;
			}
		}
		else
		{
			logWarn("OGL ES slow glTexSubImage2D case");
			uchar *row = pix->data;
			for(int y = 0; y < (int)pix->y; y++)
			{
				glTexSubImage2D( GL_TEXTURE_2D, 0, 0, y,
						pix->x, 1, format, dataType, row);
				glErrorCase(err)
				{
					logErr("%s in glTexSubImage2D, line %d", glErrorToString(err), y);
					return 0;
				}
				row += pix->pitch;
			}
		}
	#endif

	return 1;
}

static uint replaceGLTexture(Pixmap *pix, bool upload, uint internalFormat, bool includePadding)
{
	/*#ifdef CONFIG_GFX_OPENGL_ES // pal tex test
	if(pix->format->id == PIXEL_IA88)
	{
		logMsg("testing pal tex");
		return 1;
	}
	#endif*/

	uint alignment = setUnpackAlignForPitch(pix->pitch);
	assert((ptrsize)pix->data % (ptrsize)alignment == 0);
	#ifndef CONFIG_GFX_OPENGL_ES
		glcPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	#endif
	GLenum format = pixelFormatToOGLFormat(*pix->format);
	GLenum dataType = pixelFormatToOGLDataType(*pix->format);
	uint xSize = includePadding ? pix->pitchPixels() : pix->x;
	if(includePadding && pix->pitchPixels() != pix->x)
		logMsg("including padding in texture size, %d", pix->pitchPixels());
	clearGLError();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, xSize, pix->y,
				0, format, dataType, upload ? pix->data : 0);
	glErrorCase(err)
	{
		logErr("%s in glTexImage2D", glErrorToString(err));
		return 0;
	}
	return 1;
}

#ifdef SUPPORT_ANDROID_DIRECT_TEXTURE

static Pixmap eglPixmap;

static int pixelFormatToDirectAndroidFormat(const PixelFormatDesc &format)
{
	switch(format.id)
	{
		case PIXEL_RGBA8888: return HAL_PIXEL_FORMAT_RGBA_8888;
		case PIXEL_BGRA8888: return HAL_PIXEL_FORMAT_BGRA_8888;
		//case PIXEL_RGB888: return HAL_PIXEL_FORMAT_RGB_888;
		case PIXEL_RGB565: return HAL_PIXEL_FORMAT_RGB_565;
		case PIXEL_ARGB1555: return HAL_PIXEL_FORMAT_RGBA_5551;
		case PIXEL_ARGB4444: return HAL_PIXEL_FORMAT_RGBA_4444;
		//case PIXEL_I8: return GGL_PIXEL_FORMAT_L_8;
		//case PIXEL_IA88: return GGL_PIXEL_FORMAT_LA_88;
		default: return GGL_PIXEL_FORMAT_NONE;
	}
}

/*#include <unistd.h>

static void printPrivHnd(buffer_handle_t handle)
{
	private_handle_t* pHnd = (private_handle_t*)handle;
	logMsg("app pid %d version %d numFds %d numInts %d", (int)getpid(), pHnd->version, pHnd->numFds, pHnd->numInts);
	logMsg("fd %d magic %d flags %x size %d offset %d gpu_fd %d base %d lockState %x writeOwner %d gpuaddr %d pid %d",
			pHnd->fd, pHnd->magic, pHnd->flags, pHnd->size, pHnd->offset, pHnd->gpu_fd, pHnd->base, pHnd->lockState, pHnd->writeOwner, pHnd->gpuaddr, pHnd->pid);

}*/

static int eglImgCount = 0;

static bool setupEGLImage(Pixmap *pix, uint texRef, uint usedX, uint usedY)
{
	assert(eglImgCount == 0);

	int androidFormat = pixelFormatToDirectAndroidFormat(*pix->format);
	if(androidFormat == GGL_PIXEL_FORMAT_NONE)
	{
		logMsg("format cannot be used");
		return 0;
	}

	setupAndroidNativeBuffer(eglBuf, usedX, usedY, androidFormat,
		/*GRALLOC_USAGE_SW_READ_OFTEN |*/ GRALLOC_USAGE_SW_WRITE_OFTEN
		/*| GRALLOC_USAGE_HW_RENDER*/ | GRALLOC_USAGE_HW_TEXTURE);
	if(allocDev->alloc(allocDev, eglBuf.width, eglBuf.height, eglBuf.format,
		eglBuf.usage, &eglBuf.handle, &eglBuf.stride) != 0)
	{
		logMsg("error in alloc buffer");
		return 0;
	}
	logMsg("got buffer %p with stride %d", eglBuf.handle, eglBuf.stride);
	//printPrivHnd(eglBuf.handle);

	if(grallocMod->registerBuffer(grallocMod, eglBuf.handle) != 0)
	{
		logMsg("error registering");
	}
	logMsg("testing locking");
	void *data;
	if(grallocMod->lock(grallocMod, eglBuf.handle, GRALLOC_USAGE_SW_WRITE_OFTEN, 0, 0, usedX, usedY, &data) != 0)
	{
		logMsg("error locking");
		return 0;
	}
	logMsg("data addr %p", data);
	//printPrivHnd(eglBuf.handle);
	//memset(data, 0, eglBuf.stride * eglBuf.height * 2);
	logMsg("unlocking");
	if(grallocMod->unlock(grallocMod, eglBuf.handle) != 0)
	{
		logMsg("error unlocking");
		return 0;
	}
	//printPrivHnd(eglBuf.handle);

	//eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglImg = eglCreateImageKHR(Base::getAndroidEGLDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, (EGLClientBuffer)&eglBuf, eglImgAttrs);
	if(eglImg == EGL_NO_IMAGE_KHR)
	{
		logMsg("error creating EGL image");
		return 0;
	}

	clearGLError();
	glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, (GLeglImageOES)eglImg);
	glErrorCase(err)
	{
		logErr("%s in glEGLImageTargetTexture2DOES", glErrorToString(err));
		return 0;
	}

	eglPixmap.init(0, pix->format, usedX, usedY, 0);
	eglPixmap.pitch = eglBuf.stride * eglPixmap.format->bytesPerPixel;

	eglImgCount++;
	return 1;
}

#endif

static uint createGLTexture(Pixmap *pix, bool upload, uint internalFormat, int xWrapType, int yWrapType, uint usedX, uint usedY, uint hints, GfxBufferImage &bufImg, uint filter)
{
	//logMsg("createGLTexture");
	int newTexture = 0;
	uint texRef = texReuseCache_getExistingUsable(&texCache, pix->x, pix->y, internalFormat);
	if(!texRef)
	{
		//logMsg("no texture match found in cache for %d,%d %s", pix->x, pix->y, glImageFormatToString(internalFormat));
		texRef = texReuseCache_getNew(&texCache, pix->x, pix->y, internalFormat);
		if(texRef == 0)
			return(0);
		newTexture = 1;
	}

	//logMsg("binding texture %d", texRef);
	glcBindTexture(GL_TEXTURE_2D, texRef);
	setDefaultImageTextureParams(filter, usingMipmaping() ? MIPMAP_LINEAR : MIPMAP_NONE, xWrapType, yWrapType, usedX, usedY);

	bool includePadding = 0; //include extra bytes when x != pitch ?
	if(hints)
	{
		#if defined(CONFIG_BASE_PS3)
		logMsg("optimizing texture for frequent updates");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_ALLOCATION_HINT_SCE, GL_TEXTURE_LINEAR_SYSTEM_SCE);
		#endif

		#ifdef SUPPORT_ANDROID_DIRECT_TEXTURE
		if(useEGLImageKHR)
		{
			if(setupEGLImage(pix, texRef, usedX, usedY))
			{
				logMsg("using EGL image for texture, %dx%d %s", usedX, usedY, pix->format->name);
				pix->x = usedX;
				pix->y = usedY;
				bufImg.isDirect = 1;
				return texRef;
			}
			else
			{
				logWarn("failed to create EGL image, falling back to normal texture");
				bufImg.isDirect = 0;
			}
		}
		#endif

		includePadding = 1; // avoid slow OpenGL ES upload case, should be faster on OpenGL too
	}

	if(useAutoMipmapGeneration)
	{
		#ifndef CONFIG_GFX_OPENGL_ES
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
		#elif !defined(CONFIG_BASE_PS3)
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		#else
			// TODO: find PS3 version
		#endif
	}
	{
		GLenum format = pixelFormatToOGLFormat(*pix->format);
		GLenum dataType = pixelFormatToOGLDataType(*pix->format);
		logMsg("%s texture %dx%d with internal format %s from image %s:%s", upload ? "uploading" : "creating", pix->x, pix->y, glImageFormatToString(internalFormat), glImageFormatToString(format), glDataTypeToString(dataType));
	}
	if(newTexture && replaceGLTexture(pix, upload, internalFormat, includePadding))
	{
		//logMsg("success");
	}
	else if(upload && !writeGLTexture(pix, includePadding))
	{
		logErr("error writing texture");
		texReuseCache_doneUsing(&texCache, texRef);
		return 0;
	}

	#ifndef CONFIG_GFX_OPENGL_ES
	if(upload && useFBOFuncs)
	{
		logErr("generating mipmaps");
		glGenerateMipmapEXT(GL_TEXTURE_2D);
	}
	#endif

	return(texRef);
}

static const PixelFormatDesc *swapRGBToPreferedOrder(const PixelFormatDesc *fmt)
{
	if(Gfx::preferBGR && fmt->id == PIXEL_RGB888)
		return &PixelFormatBGR888;
	else if(Gfx::preferBGRA && fmt->id == PIXEL_RGBA8888)
		return &PixelFormatBGRA8888;
	else
		return fmt;
}

void GfxBufferImage::setFilter(uint filter)
{
	var_copy(filterGL, openGLFilterType(filter, usingMipmaping() ? MIPMAP_LINEAR : MIPMAP_NONE));
	logMsg("setting texture filter %s", filter == GfxBufferImage::nearest ? "nearest" : "linear");
	Gfx::setActiveTexture(tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterGL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterGL);
}

#if defined(CONFIG_RESOURCE_IMAGE)
CallResult GfxBufferImage::subInit(ResourceImage &img, int x, int y, int xSize, int ySize)
{
	using namespace Gfx;
	uint texX, texY;
	textureSizeSupport.findBufferXYPixels(texX, texY, img.width(), img.height());
	tid = img.gfxD.tid;
	xStart = pixelToTexC((uint)x, texX);
	yStart = pixelToTexC((uint)y, texY);
	xEnd = pixelToTexC((uint)x+xSize, texX);
	yEnd = pixelToTexC((uint)y+ySize, texY);
	return OK;
}

CallResult GfxBufferImage::init(ResourceImage &img, uint filter)
{
	using namespace Gfx;
	//logMsg("GfxBufferImage::init");
	int wrapMode = bestClampMode(0);

	uint texX, texY;
	textureSizeSupport.findBufferXYPixels(texX, texY, img.width(), img.height());

	var_copy(pixFmt, swapRGBToPreferedOrder(img.pixelFormat()));
	Pixmap texPix;
	uint uploadPixStoreSize = texX * texY * pixFmt->bytesPerPixel;
	#if defined(CONFIG_BASE_PS3)
	//logMsg("alloc in heap"); // PS3 has 1MB stack limit
	uchar *uploadPixStore = (uchar*)mem_alloc(uploadPixStoreSize);
	if(!uploadPixStore)
		return OUT_OF_MEMORY;
	#else
	uchar uploadPixStore[uploadPixStoreSize] __attribute__ ((aligned (8)));
	#endif
	mem_zero(uploadPixStore, uploadPixStoreSize);
	texPix.init(uploadPixStore, pixFmt, texX, texY, 0);
	img.getImage(&texPix);
	tid = createGLTexture(&texPix, 1, pixelToOGLInternalFormat(*texPix.format), wrapMode, wrapMode, img.width(), img.height(), 0, *this, filter);
	if(!tid)
	{
		#if defined(CONFIG_BASE_PS3)
		mem_free(uploadPixStore);
		#endif
		return INVALID_PARAMETER;
	}

	xStart = pixelToTexC((uint)0, texPix.x);
	yStart = pixelToTexC((uint)0, texPix.y);
	xEnd = pixelToTexC(img.width(), texPix.x);
	yEnd = pixelToTexC(img.height(), texPix.y);

	#if defined(CONFIG_BASE_PS3)
	mem_free(uploadPixStore);
	#endif
	return OK;
}
#endif

CallResult GfxBufferImage::init(Pixmap &pix, bool upload, uint filter, uint hints)
{
	using namespace Gfx;
	if(tid)
		deinit();

	var_selfs(hints);

	int wrapMode = bestClampMode(0);

	uint xSize = hints ? pix.pitchPixels() : pix.x;
	uint texX, texY;
	textureSizeSupport.findBufferXYPixels(texX, texY, xSize, pix.y,
		hints ? TextureSizeSupport::streamHint : 0);

	Pixmap texPix;
	texPix.init(0, pix.format, texX, texY, 0);

	/*uchar uploadPixStore[texX * texY * pix.format->bytesPerPixel];

	if(upload && pix.pitch != uploadPix.pitch)
	{
		mem_zero(uploadPixStore);
		pix.copy(0, 0, 0, 0, &uploadPix, 0, 0);
	}*/
	assert(upload == 0);

	tid = createGLTexture(&texPix, upload, pixelToOGLInternalFormat(*pix.format), wrapMode, wrapMode, pix.x, pix.y, hints, *this, filter);
	if(!tid)
	{
		return INVALID_PARAMETER;
	}

	xStart = pixelToTexC((uint)0, texPix.x);
	yStart = pixelToTexC((uint)0, texPix.y);
	xEnd = pixelToTexC(pix.x, texPix.x);
	yEnd = pixelToTexC(pix.y, texPix.y);

	return OK;
}

Pixmap *GfxBufferImage::lock(uint x, uint y, uint xlen, uint ylen)
{
	#ifdef SUPPORT_ANDROID_DIRECT_TEXTURE
	assert(isDirect);
	void *data;
	if(grallocMod->lock(grallocMod, eglBuf.handle, GRALLOC_USAGE_SW_WRITE_OFTEN, x, y, xlen, ylen, &data) != 0)
	{
		logMsg("error locking");
		return 0;
	}
	eglPixmap.data = (uchar*)data;
	return &eglPixmap;
	#endif
	return 0;
}

void GfxBufferImage::unlock()
{
	#ifdef SUPPORT_ANDROID_DIRECT_TEXTURE
	assert(isDirect);
	grallocMod->unlock(grallocMod, eglBuf.handle);
	#endif
}

void GfxBufferImage::write(Pixmap &p)
{
	glcBindTexture(GL_TEXTURE_2D, tid);

	#ifdef SUPPORT_ANDROID_DIRECT_TEXTURE
	if(isDirect)
	{
		//logMsg("updating EGL image");
		void *data;
		Pixmap *texturePix = lock(0, 0, p.x, p.y);
		if(!texturePix)
		{
			return;
		}
		p.copy(0, 0, 0, 0, texturePix, 0, 0);
		unlock();
		return;
	}
	#endif

	//logMsg("pix data %p", p.data);
	writeGLTexture(&p, hints);

	#ifdef CONFIG_BASE_ANDROID
		if(unlikely(glSyncHackEnabled)) glFinish();
	#endif
}

void GfxBufferImage::replace(Pixmap &p)
{
	//logMsg("replace");
	glcBindTexture(GL_TEXTURE_2D, tid);
	replaceGLTexture(&p, 1, pixelToOGLInternalFormat(*p.format), hints);
}

void GfxBufferImage::deinit()
{
	#ifdef SUPPORT_ANDROID_DIRECT_TEXTURE
	if(isDirect)
	{
		eglImgCount--;
		assert(eglImgCount == 0);
		logMsg("freeing EGL image");
		eglDestroyImageKHR(Base::getAndroidEGLDisplay(), eglImg);
		if(allocDev->free && allocDev->free(allocDev, eglBuf.handle) != 0)
		{
			logWarn("error freeing buffer");
		}
		isDirect = 0;
	}
	#endif

	texReuseCache_doneUsing(&texCache, tid);
	tid = 0;
}
