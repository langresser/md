#pragma once

#include <base/interface.h>
#include <input/interface.h>
#include <input/DragPointer.hh>
#include <resource2/face/ResourceFace.hh>

class ViewAnimation
{
public:
	constexpr ViewAnimation() { }
	virtual void initShow() = 0;
	virtual void initActive() = 0;
	virtual void initDismiss() = 0;
	virtual bool update() = 0;
};

class View
{
public:
	constexpr View() : dismissHandler(0), animation(0), displayState(0), name_("") { }
	constexpr View(const char *name) : dismissHandler(0), animation(0), displayState(0), name_(name) { }

	virtual void deinit() = 0;
	virtual Rect2<int> &viewRect() = 0;
	virtual void place() = 0;
	void place(const Rect2<int> &rect)
	{
		this->viewRect() = rect;
		place();
	}
	virtual void draw() = 0;
	virtual void inputEvent(const InputEvent &event) = 0;
	virtual void clearSelection() { }; // de-select any items from previous input
	virtual void onShow() { };

	static ResourceFace *defaultFace;

	void (*dismissHandler)();

	enum { SHOW, ACTIVE, HIDE };
	ViewAnimation *animation;
	uint displayState;

	const char *name_;
	const char *name() { return name_; }

	// Does the platform need an on-screen/pointer-based control to move to a previous view?
	static bool needsBackControl;
	static const bool needsBackControlDefault = !(Config::envIsPS3 || Config::envIsAndroid || Config::envIsWebOS);
	static const bool needsBackControlIsConst = Config::envIsPS3 || Config::envIsIOS;

	static void setNeedsBackControl(bool on)
	{
		if(!needsBackControlIsConst) // only modify on environments that make sense
		{
			needsBackControl = on;
		}
	}

	void doDismiss()
	{
		logMsg("dimiss view with hanlder %p", dismissHandler);
		deinit();
		if(dismissHandler)
			dismissHandler();
	}

	void dismiss(void (*handler)() = 0)
	{
		if(handler)
			dismissHandler = handler;
		if(!animation)
		{
			doDismiss();
		}
		else
		{
			animation->initDismiss();
			Base::displayNeedsUpdate();
		}
		displayState = HIDE;
	}

	void show(bool animated = 1)
	{
		if(animated && animation)
		{
			animation->initShow();
			displayState = SHOW;
		}
		else
		{
			if(animation)
				animation->initActive();
			displayState = ACTIVE;
		}
		onShow();
		//logMsg("showed view");
		Base::displayNeedsUpdate();
	}

	void init(ViewAnimation *animation = 0, bool animated = 0)
	{
		this->animation = animation;
		//show(animated);
	}

	bool updateAnimation()
	{
		if(animation && animation->update())
		{
			// still animating
			Base::displayNeedsUpdate();
			return 1;
		}

		if(displayState == HIDE)
		{
			// hide animation done, trigger view to dismiss
			doDismiss();
			return 0;
		}

		if(displayState == SHOW)
		{
			// show animation complete
			displayState = ACTIVE;
		}

		// not animating, view is active
		return 1;
	}
};
