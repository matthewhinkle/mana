//
//  xcb-event.c
//  Mana
//
//  Created by Matthew Hinkle on 3/1/14.
//  Copyright (c) 2014 Matthew Hinkle. All rights reserved.
//

#if defined (__unix__) || defined (__linux__) || (defined (__APPLE__) && defined (__MACH__) && defined (MANA_USE_X11) && defined (MANA_USE_X11_XCB))

#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

#include "event.h"
#include "input.h"
#include "window.h"
#include "x-key.h"

static inline mfwMod makeMod(uint16_t state) {
	mfwMod mod = Mod_Nil;

	if(state & XCB_MOD_MASK_SHIFT) mod |= Mod_Shift;
	if(state & XCB_MOD_MASK_CONTROL) mod |= Mod_Control;
	if(state & XCB_MOD_MASK_LOCK) mod |= Mod_Lock;

	if(state & XCB_MOD_MASK_1) mod |= Mod_Alt;
	if(state & XCB_MOD_MASK_2) mod |= Mod_NumLock;
	if(state & XCB_MOD_MASK_3) mod |= Mod_AltGr;
	if(state & XCB_MOD_MASK_4) mod |= Mod_Mod;
	if(state & XCB_MOD_MASK_5) mod |= Mod_ScrollLock;

	if(state & XCB_BUTTON_MASK_1) mod |= Mod_MouseButton1;
	if(state & XCB_BUTTON_MASK_2) mod |= Mod_MouseButton2;
	if(state & XCB_BUTTON_MASK_3) mod |= Mod_MouseButton3;
	if(state & XCB_BUTTON_MASK_4) mod |= Mod_MouseButton4;
	if(state & XCB_BUTTON_MASK_5) mod |= Mod_MouseButton5;

	return mod;
}

static inline mfwButton makeButton(xcb_button_t button) {
	switch(button) {
	case XCB_BUTTON_INDEX_1: return Mouse_Button1;
	case XCB_BUTTON_INDEX_2: return Mouse_Button2;
	case XCB_BUTTON_INDEX_3: return Mouse_Button3;
	case XCB_BUTTON_INDEX_4: return Mouse_Button4;
	case XCB_BUTTON_INDEX_5: return Mouse_Button5;
	default: return Mouse_Unknown;
	}
}

static inline void mapKeyPressEvent(struct mfwWindow * restrict window, xcb_key_press_event_t * restrict x, mfwEvent * restrict e) {
	const xcb_keysym_t sym = xcb_key_press_lookup_keysym(window->keysyms, x, 0);
	const mfwKey key = makeKey((unsigned)sym);
	const mfwMod mod = makeMod(x->state);
	*e = mfwMakeKeyPressEvent(key, mod, x->event_x, x->event_y, window, (uint64_t)x->time);
}

static const int MOUSE_SCROLL_UP = 1;
static const int MOUSE_SCROLL_DOWN = -1;

static inline void mapMouseScrollEvent(struct mfwWindow * restrict window, xcb_button_press_event_t * restrict x, mfwButton button, mfwEvent * restrict e) {
	const mfwMod mod = makeMod(x->state);
	if(Mouse_Button3 & button) {
		*e = mfwMakeMouseScrollEvent(mod, 0, MOUSE_SCROLL_UP, 0, window, (uint64_t)x->time);
	} else if(Mouse_Button4 & button) {
		*e = mfwMakeMouseScrollEvent(mod, 0, MOUSE_SCROLL_DOWN, 0, window, (uint64_t)x->time);
	} else {
		*e = mfwMakeUnknownEvent();
	}
}

static inline void mapMousePressEvent(struct mfwWindow * restrict window, xcb_button_press_event_t * restrict x, mfwEvent * restrict e) {
	const mfwButton button = makeButton(x->detail);
	if(Mouse_Button4 & button || Mouse_Button5 & button) {
		mapMouseScrollEvent(window, x, button, e);
	} else {
		const mfwMod mod = makeMod(x->state);
		*e = mfwMakeMousePressEvent(x->event_x, x->event_y, mod, button, window, (uint64_t)x->time);
	}
}

static inline void mapMouseReleaseEvent(struct mfwWindow * restrict window, xcb_button_release_event_t * restrict x, mfwEvent * restrict e) {
	const mfwButton button = makeButton(x->detail);
	if(Mouse_Button4 & button || Mouse_Button5 & button) return;
	const mfwMod mod = makeMod(x->state);
	*e = mfwMakeMouseReleaseEvent(x->event_x, x->event_y, mod, button, window, (uint64_t)x->time);
}

static int mapEvent(struct mfwWindow * restrict window, xcb_generic_event_t * restrict x, mfwEvent * restrict e)
{
	assert(window);

	if(!(x && e)) return 0;

	switch(x->response_type & ~0x80) {
	case XCB_KEY_RELEASE: /* same as key press */
	case XCB_KEY_PRESS: mapKeyPressEvent(window, (xcb_key_press_event_t *)x, e); return 1;
	case XCB_BUTTON_PRESS: mapMousePressEvent(window, (xcb_button_press_event_t *)x, e); return 1;
	case XCB_BUTTON_RELEASE: mapMouseReleaseEvent(window, (xcb_button_release_event_t *)x, e); return 1;
	default: *e = mfwMakeUnknownEvent(); return 0;
	}
}

extern int mfwEvent_next(struct mfwWindow * window, mfwEvent * event)
{
	assert(window);
	assert(window->window);
	assert(window->xcb);

	if(mfwEvent_sharedQueue_next(event)) return 1;

	xcb_generic_event_t * e = NULL;
	if((e = xcb_poll_for_event(window->xcb))) {
		mapEvent(window, e, event);
		free(e);
		return 1;
	}

	return 0;
}

extern int mfwEvent_peek(struct mfwWindow * window, mfwEvent * event)
{
	assert(window);

	if(mfwEvent_sharedQueue_peek(event)) return 1;

	return 0;
}

#endif