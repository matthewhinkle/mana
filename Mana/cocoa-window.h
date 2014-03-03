//
//  cocoa-window.h
//  Mana
//
//  Created by Matthew Hinkle on 2/8/14.
//  Copyright (c) 2014 Matthew Hinkle. All rights reserved.
//

#ifndef __Mana__cocoa_window__
#define __Mana__cocoa_window__

#if defined (__APPLE__) && defined (__MACH__) && !defined (MANA_USE_X11)

#import <Cocoa/Cocoa.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <Common/emalloc.h>
#include <Common/rects.h>

#include "gl-context.h"

typedef struct mfwGLContext {
	const void * nativeView;
} mfwGLContext;

typedef struct mfwWindow {
	const void * nativeWindow;
} mfwWindow;

#endif

#endif /* defined(__Mana_cocoa_window__) */
