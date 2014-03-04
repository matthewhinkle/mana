//
//  x-next-event.c
//  Mana
//
//  Created by Matthew Hinkle on 2/25/14.
//  Copyright (c) 2014 Matthew Hinkle. All rights reserved.
//

#if defined (__unix__) || defined (__linux__) || (defined (__APPLE__) && defined (__MACH__) && defined (MANA_USE_X11) && !defined (MANA_USE_X11_XCB))

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#include "event.h"
#include "input.h"
#include "window.h"
#include "x-key.h"

static inline mfwMod makeMod(unsigned state) {
	mfwMod mod = Mod_Nil;

	if(state & ShiftMask) mod |= Mod_Shift;
	if(state & ControlMask) mod |= Mod_Control;
	if(state & LockMask) mod |= Mod_Lock;

	if(state & Mod1Mask) mod |= Mod_Alt;
	if(state & Mod2Mask) mod |= Mod_NumLock;
	if(state & Mod3Mask) mod |= Mod_AltGr;
	if(state & Mod4Mask) mod |= Mod_Mod;
	if(state & Mod5Mask) mod |= Mod_ScrollLock;

	if(state & Button1Mask) mod |= Mod_MouseButton1;
	if(state & Button2Mask) mod |= Mod_MouseButton2;
	if(state & Button3Mask) mod |= Mod_MouseButton3;
	if(state & Button4Mask) mod |= Mod_MouseButton4;
	if(state & Button5Mask) mod |= Mod_MouseButton5;

	return mod;
}

static inline mfwButton makeButton(XButtonEvent * restrict x) {
	if(!x) return Mouse_Nil;

	switch(x->button) {
	case Button1: return Mouse_Button1;
	case Button2: return Mouse_Button2;
	case Button3: return Mouse_Button3;
	case Button4: return Mouse_Button4;
	case Button5: return Mouse_Button5;
	default: return Mouse_Unknown;
	}
}

static inline void mapKeyPressEvent(struct mfwWindow * restrict window, XKeyEvent * restrict x, mfwEvent * restrict e) {
	KeySym sym = 0;
	XkbLookupKeySym(window->display, x->keycode, x->state, NULL, &sym);
	const mfwKey key = makeKey((unsigned)sym);
	const mfwMod mod = makeMod(x->state);
	*e = mfwMakeKeyPressEvent(key, mod, x->x, x->y, window, (uint64_t)x->time);
}

static inline void mapKeyReleaseEvent(struct mfwWindow * restrict window, XKeyEvent * restrict x, mfwEvent * restrict e) {
	KeySym sym = 0;
	XkbLookupKeySym(window->display, x->keycode, x->state, NULL, &sym);
	const mfwKey key = makeKey((unsigned)sym);
	const mfwMod mod = makeMod(x->state);
	*e = mfwMakeKeyReleaseEvent(key, mod, x->x, x->y, window, (uint64_t)x->time);
}

static inline void mapMouseMotionEvent(struct mfwWindow * restrict window, XMotionEvent * restrict x, mfwEvent * e) {
	const mfwMod mod = makeMod(x->state);
	*e = mfwMakeMouseMotionEvent(x->x, x->y, mod, window, (uint64_t)x->time);
}

static const int MOUSE_SCROLL_UP = 1;
static const int MOUSE_SCROLL_DOWN = -1;

static inline void mapMouseScrollEvent(struct mfwWindow * restrict window, XButtonEvent * restrict x, mfwEvent * restrict e) {
	const mfwButton button = makeButton(x);
	const mfwMod mod = makeMod(x->state);
	if(Mouse_Button3 & button) {
		*e = mfwMakeMouseScrollEvent(mod, 0, MOUSE_SCROLL_UP, 0, window, (uint64_t)x->time);
	} else if (Mouse_Button4 & button) {
		*e = mfwMakeMouseScrollEvent(mod, 0, MOUSE_SCROLL_DOWN, 0, window, (uint64_t)x->time);
	} else {
		*e = mfwMakeUnknownEvent();
	}
}

static inline void mapMousePressEvent(struct mfwWindow * restrict window, XButtonEvent * restrict x, mfwEvent * restrict e) {
	const mfwButton button = makeButton(x);
	const mfwMod mod = makeMod(x->state);

	if(Mouse_Button4 & button || Mouse_Button5 & button) {
		mapMouseScrollEvent(window, x, e);
	} else {
		*e = mfwMakeMousePressEvent(x->x, x->y, mod, button, window, (uint64_t)x->time);
	}
}

static inline void mapMouseReleaseEvent(struct mfwWindow * restrict window, XButtonEvent * restrict x, mfwEvent * restrict e) {
	const mfwButton button = makeButton(x);
	const mfwMod mod = makeMod(x->state);
	if(Mouse_Button4 & button || Mouse_Button5 & button) return;
	*e = mfwMakeMouseReleaseEvent(x->x, x->y, mod, button, window, (uint64_t)x->time);
}

static inline void mapWindowCreateEvent(struct mfwWindow * restrict window, XCreateWindowEvent * restrict x, mfwEvent * restrict e) {
	*e = mfwMakeWindowCreateEvent(window);
}

static inline void mapWindowExposeEvent(struct mfwWindow * restrict window, XExposeEvent * restrict x, mfwEvent * restrict e) {
	*e = mfwMakeWindowExposeEvent(window);
}

static inline void mapWindowConfigureEvent(struct mfwWindow * restrict window, XConfigureEvent * restrict x, mfwEvent * restrict e) {
	*e = mfwMakeWindowConfigureEvent(x->x, x->y, x->width, x->height, window);
}

static inline void mapWindowExitEvent(struct mfwWindow * restrict window, XDestroyWindowEvent * restrict x, mfwEvent * restrict e) {
	*e = mfwMakeWindowExitEvent(window);
}

static int mapEvent(struct mfwWindow * restrict window, XEvent * restrict x, mfwEvent * restrict e)
{
	assert(window);

	if(!(x && e)) return 0;

	switch(x->type) {
	case MotionNotify: mapMouseMotionEvent(window, &x->xmotion, e); return 1;
	case KeyPress: mapKeyPressEvent(window, &x->xkey, e); return 1;
	case KeyRelease: mapKeyReleaseEvent(window, &x->xkey, e); return 1;
	case ButtonPress: mapMousePressEvent(window, &x->xbutton, e); return 1;
	case ButtonRelease: mapMouseReleaseEvent(window, &x->xbutton, e); return 1;
	case CreateNotify: mapWindowCreateEvent(window, &x->xcreatewindow, e); return 1;
	case Expose: mapWindowExposeEvent(window, &x->xexpose, e); return 1;
	case ConfigureNotify: mapWindowConfigureEvent(window, &x->xconfigure, e); return 1;
	case DestroyNotify: mapWindowExitEvent(window, &x->xdestroywindow, e); return 1;
	default: *e = mfwMakeUnknownEvent(); return 0;
	}
}

static void handleInternalMappingEvent(XMappingEvent * restrict x)
{
	XRefreshKeyboardMapping(x);
}

static void handleInternalEvent(XEvent * restrict x)
{
	if(!x) return;

	switch(x->type) {
	case MappingNotify: handleInternalMappingEvent(&x->xmapping); return;
	default: return;
	}
}

extern int mfwEvent_next(struct mfwWindow * restrict window, mfwEvent * restrict event)
{
	assert(window);
	assert(window->display);

	if(mfwEvent_sharedQueue_next(event)) return 1;

	Display * display = window->display;
	assert(display);

	if(XPending(display)) {
		XEvent e;
		XNextEvent(display, &e);
		if(!mapEvent(window, &e, event)) {
			handleInternalEvent(&e);
		}
		return 1;
	}

	return 0;
}

extern int mfwEvent_peek(struct mfwWindow * restrict window, mfwEvent * restrict event)
{
	assert(window);

	if(mfwEvent_sharedQueue_peek(event)) return 1;

	Display * display = window->display;
	assert(display);

	if(XPending(display)) {
		XEvent x;
		XPeekEvent(display, &x);
		mapEvent(window, &x, event);
		return 1;
	}

	return 0;
}

#endif
