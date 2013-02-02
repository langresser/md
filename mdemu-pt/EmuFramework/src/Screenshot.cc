#include <Screenshot.hh>
#include <data-type/image/libpng/reader.h>
#include <io/sys.hh>
#include <fs/sys.hh>

static void png_ioWriter(png_structp pngPtr, png_bytep data, png_size_t length)
{
	Io *stream = (Io*)png_get_io_ptr(pngPtr);

	if(stream->fwrite(data, length, 1) != 1)
	{
		logErr("error writing png file");
		//png_error(pngPtr, "Write Error");
	}
}

static void png_ioFlush(png_structp pngPtr)
{
	logMsg("called png_ioFlush");
}

bool writeScreenshot(const Pixmap &vidPix, const char *fname)
{
	Io *fp = IoSys::create(fname);
	if(!fp)
	{
		return 0;
	}

	png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!pngPtr)
	{
		fp->close();
		FsSys::remove(fname);
		return 0;
	}
	png_infop infoPtr = png_create_info_struct(pngPtr);
	if(!infoPtr)
	{
		png_destroy_write_struct(&pngPtr, (png_infopp)NULL);
		fp->close();
		FsSys::remove(fname);
		return 0;
	}

	if(setjmp(png_jmpbuf(pngPtr)))
	{
		png_destroy_write_struct(&pngPtr, &infoPtr);
		fp->close();
		FsSys::remove(fname);
		return 0;
	}

	uint imgwidth = vidPix.x;
	uint imgheight = vidPix.y;

	png_set_write_fn(pngPtr, fp, png_ioWriter, png_ioFlush);
	png_set_IHDR(pngPtr, infoPtr, imgwidth, imgheight, 8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

	png_write_info(pngPtr, infoPtr);

	//png_set_packing(pngPtr);

	png_byte *rowPtr= (png_byte*)mem_alloc(png_get_rowbytes(pngPtr, infoPtr));
	uint8 *screen = vidPix.data;
	for(uint y=0; y < vidPix.y; y++, screen+=vidPix.pitch)
	{
		png_byte *rowpix = rowPtr;
		for(uint x=0; x < vidPix.x; x++)
		{
			// assumes RGB565
			uint16 pixVal = *(uint16 *)(screen+2*x);
			uint32 r = pixVal >> 11, g = (pixVal >> 5) & 0x3f, b = pixVal & 0x1f;
			r *= 8; g *= 4; b *= 8;
			*(rowpix++) = r;
			*(rowpix++) = g;
			*(rowpix++) = b;
			if(imgwidth!=vidPix.x)
			{
				*(rowpix++) = r;
				*(rowpix++) = g;
				*(rowpix++) = b;
			}
		}
		png_write_row(pngPtr, rowPtr);
		if(imgheight!=vidPix.y)
			png_write_row(pngPtr, rowPtr);
	}

	mem_free(rowPtr);

	png_write_end(pngPtr, infoPtr);
	png_destroy_write_struct(&pngPtr, &infoPtr);

	fp->close();
	logMsg("%s saved.", fname);
	return 1;
}
