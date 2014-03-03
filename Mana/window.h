//
//  window.h
//  Mana
//
//  Created by Matthew Hinkle on 11/16/13.
//  Copyright (c) 2013 Matthew Hinkle. All rights reserved.
//

#ifndef __Mana__window__
#define __Mana__window__

struct mfwWindow;
struct mfwGLContext;
struct recti;

extern void mfwInitWindow(struct mfwWindow * restrict, const char * title, struct recti frame);
extern void mfwInitWindowFullscreen(struct mfwWindow * restrict);
extern void mfwDestroyWindow(struct mfwWindow * restrict);

extern struct mfwWindow * mfwNewWindow(const char * title, struct recti frame);
extern struct mfwWindow * mfwNewWindowFullscreen();
extern void mfwDeleteWindow(struct mfwWindow * restrict);

extern struct recti mfwWindow_getFrame(const struct mfwWindow * restrict);

extern void mfwWindow_setFrame(struct mfwWindow * restrict, struct recti frame);
extern void mfwWindow_setFullscreen(struct mfwWindow * restrict, int fullscreen);
extern void mfwWindow_setTitle(struct mfwWindow * restrict, const char * title);

extern void mfwWindow_bringToFront(struct mfwWindow * restrict);
extern void mfwWindow_update(struct mfwWindow * restrict);

#include "window-incl.h"

#endif /* defined(__Mana__window__) */
