#include <AlertView.hh>

void AlertView::init(const char *label, MenuItem **menuItem, bool highlightFirst)
{
	labelFrame.init();
	text.init(label, View::defaultFace);
	menu.init(menuItem, 2, highlightFirst, C2DO);
	menu.tbl.onlyScrollIfNeeded = 1;
}

void AlertView::deinit()
{
	//logMsg("deinit AlertView");
	text.deinit();
	menu.deinit();
}

void AlertView::place()
{
	uint xSize = rect.xSize() * .8;
	text.maxLineSize = Gfx::iXSize(xSize) * 0.95;
	text.compile();

	uint menuYSize = menu.items * menu.item[0]->ySize()*2;
	uint labelYSize = Gfx::toIYSize(text.ySize + (text.nominalHeight * .5));
	Rect2<int> viewFrame;
	viewFrame.setPosRel(rect.xSize()/2, rect.ySize()/2,
			xSize, labelYSize + menuYSize, C2DO);

	labelFrame.setXSize(Gfx::iXSize(viewFrame.xSize()));
	labelFrame.setYSize(Gfx::iYSize(labelYSize));
	labelFrame.setPos(&viewFrame, 0, 0, LT2DO, LT2DO);

	Rect2<int> menuViewFrame;
	menuViewFrame.setPosRel(viewFrame.x, viewFrame.y + labelYSize,
			viewFrame.xSize(), menuYSize, LT2DO);
	menu.place(menuViewFrame);
}

void AlertView::inputEvent(const InputEvent &e)
{
	if(e.state == INPUT_PUSHED)
	{
		if(e.isDefaultCancelButton())
		{
			removeModalView();
			return;
		}
	}
	menu.inputEvent(e);
}

void AlertView::draw()
{
	using namespace Gfx;
	setBlendMode(BLEND_MODE_ALPHA);
	resetTransforms();
	setColor(.4, .4, .4, .8);
	GeomRect::draw(&labelFrame);
	setColor(.1, .1, .1, .6);
	GeomRect::draw(menu.viewRect());

	setColor(COLOR_WHITE);
	text.draw(labelFrame.xPos(C2DO), labelFrame.yPos(C2DO), C2DO, C2DO);
	//setClipRect(1);
	//setClipRectBounds(menu.viewRect());
	menu.draw();
	//setClipRect(0);
}
