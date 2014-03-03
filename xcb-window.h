//
//  xcb-window-h.h
//  Mana
//
//  Created by Matthew Hinkle on 11/18/13.
//  Copyright (c) 2013 Matthew Hinkle. All rights reserved.
//

#ifndef __Mana__xcb_window_h__
#define __Mana__xcb_window_h__

#if defined (__unix__) || defined (__linux__) || (defined (__APPLE__) && defined (__MACH__) && defined (MANA_USE_X11) && defined (MANA_USE_X11_XCB))

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

#include "gl.h"
#include "gl-context.h"
#include <GL/glx.h>

#include "Common/emalloc.h"
#include "Common/rects.h"

typedef struct mfwGLContext {
	Display * display;
	Drawable drawable;
	GLXContext native;
} mfwGLContext;

typedef struct mfwWindow {
	Display * display;
	int defaultScreen;

	xcb_connection_t * xcb;
	xcb_screen_t * screen;
	xcb_window_t window;
	GLXFBConfig fbConfig;

	xcb_key_symbols_t * keysyms;

	recti frame;
} mfwWindow;

#endif

#endif /* defined(__Mana__x_window_h__) */
