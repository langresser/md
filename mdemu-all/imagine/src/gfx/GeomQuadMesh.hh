#pragma once

#include <gfx/interface.h>
#include <util/Array2D.hh>

class GeomQuadMesh
{
public:
	constexpr GeomQuadMesh(): verts(0), idxs(0), i(0) { }
	//CallResult init(VertexPos *q, uint qSize, VertexPos *span, uint extraSpans, VertexColor color, uint blend);
	CallResult init(const VertexPos *x, uint xVals, const VertexPos *y, uint yVals, VertexColor color = 0);
	template <size_t S1, size_t S2>
	CallResult init(const VertexPos (&x)[S1], const VertexPos (&y)[S2], VertexColor color = 0)
	{
		return init(x, S1, y, S2, color);
	}
	template <size_t S1>
	CallResult init(const VertexPos (&x)[S1], const VertexPos *y, uint yVals, VertexColor color = 0)
	{
		return init(x, S1, y, yVals, color);
	}
	template <size_t S2>
	CallResult init(const VertexPos *x, uint xVals, const VertexPos (&y)[S2], VertexColor color = 0)
	{
		return init(x, xVals, y, S2, color);
	}
	void deinit();
	void draw();
	void setColorRGB(GColor r, GColor g, GColor b);
	void setColorTranslucent(GColor a);
	void setColorRGBV(GColor r, GColor g, GColor b, uint i);
	void setColorTranslucentV(GColor a, uint i);
	void setPos(GC x, GC y, GC x2, GC y2);

	uint verts;
	Array2D<ColVertex> v;
	uint idxs;
	VertexIndex *i;
} ;
