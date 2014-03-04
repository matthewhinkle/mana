//
//  xcb-window-h.cpp
//  Mana
//
//  Created by Matthew Hinkle on 11/18/13.
//  Copyright (c) 2013 Matthew Hinkle. All rights reserved.
//

#if defined (__unix__) || defined (__linux__) || (defined (__APPLE__) && defined (__MACH__) && defined (MANA_USE_X11) && defined (MANA_USE_X11_XCB))

#include "xcb-window.h"

/* GLContext X11-XCB impl */

extern void mfwInitGLContext(mfwGLContext * restrict c, mfwWindow * restrict w)
{
	assert(c);
	assert(w);
	assert(w->display);
	assert(w->fbConfig);
	assert(w->window);

	c->display = w->display;
	c->drawable = w->window;
	c->native = glXCreateNewContext(c->display, w->fbConfig, GLX_RGBA_TYPE, 0, True);
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
	assert(w);

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

/* Window X11-XCB impl */

static const uint8_t PROPERTY_FORMAT_SIZE_8 = 8;
static const uint8_t PROPERTY_FORMAT_SIZE_32 = 32;
static const uint32_t MAX_TITLE_LEN = 128;

extern void mfwWindow_setTitle(mfwWindow * restrict w, const char * restrict title)
{
	assert(w);

	xcb_change_property(
		w->xcb,
		XCB_PROP_MODE_REPLACE,
		w->window,
		XCB_ATOM_WM_NAME,
		XCB_ATOM_STRING,
		PROPERTY_FORMAT_SIZE_8,
		MAX_TITLE_LEN,
		title
	);
	xcb_flush(w->xcb);
}

static xcb_screen_t * get_screen(mfwWindow * restrict w)
{
	xcb_screen_iterator_t i = xcb_setup_roots_iterator(xcb_get_setup(w->xcb));
	for(int n = w->defaultScreen; i.rem && n > 0; n--, xcb_screen_next(&i));
	return i.data;
}

static GLXFBConfig get_fbConfig(mfwWindow * restrict w)
{
	static const int FB_ATTR[] = {
		GLX_DOUBLEBUFFER, True,
		None
	};

	int nitems = 0;
	const GLXFBConfig * configs = glXChooseFBConfig(w->display, w->defaultScreen, FB_ATTR, &nitems);
	assert(configs);
	assert(nitems > 0);
	return *configs;
}

static const uint32_t EVENT_MASK = XCB_EVENT_MASK_EXPOSURE
                                 | XCB_EVENT_MASK_KEY_PRESS
								 | XCB_EVENT_MASK_KEY_RELEASE
								 | XCB_EVENT_MASK_BUTTON_PRESS
								 | XCB_EVENT_MASK_BUTTON_RELEASE
								 | XCB_EVENT_MASK_BUTTON_MOTION;
static const uint32_t VALUE_MASK = XCB_CW_EVENT_MASK
                                 | XCB_CW_COLORMAP;

static void initWindow(mfwWindow * restrict w, const recti * restrict frame)
{
	w->xcb = XGetXCBConnection(w->display);
	assert(w->xcb);

	XSetEventQueueOwner(w->display, XCBOwnsEventQueue);

	w->screen = get_screen(w);
	assert(w->screen);

	GLXFBConfig fbConfig = get_fbConfig(w);
	assert(fbConfig);
	w->fbConfig = fbConfig;

	int visualId = 0;
	glXGetFBConfigAttrib(w->display, fbConfig, GLX_VISUAL_ID, &visualId);

	GLXContext context = glXCreateNewContext(w->display, fbConfig, GLX_RGBA_TYPE, 0, True);
	assert(context);

	w->window = xcb_generate_id(w->xcb);
	xcb_colormap_t colormap = xcb_generate_id(w->xcb);
	xcb_create_colormap(w->xcb, XCB_COLORMAP_ALLOC_NONE, colormap, w->screen->root, visualId);

	recti bounds;
	recti_init(
		&bounds,
		0,	/* set to upper left corner of parent */
		0,
		frame ? frame->width : w->screen->width_in_pixels,
		frame ? frame->height : w->screen->height_in_pixels
	);
	w->frame = frame ? *frame : bounds;

	uint32_t values[] = { EVENT_MASK, colormap, 0 };
	xcb_create_window(
		w->xcb,
		XCB_COPY_FROM_PARENT,
		w->window,
		w->screen->root,
		bounds.x,
		bounds.y,
		bounds.width,
		bounds.height,
		0, /* no border */
		XCB_WINDOW_CLASS_INPUT_OUTPUT,
		visualId,
		VALUE_MASK,
		values
	);

	w->keysyms = xcb_key_symbols_alloc(w->xcb);
	assert(w->keysyms);
}

extern void mfwWindow_setFrame(mfwWindow * restrict w, recti frame)
{
	assert(w);

	const uint32_t values[] = { frame.x, frame.y, frame.width, frame.height };

	xcb_configure_window(
		w->xcb,
		w->window,
		XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
		values
	);
	xcb_flush(w->xcb);
}

extern void mfwInitWindow(mfwWindow * restrict w, const char * restrict title, recti frame)
{
	assert(w);

	w->display = XOpenDisplay(NULL);
	assert(w->display);
	w->defaultScreen = XDefaultScreen(w->display);

	initWindow(w, &frame);
	xcb_map_window(w->xcb, w->window);

	if(title) {
		mfwWindow_setTitle(w, title);
	}

	mfwWindow_setFrame(w, frame);
}

enum {
	_NET_WM_STATE_REMOVE = 0,
	_NET_WM_STATE_ADD = 1,
	_NET_WM_STATE_TOGGLE = 2
};

static const char _NET_WM_STATE[] = "_NET_WM_STATE";
static const char _NET_WM_STATE_FULLSCREEN[] = "_NET_WM_STATE_FULLSCREEN";

static void mfwWindow_setFullscreen_internal(mfwWindow * restrict w) {
	assert(w);

	xcb_intern_atom_cookie_t state = xcb_intern_atom(w->xcb, 0, strlen(_NET_WM_STATE), _NET_WM_STATE);
	xcb_intern_atom_reply_t * sreply = xcb_intern_atom_reply(w->xcb, state, NULL);

	xcb_intern_atom_cookie_t fs = xcb_intern_atom(w->xcb, 0, strlen(_NET_WM_STATE_FULLSCREEN), _NET_WM_STATE_FULLSCREEN);
	xcb_intern_atom_reply_t * fsreply = xcb_intern_atom_reply(w->xcb, fs, NULL);

	xcb_change_property(
		w->xcb,
		XCB_PROP_MODE_REPLACE,
		w->window, sreply->atom,
		XCB_ATOM_ATOM,
		PROPERTY_FORMAT_SIZE_32,
		1,
		&fsreply->atom);
	xcb_flush(w->xcb);
}

extern void mfwInitWindowFullscreen(mfwWindow * restrict w)
{
	assert(w);

	w->display = XOpenDisplay(NULL);
	assert(w->display);
	w->defaultScreen = XDefaultScreen(w->display);

	initWindow(w, NULL);
	xcb_map_window(w->xcb, w->window);
	mfwWindow_setFullscreen_internal(w);
}

extern void mfwDestroyWindow(mfwWindow * restrict w)
{
	if(!w) return;

	if(w->display) {
		if(w->xcb) {
			xcb_destroy_window(w->xcb, w->window);
			w->xcb = NULL; /* XCloseDisplay() deallocates this */
		}

		if(w->keysyms) {
			free(w->keysyms);
			w->keysyms = NULL;
		}

		XCloseDisplay(w->display);
		w->display = NULL;
	}
}

extern mfwWindow * mfwNewWindow(recti frame)
{
	mfwWindow * w = NULL;
	if((w = emalloc(sizeof(mfwWindow)))) {
		mfwInitWindow(w, NULL, frame);
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

extern void mfwWindow_setFullscreen(mfwWindow * restrict w, int fullscreen)
{
	assert(w);
	assert(w->xcb);
	assert(w->window);

	xcb_intern_atom_cookie_t state = xcb_intern_atom(w->xcb, 0, strlen(_NET_WM_STATE), _NET_WM_STATE);
	xcb_intern_atom_reply_t * sreply = xcb_intern_atom_reply(w->xcb, state, NULL);

	xcb_intern_atom_cookie_t fs = xcb_intern_atom(w->xcb, 0, strlen(_NET_WM_STATE_FULLSCREEN), _NET_WM_STATE_FULLSCREEN);
	xcb_intern_atom_reply_t * fsreply = xcb_intern_atom_reply(w->xcb, fs, NULL);

	xcb_client_message_event_t e;
	e.response_type = XCB_CLIENT_MESSAGE;
	e.window = w->window;
	e.type = sreply->atom;
	e.format = 32;
	e.data.data32[0] = fullscreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
	e.data.data32[1] = fsreply->atom;
	e.data.data32[2] = XCB_ATOM_NONE;
	e.data.data32[3] = 0;
	e.data.data32[4] = 0;

	xcb_send_event(
		w->xcb,
		1,
		w->window,
		XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
		(const char *)&e
	);
	xcb_flush(w->xcb);
}

extern void mfwWindow_bringToFront(mfwWindow * restrict w)
{
	assert(w);
	assert(w->xcb);
	assert(w->window);

	const uint32_t values[] = { XCB_STACK_MODE_ABOVE };
	xcb_configure_window(w->xcb, w->window, XCB_CONFIG_WINDOW_STACK_MODE, values);
}

extern void mfwWindow_update(mfwWindow * restrict w) { }

#endif