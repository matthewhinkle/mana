//
//  x-window.cpp
//  Mana
//
//  Created by Matthew Hinkle on 12/7/13.
//  Copyright (c) 2013 Matthew Hinkle. All rights reserved.
//

#if defined (__unix__) || defined (__linux__) || (defined (__APPLE__) && defined (__MACH__) && defined (MANA_USE_X11) && !defined (MANA_USE_X11_XCB))

#include "x-window.h"

/* GLContext X11 impl */

extern void mfwInitGLContext(mfwGLContext * restrict c, mfwWindow * restrict w)
{
	assert(c);
	assert(w);

	c->display = w->display;
	c->drawable = w->window;
	c->native = glXCreateContext(w->display, w->vi, NULL, True);
	assert(c->native);
}

extern void mfwDestroyGLContext(mfwGLContext * restrict c)
{
	if(!(c)) return;

	if(c->display && c->native) {
		glXDestroyContext(c->display, c->native);
		c->native = 0;
	}
}

extern mfwGLContext * mfwNewGLContext(mfwWindow * restrict w)
{
	mfwGLContext * c = NULL;
	if((c = emalloc(sizeof(mfwGLContext)))) {
		mfwInitGLContext(c, w);
	}
	return c;
}

extern void mfwDeleteGLContext(mfwGLContext * restrict c)
{
	if(!(c)) return;

	mfwDestroyGLContext(c);
	free(c);
}

extern const void * mfwGLContext_getNativeContext(mfwGLContext * restrict c)
{
	assert(c);
	assert(c->native);

	return c->native;
}

extern void mfwGLContext_makeCurrent(mfwGLContext * restrict c)
{
	assert(c);
	assert(c->display);
	assert(c->drawable);
	assert(c->native);

	glXMakeContextCurrent(c->display, c->drawable, c->drawable, c->native);
}

extern void mfwGLContext_swapBuffers(mfwGLContext * restrict c)
{
	assert(c);
	assert(c->display);
	assert(c->drawable);

	glXSwapBuffers(c->display, c->drawable);
}

extern void mfwGLContext_lock(mfwGLContext * restrict c)
{
	assert(c);
	assert(c->display);
	XLockDisplay(c->display);
}

extern void mfwGLContext_unlock(mfwGLContext * restrict c)
{
	assert(c);
	assert(c->display);
	XUnlockDisplay(c->display);
}

/* Window X11 impl */

static XVisualInfo * get_visualInfo(mfwWindow * restrict w)
{
	static int VI_ATTR[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 32,
		None
	};

	return glXChooseVisual(w->display, w->screen, VI_ATTR);
}

static const unsigned long EVENT_MASK = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask | PointerMotionHintMask;
static const unsigned long VALUE_MASK =  CWBackPixel | CWBorderPixel | CWColormap | CWCursor | CWEventMask;

static Display * openDisplay(char * restrict name)
{
	int event = 0;
	int error = 0;
	int major = XkbMajorVersion;
	int minor = XkbMinorVersion;
	int reason = 0;
	Display * d = XkbOpenDisplay(NULL, &event, &error, &major, &minor, &reason);
	switch(reason) {
	case XkbOD_Success: return d;
	case XkbOD_BadLibraryVersion: ERROR("Failed to initialize Xkb: version incompatibility between compile and run-time Xlib/Xkb libraries.\n"); break;
	case XkbOD_ConnectionRefused: ERROR("Failed to initialize Xkb: connection refused when attempting to open display.\n"); break;
	case XkbOD_BadServerVersion: ERROR("Failed to initialize Xkb: incompatible Xkb extension version between library and server.\n"); break;
	case XkbOD_NonXkbServer: WARN("Failed to initialize Xkb: server does not support Xkb.\n"); break;
	default: WARN("Unknown XkbOpenDisplay reason [id = %d] encountered.\n", reason); break;
	}

	WARN("Falling back to core Xlib keyboard implemenation.\n");
	return XOpenDisplay(name);
}

static void initWindow(mfwWindow * restrict w, recti * restrict frame)
{
	w->display = openDisplay(NULL);
	assert(w->display);

	w->screen = XDefaultScreen(w->display);

	XVisualInfo * vi = get_visualInfo(w);
	assert(vi);
	w->vi = vi;

	Window root = XRootWindow(w->display, vi->screen);
	assert(root);

	w->colormap = XCreateColormap(w->display, root, vi->visual, AllocNone);
	XSetWindowAttributes attr;
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = w->colormap;
	attr.event_mask = EVENT_MASK;

	recti bounds;
	if(frame) {
		recti_init(&bounds, 0, 0, frame->width, frame->height);
		w->frame = *frame;
	} else {
		Screen * xscreen = XScreenOfDisplay(w->display, w->screen);
		recti_init(&bounds, 0, 0, xscreen->width, xscreen->height);
		w->frame = bounds;
	}

	w->window = XCreateWindow(
		w->display,
		root,
		bounds.x,
		bounds.y,
		bounds.width,
		bounds.height,
		0,	/* no border */
		vi->depth,
		InputOutput,
		vi->visual,
		VALUE_MASK,
		&attr
	);
}

static const uint8_t PROPERTY_FORMAT_SIZE_8 = 8;
static const uint8_t PROPERTY_FORMAT_SIZE_32 = 32;
static const uint32_t MAX_TITLE_LEN = 128;

extern void mfwWindow_setTitle(struct mfwWindow * restrict w, const char * restrict title) {
	assert(w);
	assert(w->display);
	assert(w->window);

	const Atom prop = XInternAtom(w->display, "WM_NAME", True);
	assert(prop);
	const Atom type = XInternAtom(w->display, "STRING", True);
	assert(type);

	XChangeProperty(
		w->display,
		w->window,
		prop, type,
		PROPERTY_FORMAT_SIZE_8,
		PropModeReplace,
		(unsigned char *)title,
		MAX_TITLE_LEN
	);
	XFlush(w->display);
}

extern void mfwWindow_setFrame(struct mfwWindow * restrict w, recti frame)
{
	assert(w);
	assert(w->display);
	assert(w->window);

	XWindowChanges values[] = { frame.x, frame.y, frame.width, frame.height };

	XConfigureWindow(
		w->display,
		w->window,
		CWX | CWY | CWWidth | CWHeight,
		values
	);
	XFlush(w->display);
}

extern void mfwInitWindow(mfwWindow * restrict w, const char * restrict title, recti frame)
{
	assert(w);

	initWindow(w, &frame);
	XMapWindow(w->display, w->window);

	if(title) {
		mfwWindow_setTitle(w, title);
	}

	mfwWindow_setFrame(w, frame);
	XFlush(w->display);
}

static const uint32_t HINTS_FULLSCREEN[] = {
		2,		/* flags: specify we are changing the decorations field (idx 2) */
		0,		/* functions: unused for fullscreen */
		0,		/* decorations: remove decorations */
		0,		/* inputMode: unused */
		0		/* status: unused */
	};
static const uint32_t HINTS_WINDOWED[] = {
		2,		/* flags: specify we are changing the decorations field (idx 2) */
		0,		/* functions: unused for fullscreen */
		1,		/* decorations: add decorations */
		0,		/* inputMode: unused */
		0		/* status: unused */
	};

static void mfwWindow_setFullscreen_internal(mfwWindow * restrict w, int fullscreen) {
	assert(w);

	Atom prop = XInternAtom(w->display, "_MOTIF_WM_HINTS", True);
	assert(prop);

	static const int hintCount = sizeof(HINTS_FULLSCREEN) / sizeof(*HINTS_FULLSCREEN);

	XChangeProperty(
		w->display,
		w->window,
		prop,
		prop,
		PROPERTY_FORMAT_SIZE_32,
		PropModeReplace,
		(const unsigned char *)HINTS_FULLSCREEN,
		hintCount);
}

extern void mfwInitWindowFullscreen(mfwWindow * restrict w)
{
	assert(w);

	initWindow(w, NULL);
	XMapWindow(w->display, w->window);
	mfwWindow_setFullscreen_internal(w, 1);
	XFlush(w->display);
}

extern void mfwDestroyWindow(mfwWindow * restrict w)
{
	if(!w) return;

	if(w->display) {
		XFreeColormap(w->display, w->colormap);
		w->colormap = 0;
		XDestroyWindow(w->display, w->window);
		w->window = 0;
		XCloseDisplay(w->display);
		w->display = NULL;
	}
}

extern mfwWindow * mfwNewWindow(const char * restrict title, struct recti frame) {
	mfwWindow * w = NULL;
	if((w = emalloc(sizeof(mfwWindow)))) {
		mfwInitWindow(w, title, frame);
	}
	return w;
}

extern mfwWindow * mfwNewWindowFullscreen()
{
	mfwWindow * w = NULL;
	if((w = emalloc(sizeof(mfwWindow)))) {
		mfwInitWindowFullscreen(w);
	}
	return w;
}

extern void mfwDeleteWindow(mfwWindow * restrict w)
{
	if(!w) return;

	mfwDestroyWindow(w);
	free(w);
}

extern recti mfwWindow_getFrame(const mfwWindow * restrict w)
{
	assert(w);

	return w->frame;
}

enum {
	_NET_WM_STATE_REMOVE = 0,
	_NET_WM_STATE_ADD = 1,
	_NET_WM_STATE_TOGGLE = 2
};

extern void mfwWindow_setFullscreen(mfwWindow * restrict w, int fullscreen)
{
	assert(w);
	assert(w->display);
	assert(w->window);

	Atom state = XInternAtom(w->display, "_NET_WM_STATE", True);
	assert(state);

	Atom fs = XInternAtom(w->display, "_NET_WM_STATE_FULLSCREEN", True);
	assert(fs);

	XClientMessageEvent e;
	e.type = ClientMessage;
	e.display = w->display;
	e.window = w->window;
	e.message_type = state;
	e.format = 32;
	e.data.l[0] = fullscreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
	e.data.l[1] = fs;
	e.data.l[2] = 0;
	e.data.l[3] = 0;
	e.data.l[4] = 0;

	XSendEvent(
		w->display,
		w->window,
		True,
		SubstructureRedirectMask | SubstructureNotifyMask,
		(XEvent *)&e);
	XFlush(w->display);
}

extern void mfwWindow_bringToFront(mfwWindow * restrict w)
{
	assert(w);
	assert(w->display);
	assert(w->window);

	XWindowChanges values[] = { Above };
	XConfigureWindow(w->display, w->window, CWStackMode, values);
	XFlush(w->display);
}

extern void mfwWindow_update(mfwWindow * restrict w) { }

#endif