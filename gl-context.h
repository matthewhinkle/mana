//
//  gl-context.h
//  Mana
//
//  Created by Matthew Hinkle on 11/17/13.
//  Copyright (c) 2013 Matthew Hinkle. All rights reserved.
//

#ifndef __Mana__gl_context__
#define __Mana__gl_context__

struct mfwGLContext;
struct mfwWindow;

extern void mfwInitGLContext(struct mfwGLContext * restrict, struct mfwWindow * restrict window);
extern void mfwDestroyGLContext(struct mfwGLContext * restrict);

extern struct mfwGLContext * mfwNewGLContext(struct mfwWindow * window);
extern void mfwDeleteGLContext(struct mfwGLContext * restrict);

extern const void * mfwGLContext_getNativeContext(struct mfwGLContext * restrict);
extern void mfwGLContext_makeCurrent(struct mfwGLContext * restrict);
extern void mfwGLContext_swapBuffers(struct mfwGLContext * restrict);
extern void mfwGLContext_lock(struct mfwGLContext * restrict);
extern void mfwGLContext_unlock(struct mfwGLContext * restrict);

#endif /* defined(__Mana__gl_context__) */
