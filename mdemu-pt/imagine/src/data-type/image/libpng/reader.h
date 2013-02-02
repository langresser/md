#pragma once

#include <engine-globals.h>

#define PNG_SKIP_SETJMP_CHECK
#include <png.h>

#include <io/Io.hh>
class PixelFormatDesc;

class Png
{
public:
	CallResult readHeader (Io *stream);
	CallResult readImage (Io *stream, void* buffer, uint bufferLinePadBytes, const PixelFormatDesc &outFormat);
	int getWidth ();
	int getHeight ();
	bool hasAlphaChannel();
	bool isGrayscale();
	const PixelFormatDesc *pixelFormat();
	void freeImageData();
private:
	png_structp png;
	png_infop info;
	//png_infop end;
	void setTransforms(const PixelFormatDesc &outFormat, png_infop transInfo);
	static bool supportUncommonConv;
};
