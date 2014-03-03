//
//  cocoa-event.m
//  Mana
//
//  Created by Matthew Hinkle on 3/1/14.
//  Copyright (c) 2014 Matthew Hinkle. All rights reserved.
//

#if defined (__APPLE__) && defined (__MACH__) && !defined (MANA_USE_X11)

#import <Cocoa/Cocoa.h>

/* import for key code */
#import <Carbon/Carbon.h>

#include "event.h"
#include "input.h"
#include "window.h"

static inline mfwMod makeMod(NSUInteger state) {
	mfwMod mod = Mod_Nil;

	if(state & NSShiftKeyMask) mod |= Mod_Shift;
	if(state & NSControlKeyMask) mod |= Mod_Control;
	if(state & NSAlphaShiftKeyMask) mod |= Mod_Lock;

	if(state & NSAlternateKeyMask) mod |= Mod_Alt;
	if(state & NSNumericPadKeyMask) mod |= Mod_NumLock;
	if(state & NSCommandKeyMask) mod |= Mod_Mod;

	return mod;
}

static inline int isShift(mfwMod mod) {
	return (Mod_Shift & mod);
}

static inline mfwKey makeKey(unsigned key, mfwMod mod)
{
	switch(key) {
	case kVK_LeftArrow: return Key_Left;
	case kVK_UpArrow: return Key_Up;
	case kVK_RightArrow: return Key_Right;
	case kVK_DownArrow: return Key_Down;

	case kVK_Delete: return Key_Backspace;
	case kVK_Tab: return Key_Tab;
	case kVK_Return: return Key_Return;
	case kVK_Escape: return Key_Esc;
	case kVK_ForwardDelete: return Key_Delete;

	case kVK_ANSI_0: return isShift(mod) ? Key_RightParen : Key_0;
	case kVK_ANSI_1: return isShift(mod) ? Key_ExclamationPoint : Key_1;
	case kVK_ANSI_2: return isShift(mod) ? Key_At : Key_2;
	case kVK_ANSI_3: return isShift(mod) ? Key_NumberSign : Key_3;
	case kVK_ANSI_4: return isShift(mod) ? Key_Currency : Key_4;
	case kVK_ANSI_5: return isShift(mod) ? Key_Percent : Key_5;
	case kVK_ANSI_6: return Key_6;
	case kVK_ANSI_7: return isShift(mod) ? Key_Ampersand : Key_7;
	case kVK_ANSI_8: return isShift(mod) ? Key_Asterisk : Key_8;
	case kVK_ANSI_9: return isShift(mod) ? Key_LeftParen : Key_9;

	case kVK_ANSI_A: return isShift(mod) ? Key_A : Key_a;
	case kVK_ANSI_B: return isShift(mod) ? Key_B : Key_b;
	case kVK_ANSI_C: return isShift(mod) ? Key_C : Key_c;
	case kVK_ANSI_D: return isShift(mod) ? Key_D : Key_d;
	case kVK_ANSI_E: return isShift(mod) ? Key_E : Key_e;
	case kVK_ANSI_F: return isShift(mod) ? Key_F : Key_f;
	case kVK_ANSI_G: return isShift(mod) ? Key_G : Key_g;
	case kVK_ANSI_H: return isShift(mod) ? Key_H : Key_h;
	case kVK_ANSI_I: return isShift(mod) ? Key_I : Key_i;
	case kVK_ANSI_J: return isShift(mod) ? Key_J : Key_j;
	case kVK_ANSI_K: return isShift(mod) ? Key_K : Key_k;
	case kVK_ANSI_L: return isShift(mod) ? Key_L : Key_l;
	case kVK_ANSI_M: return isShift(mod) ? Key_M : Key_m;
	case kVK_ANSI_N: return isShift(mod) ? Key_N : Key_n;
	case kVK_ANSI_O: return isShift(mod) ? Key_O : Key_o;
	case kVK_ANSI_P: return isShift(mod) ? Key_P : Key_p;
	case kVK_ANSI_Q: return isShift(mod) ? Key_Q : Key_q;
	case kVK_ANSI_R: return isShift(mod) ? Key_R : Key_r;
	case kVK_ANSI_S: return isShift(mod) ? Key_S : Key_s;
	case kVK_ANSI_T: return isShift(mod) ? Key_T : Key_t;
	case kVK_ANSI_U: return isShift(mod) ? Key_U : Key_u;
	case kVK_ANSI_V: return isShift(mod) ? Key_V : Key_v;
	case kVK_ANSI_W: return isShift(mod) ? Key_W : Key_w;
	case kVK_ANSI_X: return isShift(mod) ? Key_X : Key_x;
	case kVK_ANSI_Y: return isShift(mod) ? Key_Y : Key_y;
	case kVK_ANSI_Z: return isShift(mod) ? Key_Z : Key_z;

	case kVK_ANSI_LeftBracket: return isShift(mod) ? Key_LeftBrace : Key_LeftBracket;
	case kVK_ANSI_RightBracket: return isShift(mod) ? Key_RightBrace : Key_RightBracket;
	case kVK_ANSI_Backslash: return isShift(mod) ? Key_Bar : Key_BackSlash;
	case kVK_ANSI_Grave: return isShift(mod) ? Key_Grave : Key_Tilde;

	case kVK_F1: return Key_F1;
	case kVK_F2: return Key_F2;
	case kVK_F3: return Key_F3;
	case kVK_F4: return Key_F4;
	case kVK_F5: return Key_F5;
	case kVK_F6: return Key_F6;
	case kVK_F7: return Key_F7;
	case kVK_F8: return Key_F8;
	case kVK_F9: return Key_F9;
	case kVK_F10: return Key_F10;
	case kVK_F11: return Key_F11;
	case kVK_F12: return Key_F12;

	case kVK_Shift: return Key_LeftShift | Key_RightShift;
	case kVK_Control: return Key_LeftControl | Key_RightControl;
	case kVK_CapsLock: return Key_CapsLock;
	case kVK_Option: return Key_LeftAlt | Key_RightAlt;
	case kVK_Command: return Key_LeftMeta | Key_RightMeta;

	case kVK_Space: return Key_Space;
	case kVK_ANSI_Equal: return isShift(mod) ? Key_Plus : Key_Equal;
	case kVK_ANSI_Minus: return isShift(mod) ? Key_Underscore : Key_Minus;
	case kVK_ANSI_Period: return isShift(mod) ? Key_Greater : Key_Period;
	case kVK_ANSI_Comma: return isShift(mod) ? Key_Less : Key_Comma;
	case kVK_ANSI_Slash: return isShift(mod) ? Key_QuestionMark : Key_Slash;

	case kVK_ANSI_Semicolon: return isShift(mod) ? Key_Colon : Key_Semicolon;
	case kVK_ANSI_Quote: return isShift(mod) ? Key_DoubleQuote : Key_LeftQuote | Key_RightQuote;

	case kVK_Home: return Key_Home;
	case kVK_PageDown: return Key_PageDown;
	case kVK_PageUp: return Key_PageUp;
	case kVK_End: return Key_End;

	default: return Key_Unknown;
	}
}

static inline void mapKeyPressEvent(struct mfwWindow * restrict window, NSEvent * const c, mfwEvent * restrict e)
{
	assert(window);
	const mfwMod mod = makeMod([c modifierFlags]);
	const mfwKey key = makeKey([c keyCode], mod);
	const NSPoint loc = [c locationInWindow];
	*e = mfwMakeKeyPressEvent(key, mod, (int32_t)loc.x, (int32_t)loc.y, window, (uint64_t)[c timestamp]);
}

static inline void mapKeyReleaseEvent(struct mfwWindow * restrict window, NSEvent * const c, mfwEvent * restrict e)
{
	assert(window);
	const mfwMod mod = makeMod([c modifierFlags]);
	const mfwKey key = makeKey([c keyCode], mod);
	const NSPoint loc = [c locationInWindow];
	*e = mfwMakeKeyReleaseEvent(key, mod, (int32_t)loc.x, (int32_t)loc.y, window, (uint64_t)[c timestamp]);
}

static inline void mapMousePressEvent(struct mfwWindow * restrict window, NSEvent * const c, mfwButton button, mfwEvent * restrict e)
{
	assert(window);
	const mfwMod mod = makeMod([c modifierFlags]);
	const NSPoint loc = [c locationInWindow];
	*e = mfwMakeMousePressEvent((int32_t)loc.x, (int32_t)loc.y, mod, button, window, (uint64_t)[c timestamp]);
}

static inline void mapMouseReleaseEvent(struct mfwWindow * restrict window, NSEvent * const c, mfwButton button, mfwEvent * restrict e)
{
	assert(window);
	const mfwMod mod = makeMod([c modifierFlags]);
	const NSPoint loc = [c locationInWindow];
	*e = mfwMakeMouseReleaseEvent((int32_t)loc.x, (int32_t)loc.y, mod, button, window, (uint64_t)[c timestamp]);
}

static inline void mapMouseScrollEvent(struct mfwWindow * restrict window, NSEvent * const c, mfwEvent * restrict e)
{
	assert(window);
	const mfwMod mod = makeMod([c modifierFlags]);
	*e = mfwMakeMouseScrollEvent(mod, (int)[c deltaX], (int)[c deltaZ], (int)[c deltaZ], window, (uint64_t)[c timestamp]);
}

static int mapEvent(struct mfwWindow * restrict window, NSEvent * const c, mfwEvent * restrict e)
{
	assert(window);

	if(!(c && e)) return 0;

	switch([c type]) {
	case NSKeyDown: mapKeyPressEvent(window, c, e); return 1;
	case NSKeyUp: mapKeyReleaseEvent(window, c, e); return 1;
	case NSLeftMouseDown: mapMousePressEvent(window, c, Mouse_Button1, e); return 1;
	case NSLeftMouseUp: mapMouseReleaseEvent(window, c, Mouse_Button1, e); return 1;
	case NSRightMouseDown: mapMousePressEvent(window, c, Mouse_Button2, e); return 1;
	case NSRightMouseUp: mapMouseReleaseEvent(window, c, Mouse_Button2, e); return 1;
	case NSOtherMouseDown: mapMousePressEvent(window, c, Mouse_Button3, e); return 1;
	case NSOtherMouseUp: mapMouseReleaseEvent(window, c, Mouse_Button3, e); return 1;
	case NSScrollWheel: mapMouseScrollEvent(window, c, e); return 1;
	default: *e = mfwMakeUnknownEvent(); return 0;
	}
}

static const NSUInteger EVENT_MASK = NSLeftMouseDownMask
								   | NSLeftMouseUpMask
								   | NSRightMouseDownMask
								   | NSRightMouseUpMask
								   | NSMouseMovedMask
								   | NSLeftMouseDraggedMask
								   | NSRightMouseDraggedMask
								   | NSMouseEnteredMask
								   | NSMouseExitedMask
								   | NSKeyDownMask
								   | NSKeyUpMask
								   | NSFlagsChangedMask
								   | NSCursorUpdateMask
								   | NSScrollWheelMask
								   | NSOtherMouseDownMask
								   | NSOtherMouseUpMask
								   | NSOtherMouseDraggedMask;

extern int mfwEvent_next(struct mfwWindow * window, mfwEvent * event)
{
	assert(window);

	if(mfwEvent_sharedQueue_next(event)) return 1;

	NSEvent * const e = [[NSApplication sharedApplication] nextEventMatchingMask:EVENT_MASK untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
	if(e) {
		mapEvent(window, e, event);
		return 1;
	}

	return 0;
}

extern int mfwEvent_peek(struct mfwWindow * window, mfwEvent * event)
{
	assert(window);

	if(mfwEvent_sharedQueue_peek(event)) return 1;

	NSEvent * const e = [[NSApplication sharedApplication] nextEventMatchingMask:EVENT_MASK untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:NO];
	if(e) {
		mapEvent(window, e, event);
		return 1;
	}

	return 0;
}

#endif
