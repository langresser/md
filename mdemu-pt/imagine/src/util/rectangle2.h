#pragma once

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

#include <config.h>
#include <util/number.h>
#include <util/2DOrigin.h>
#ifdef CONFIG_GFX
#include <gfx/viewport.hh>
#endif

//TODO: remove old code

template<class T>
class Rect2
{
public:
	T x, y, x2, y2;

	constexpr Rect2(): x(0), y(0), x2(0), y2(0) { }
	constexpr Rect2(T x, T y, T x2, T y2): x(x), y(y), x2(x2), y2(y2) { }

	void setZero()
	{
		x = y = x2 = y2 = 0;
	}

	bool overlaps(const Rect2 &other) const
	{
		//logMsg("testing rect %d,%d %d,%d with %d,%d %d,%d", a1.x, a1.x2, a1.y, a1.y2, a2.x, a2.x2, a2.y, a2.y2);
		return  y2 < other.y ? 0 :
				y > other.y2 ? 0 :
				x2 < other.x ? 0 :
				x > other.x2 ? 0 : 1;
	}

	bool overlaps(T px, T py) const
	{
		//logMsg("testing %d,%d in rect %d,%d %d,%d", px, py, a.x, a.y, a.x2, a.y2);
		return IG::isInRange(px, x, x2) && IG::isInRange(py, y, y2);
	}

	bool contains(const Rect2 &other) const
	{
		//logMsg("testing %d,%d %d,%d is in %d,%d %d,%d", x, y, x2, y2, other.x, other.y, other.x2, other.y2);
		return x <= other.x && x2 >= other.x2 &&
			y <= other.y && y2 >= other.y2;
	}

	T xCenter() const
	{
		return ((x2 - x) / 2) + x;
	}

	T xPos(_2DOrigin origin) const
	{
		switch(origin.xScaler())
		{
			case -1: return x;
			case 1: return x2;
		}
		return xCenter();
	}

	T yCenter() const
	{
		return ((y2 - y) / 2) + y;
	}

	T yPos(_2DOrigin origin) const
	{
		switch(origin.invertYIfCartesian().yScaler())
		{
			case -1: return y;
			case 1: return y2;
		}
		return yCenter();
	}

	IG::Point2D<T> pos(_2DOrigin origin) const
	{
		return IG::Point2D<T>(xPos(origin), yPos(origin));
	}

	void setXPosRel(T newX, T size, _2DOrigin origin)
	{
		if(size <= 0) size = 1;
		size--;
		newX = origin.adjustX(newX, size, LTIC2DO);
		x = newX;
		x2 = newX + size;
	}

	void setYPosRel(T newY, T size, _2DOrigin origin)
	{
		if(size <= 0) size = 1;
		size--;
		newY = origin.invertYIfCartesian().adjustY(newY, size, LTIC2DO);
		y = newY;
		y2 = newY + size;
	}

	void setPosRel(T newX, T newY, T xSize, T ySize, _2DOrigin origin)
	{
		setXPosRel(newX, xSize, origin);
		setYPosRel(newY, ySize, origin);
		logMsg("set rect pos to %d,%d %d,%d", x, y, x2, y2);
	}

	void setPosRel(T newX, T newY, T size, _2DOrigin origin)
	{
		setPosRel(newX, newY, size, size, origin);
	}

	void setPosRel(IG::Point2D<T> pos, IG::Point2D<T> size, _2DOrigin origin)
	{
		setPosRel(pos.x, pos.y, size.x, size.y, origin);
	}

	void setPosRel(IG::Point2D<T> pos, T size, _2DOrigin origin)
	{
		setPosRel(pos.x, pos.y, size, size, origin);
	}

	#ifdef CONFIG_GFX
	void setPosRel(T newX, T newY, T xSize, T ySize, _2DOrigin posOrigin, _2DOrigin screenOrigin)
	{
		// adjust to the requested origin on the screen
		newX = LTIC2DO.adjustX(newX, (int)Gfx::viewPixelWidth(), screenOrigin.invertYIfCartesian());
		newY = LTIC2DO.adjustY(newY, (int)Gfx::viewPixelHeight(), screenOrigin.invertYIfCartesian());
		setPosRel(newX, newY, xSize, ySize, posOrigin);
	}

	void setPosRel(T newX, T newY, T size, _2DOrigin posOrigin, _2DOrigin screenOrigin)
	{
		setPosRel(newX, newY, size, size, posOrigin, screenOrigin);
	}
	#endif

	void setRel(T newX, T newY, T xSize, T ySize)
	{
		if(xSize <= 0) xSize = 1;
		x = newX;
		x2 = newX + (xSize-1);
		if(xSize <= 0) xSize = 1;
		y = newY;
		y2 = newY + (ySize-1);
		//logMsg("set rect to %d,%d %d,%d", x, y, x2, y2);
	}

	void setRelCentered(T newX, T newY, T xSize, T ySize)
	{
		x = x2 = newX;
		y = y2 = newY;
		T halfSizeX = xSize / (T)2;
		T halfSizeY = ySize / (T)2;
		x -= halfSizeX;
		y -= halfSizeY;
		x2 += halfSizeX;
		y2 += halfSizeY;
	}

	void setXPos(T newX)
	{
		IG::setLinked(x, newX, x2);
	}

	void setYPos(T newY)
	{
		IG::setLinked(y, newY, y2);
	}

	void setPos(T newX, T newY)
	{
		setXPos(newX);
		setYPos(newY);
	}

	void setPos(T newX, T newY, _2DOrigin origin)
	{
		newX = origin.adjustX(newX, xSize(), LTIC2DO);
		newY = origin.invertYIfCartesian().adjustY(newY, ySize(), LTIC2DO);
		setPos(newX, newY);
	}

	static Rect2 createRel(T newX, T newY, T xSize, T ySize)
	{
		Rect2 r;
		logMsg("creating new rel rect %d,%d %d,%d", newX, newY, xSize, ySize);
		r.setRel(newX, newY, xSize, ySize);
		return r;
	}

	T xSize() const { return (x2 - x) + 1; }

	T ySize() const { return (y2 - y) + 1; }

	IG::Point2D<T> size() const
	{
		return IG::Point2D<T>(xSize(), ySize());
	}

	// fit x,x2 inside r's x,x2 at the nearest edge
	int fitInX(const Rect2 &r)
	{
		if(x < r.x)
		{
			setXPos(r.x);
			return -1;
		}
		else if(x2 > r.x2)
		{
			IG::setLinked(x2, r.x2, x);
			return 1;
		}
		return 0;
	}

	// fit y,y2 inside r's y,y2 at the nearest edge
	int fitInY(const Rect2 &r)
	{
		if(y < r.y)
		{
			setYPos(r.y);
			return -1;
		}
		else if(y2 > r.y2)
		{
			IG::setLinked(y2, r.y2, y);
			return 1;
		}
		return 0;
	}

	// fit the rectangle inside r at the nearest edges
	// if inner doesn't fit in outer, TODO
	int fitIn(const Rect2 &r)
	{
		int boundedX = fitInX(r);
		int boundedY = fitInY(r);
		return boundedX || boundedY;
	}

	/*#ifdef CONFIG_GFX
	Coordinate gXSize() const { return gfx_iXSize(xSize()); }
	Coordinate gYSize() const { return gfx_iYSize(ySize()); }
	Coordinate gXPos(_2DOrigin o) const
	{
		T pos = xPos(o);
		if(o.xScaler() == 1)
			pos++;
		return gfx_iXPos(pos);
	}
	Coordinate gYPos(_2DOrigin o) const
	{
		T pos = yPos(o);
		if(o.invertYIfCartesian().yScaler() == 1)
			pos++;
		return gfx_iYPos(pos);
	}
	Coordinate gXPos(Coordinate scale, _2DOrigin o) const { return gXPos(o) + (gXSize() * (scale)); }
	Coordinate gYPos(Coordinate scale, _2DOrigin o) const { return gYPos(o) + (gYSize() * (scale)); }

	void setClipRectBounds()
	{
		gfx_setClipRectBounds(x, y, xSize(), ySize());
	}

	void setFullView()
	{
		x = y = 0;
		x2 = gfx_viewPixelWidth();
		y2 = gfx_viewPixelHeight();
	}

	void loadGfxTransforms(_2DOrigin o) const
	{
		gfx_loadTranslate(gXPos(o), gYPos(o));
		gfx_applyScale(gXSize(), gYSize());
	}
	#endif*/
};

