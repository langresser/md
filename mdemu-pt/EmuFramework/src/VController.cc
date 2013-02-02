/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#define thisModuleName "msgPopup"
#include <VController.hh>

void VControllerDPad::init()
{
	padArea.setZero();
	origin = LT2DO;

	/*if(visualizeBounds)
	{
		mapPix.init((uchar*)mapPixBuff, &PixelFormatRGBA8888, 128, 128);
		mapImg.init(mapPix, 0);
		mapSpr.init(&mapImg);
	}*/
}

void VControllerDPad::setImg(ResourceImage *dpadR, GC texHeight)
{
	spr.init(-.5, -.5, .5, .5, dpadR);
	spr.setImg(dpadR, 0., 0., 1., 64./texHeight);
}

void VControllerDPad::place(GC padFullSize, GC centerBtnYOffset)
{
	GC padAreaXOffset = Gfx::proj.aspectRatio < 1. ? Gfx::xMMSize(0.25) : Gfx::xMMSize(1.);
	GC padExtraArea = 1.5;
	padBase.init(1, 1);
	padBase.setYSize(padFullSize);
	padBase.setPos(padAreaXOffset * -origin.xScaler(), origin.onYCenter() ? 0 : centerBtnYOffset * -origin.yScaler(), origin, origin);
	spr.setPos(padBase);

	padArea.setPosRel(padBase.xIPos(C2DO), padBase.yIPos(C2DO), (GC)padBase.iYSize*padExtraArea, C2DO);

	// TODO: update for new getInput()
	if(visualizeBounds)
	{
		mapPix.init(&PixelFormatRGBA8888, padArea.xSize(), padArea.ySize());
		mapImg.init(mapPix, 0);
		mapSpr.init(&mapImg);

		iterateTimes(mapPix.y, y)
			iterateTimes(mapPix.x, x)
			{
				//uint input[4] = { 0 };
				int input = getInput(padArea.xPos(LT2DO) + x, padArea.yPos(LT2DO) + y);
				//logMsg("got input %d", input);
				*((uint32*)mapPix.getPixel(x,y)) = input == -1 ? PixelFormatRGBA8888.build(1., 0., 0., 1.)
										: IG::isOdd(input) ? PixelFormatRGBA8888.build(0., 1., 0., 1.)
										: PixelFormatRGBA8888.build(1., 1., 1., 1.);
				/*int numDirs = 0;
				iterateTimes(4, i)
				{
					if(input[i])
						numDirs++;
				}
				mapPixBuff[y][x] = numDirs == 1 ? PixelFormatRGBA8888.build(1., 0., 0., 1.)
						: numDirs == 2 ? PixelFormatRGBA8888.build(0., 1., 0., 1.)
						: PixelFormatRGBA8888.build(1., 1., 1., 1.);*/
			}
		mapImg.write(mapPix);
		mapSpr.setPos(padArea);
	}
}

void VControllerDPad::draw()
{
	//{ gfx_resetTransforms(); GeomRect::draw(padArea); }
	spr.draw(0);

	if(visualizeBounds)
	{
		mapSpr.draw(0);
	}
}

int VControllerDPad::getInput(int cx, int cy)
{
	if(padArea.overlaps(cx, cy))
	{
		int x = cx - padArea.xCenter(), y = cy - padArea.yCenter();
		int xDeadzone = deadzone, yDeadzone = deadzone;
		if(IG::abs(x) > deadzone)
			yDeadzone += (IG::abs(x) - deadzone)/2;
		if(IG::abs(y) > deadzone)
			xDeadzone += (IG::abs(y) - deadzone)/2;
		//logMsg("dpad offset %d,%d, deadzone %d,%d", x, y, xDeadzone, yDeadzone);
		int pad = 4; // init to center
		if(IG::abs(x) > xDeadzone)
		{
			if(x > 0)
				pad = 5; // right
			else
				pad = 3; // left
		}
		if(IG::abs(y) > yDeadzone)
		{
			if(y > 0)
				pad += 3; // shift to top row
			else
				pad -= 3; // shift to bottom row
		}
		return pad == 4 ? -1 : pad; // don't send center dpad push
	}
	return -1;
}

GfxBufferImage VControllerDPad::mapImg;
Pixmap VControllerDPad::mapPix;
GfxSprite VControllerDPad::mapSpr;
//uint VControllerDPad::mapPixBuff[128][128];

void VControllerKeyboard::init()
{
	mode = 0;
}

void VControllerKeyboard::updateImg()
{
	if(mode)
		spr.setImg(spr.img, 0., .5, 384./512., 1.);
	else
		spr.setImg(spr.img, 0., 0., 384./512., .5);
}

void VControllerKeyboard::setImg(ResourceImage *img)
{
	spr.init(-.5, -.5, .5, .5, img);
	updateImg();
}

void VControllerKeyboard::place(GC btnSize, GC yOffset)
{
	area.init(3, 2);
	area.setXSize(IG::min(btnSize*10, Gfx::proj.w));
	GC vArea = Gfx::proj.h - yOffset*2;
	if(area.ySize > vArea)
		area.setYSize(vArea);
	area.setPos(0, yOffset, CB2DO, CB2DO);

	keyXSize = (area.iXSize / cols) + (area.iXSize * (1./256.));
	keyYSize = area.iYSize / 4;
	logMsg("key size %dx%d", keyXSize, keyYSize);

	spr.setPos(area);
}

void VControllerKeyboard::draw()
{
	spr.draw(0);
}

int VControllerKeyboard::getInput(int cx, int cy)
{
	if(area.overlaps(cx, cy))
	{
		int relX = cx - area.xIPos(LT2DO), relY = cy - area.yIPos(LT2DO);
		uint row = relY/keyYSize;
		uint col;
		if((mode == 0 && row == 1) || row == 2)
		{
			relX -= keyXSize/2;
			col = relX/keyXSize;
			if(relX < 0)
				col = 0;
			else if(col > 8)
				col = 8;
		}
		else
		{
			if(row > 3)
				row = 3;
			col = relX/keyXSize;
			if(col > 9)
				col = 9;
		}
		uint idx = col + (row*cols);
		logMsg("pointer %d,%d key @ %d,%d, idx %d", relX, relY, row, col, idx);
		return idx;
	}
	return -1;
}
#undef thisModuleName
