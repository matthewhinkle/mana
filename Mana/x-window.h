//
//  x-window.h
//  Mana
//
//  Created by Matthew Hinkle on 12/7/13.
//  Copyright (c) 2013 Matthew Hinkle. All rights reserved.
//

#ifndef __Mana__x_window__
#define __Mana__x_window__

#if defined (__unix__) || defined (__linux__) || (defined (__APPLE__) && defined (__MACH__) && defined (MANA_USE_X11) && !defined (MANA_USE_X11_XCB))

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

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
	Window window;
	int screen;
	recti frame;
	Colormap colormap;
	XVisualInfo * vi;
} mfwWindow;

#endif

#endif /* defined(__Mana__x_window__) */
