//
//  cocoa-window.m
//  Mana
//
//  Created by Matthew Hinkle on 2/8/14.
//  Copyright (c) 2014 Matthew Hinkle. All rights reserved.
//

#if defined (__APPLE__) && defined (__MACH__) && !defined (MANA_USE_X11)

#include "cocoa-window.h"

/* OpenGL View */

@interface MFWView : NSOpenGLView
@end

@implementation MFWView


/*
 * initialization
 */
- (id) init {
	return self = [super init];
}

- (id) initWithCoder:(NSCoder *)aDecoder {
	return self = [super initWithCoder:aDecoder];
}

- (id) initWithFrame:(NSRect)frameRect {
	return self = [super initWithFrame:frameRect];
}

- (id) initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format {
	return self = [super initWithFrame:frameRect pixelFormat:format];
}

/*
 * events
 */
- (BOOL) acceptsFirstResponder {
	return YES;
}

@end

/* GLContext Cocoa impl */

extern void mfwInitGLContext(mfwGLContext * c, mfwWindow * w)
{
	assert(c);
	assert(w);

	const NSOpenGLPixelFormatAttribute kPixelFormatAttributes[] = {
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFADepthSize, 32,
		0
	};

	MFWView * view = [[MFWView alloc] initWithFrame:[(__bridge NSWindow *)w->nativeWindow frame]
											 pixelFormat:[[NSOpenGLPixelFormat alloc]initWithAttributes:kPixelFormatAttributes]];
	c->nativeView = CFBridgingRetain(view);
	[(__bridge NSWindow *)w->nativeWindow setContentView:view];
	[(__bridge NSWindow *)w->nativeWindow flushWindow];
}

extern void mfwDestroyGLContext(mfwGLContext * c)
{
	if(!(c)) return;

	if(c->nativeView) {
		CFBridgingRelease(c->nativeView);
		c->nativeView = NULL;
	}
}

extern mfwGLContext * mfwNewGLContext(mfwWindow * w)
{
	assert(w);

	mfwGLContext * c = NULL;
	if((c = emalloc(sizeof(mfwGLContext)))) {
		mfwInitGLContext(c, w);
	}
	return c;
}

extern void mfwDeleteGLContext(mfwGLContext * c)
{
	if(!(c)) return;

	mfwDestroyGLContext(c);
	free(c);
}

extern const void * mfwGLContext_getNativeContext(mfwGLContext * c)
{
	assert(c);
	assert(c->nativeView);

	return (__bridge const void *)[(__bridge NSOpenGLView *)c->nativeView openGLContext];
}

extern void mfwGLContext_makeCurrent(mfwGLContext * c)
{
	assert(c);
	assert(c->nativeView);

	[[(__bridge NSOpenGLView *)c->nativeView openGLContext] makeCurrentContext];
}

extern void mfwGLContext_swapBuffers(mfwGLContext * c)
{
	assert(c);
	assert(c->nativeView);

	[[(__bridge NSOpenGLView *)c->nativeView openGLContext] flushBuffer];
}

extern void mfwGLContext_lock(mfwGLContext * c)
{
	assert(c);
	assert(c->nativeView);

	CGLLockContext([[(__bridge NSOpenGLView *)c->nativeView openGLContext] CGLContextObj]);
}

extern void mfwGLContext_unlock(mfwGLContext * c)
{
	assert(c);
	assert(c->nativeView);

	CGLUnlockContext([[(__bridge NSOpenGLView *)c->nativeView openGLContext] CGLContextObj]);
}

/* Window Cocoa impl */

static const size_t MAX_TITLE_SIZE = 256;

extern void mfwWindow_setTitle(mfwWindow * w, const char * title)
{
	char t[MAX_TITLE_SIZE];
	strncpy(t, title, MAX_TITLE_SIZE);

	[(__bridge NSWindow *)w->nativeWindow setTitle:[NSString stringWithUTF8String:t]];
}

extern void mfwInitWindow(mfwWindow * w, const char * title, recti frame)
{
	assert(w);
	memset(w, 0x0, sizeof(mfwWindow));

	const NSInteger styleMask = (NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask | (title ? NSTitledWindowMask : 0)) & ~(NSBorderlessWindowMask | NSFullScreenWindowMask);

	NSWindow * nativeWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(frame.x, frame.y, frame.width, frame.height)
														  styleMask:styleMask
															backing:NSBackingStoreBuffered
															  defer:NO];
	w->nativeWindow = CFBridgingRetain(nativeWindow);
	mfwWindow_setTitle(w, title);
	[nativeWindow setLevel:NSMainMenuWindowLevel + 1];
	[nativeWindow makeKeyAndOrderFront:nativeWindow];
}

extern void mfwInitWindowFullscreen(mfwWindow * w)
{
	assert(w);

	const NSInteger styleMask = (NSBorderlessWindowMask | NSFullScreenWindowMask) & ~(NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask | NSTitledWindowMask);

	NSWindow * nativeWindow = [[NSWindow alloc] initWithContentRect:[[NSScreen mainScreen] frame]
														  styleMask:styleMask
															backing:NSBackingStoreBuffered
															  defer:NO];
	w->nativeWindow = CFBridgingRetain(nativeWindow);
	[nativeWindow setLevel:NSMainMenuWindowLevel + 1];
	[nativeWindow makeKeyAndOrderFront:nativeWindow];
}

extern void mfwDestroyWindow(mfwWindow * w)
{
	if(!(w)) return;

	if(w->nativeWindow) {
		CFBridgingRelease(w->nativeWindow);
		w->nativeWindow = NULL;
	}
}

extern mfwWindow * mfwNewWindow(const char * title, recti frame)
{
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

extern void mfwDeleteWindow(mfwWindow * w)
{
	if(!(w)) return;

	mfwDestroyWindow(w);
	free(w);
}

extern recti mfwWindow_getFrame(const mfwWindow * w)
{
	assert(w);
	assert(w->nativeWindow);

	const NSRect frame = [(__bridge NSWindow *)w->nativeWindow frame];
	recti f;
	recti_init(&f, frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
	return f;
}

extern void mfwWindow_setFrame(mfwWindow * w, recti frame)
{
	assert(w);
	assert(w->nativeWindow);

	NSRect f = NSMakeRect(frame.x, frame.y, frame.width, frame.height);
	[(__bridge NSWindow *)w->nativeWindow setFrame:f display:YES animate:YES];
}

extern void mfwWindow_setFullscreen(mfwWindow * w, int fullscreen)
{
	assert(w);
}

extern void mfwWindow_bringToFront(mfwWindow * w)
{
	assert(w);
	assert(w->nativeWindow);

	NSWindow * nativeWindow = (__bridge NSWindow *)w->nativeWindow;
	[nativeWindow makeKeyAndOrderFront:nativeWindow];
}

extern void mfwWindow_update(mfwWindow * w)
{
	assert(w);
	assert(w->nativeWindow);

	[(__bridge NSWindow *)w->nativeWindow update];
}

#endif