//
//  event.h
//  Mana
//
//  Created by Matthew Hinkle on 2/20/14.
//  Copyright (c) 2014 Matthew Hinkle. All rights reserved.
//

#ifndef __Mana__event__
#define __Mana__event__

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Common/emalloc.h"

#include "input.h"

struct mfwWindow;
union mfwEvent;

static const unsigned Event_None = 0;
static const unsigned Event_Any = 1 << 0;
static const unsigned Event_KeyPress = 1 << 1;
static const unsigned Event_KeyRelease = 1 << 2;
static const unsigned Event_MouseMotion = 1 << 3;
static const unsigned Event_MousePress = 1 << 4;
static const unsigned Event_MouseRelease = 1 << 5;
static const unsigned Event_MouseScroll = 1 << 6;
static const unsigned Event_WindowCreate = 1 << 7;
static const unsigned Event_WindowExpose = 1 << 8;
static const unsigned Event_WindowConfigure = 1 << 9;
static const unsigned Event_WindowExit = 1 << 10;
static const unsigned Event_Exit = 1 << 11;
static const unsigned Event_Unknown = 1 << 12;

/*
 * No event
 * --
 * EventType_NoEvent is returned in the absense of events.
 */
typedef struct {
	unsigned type;	/* Event_None */
	uint64_t timestamp;
} mfwNoEvent;

extern union mfwEvent mfwMakeNoEvent();

/*
 * Any event
 * --
 * Represents any event.
 */
typedef struct {
	unsigned type;	/* Event_Any */
	uint64_t timestamp;
} mfwAnyEvent;

/*
 * Key event
 * --
 * Keyboard press and release events.
 */
typedef struct {
	unsigned type; /* Event_KeyPress */
	uint64_t timestamp;
	mfwKey key;
	mfwMod modifiers;
	int32_t x;
	int32_t y;
	struct mfwWindow * window;
} mfwKeyPressEvent;

typedef struct {
	unsigned type;	/* Event_KeyRelease */
	uint64_t timestamp;
	mfwKey key;
	mfwMod modifiers;
	int32_t x;
	int32_t y;
	struct mfwWindow * window;
} mfwKeyReleaseEvent;

extern union mfwEvent mfwMakeKeyPressEvent(mfwKey key, mfwMod modifiers, int32_t x, int32_t y, struct mfwWindow * restrict window, uint64_t timestamp);
extern union mfwEvent mfwMakeKeyReleaseEvent(mfwKey key, mfwMod modifiers, int32_t x, int32_t y, struct mfwWindow * restrict window, uint64_t timestamp);

/*
 * Mouse event
 * --
 * Mouse motion and press/release events.
 */
typedef struct {
	unsigned type;	/* Event_MouseMotion */
	uint64_t timestamp;
	int32_t x;
	int32_t y;
	mfwMod modifiers;
	struct mfwWindow * window;
} mfwMouseMotionEvent;

typedef struct {
	unsigned type;	/* Event_MousePress */
	uint64_t timestamp;
	int32_t x;
	int32_t y;
	mfwMod modifiers;
	mfwButton button;
	struct mfwWindow * window;
} mfwMousePressEvent;

typedef struct {
	unsigned type;	/* Event_MouseRelease */
	uint64_t timestamp;
	int32_t x;
	int32_t y;
	mfwMod modifiers;
	mfwButton button;
	struct mfwWindow * window;
} mfwMouseReleaseEvent;

typedef struct {
	unsigned type;	/* Event_MouseScroll */
	uint64_t timestamp;
	int xDelta;
	int yDelta;
	int zDelta;
	mfwMod modifiers;
	struct mfwWindow * window;
} mfwMouseScrollEvent;

extern union mfwEvent mfwMakeMouseMotionEvent(int32_t x, int32_t y, mfwMod modifiers, struct mfwWindow * restrict window, uint64_t timestamp);
extern union mfwEvent mfwMakeMousePressEvent(int32_t x, int32_t y, mfwMod modifiers, mfwButton button, struct mfwWindow * restrict window, uint64_t timestamp);
extern union mfwEvent mfwMakeMouseReleaseEvent(int32_t x, int32_t y, mfwMod modifiers, mfwButton button, struct mfwWindow * restrict window, uint64_t timestamp);
extern union mfwEvent mfwMakeMouseScrollEvent(mfwMod modifiers, int xDelta, int yDelta, int zDelta, struct mfwWindow * restrict window, uint64_t timestamp);

/*
 * Window event
 * --
 * Window lifetime and configuration events.
 */
typedef struct {
	unsigned type;	/* Event_WindowCreate */
	struct mfwWindow * window;
} mfwWindowCreateEvent;

typedef struct {
	unsigned type;	/* Event_WindowExpose */
	struct mfwWindow * window;
} mfwWindowExposeEvent;

typedef struct {
	unsigned type;	/* Event_WindowMove */
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
	struct mfwWindow * window;
} mfwWindowConfigureEvent;

typedef struct {
	unsigned type;	/* Event_WindowExit */
	struct mfwWindow * window;
} mfwWindowExitEvent;

extern union mfwEvent mfwMakeWindowCreateEvent(struct mfwWindow * restrict window);
extern union mfwEvent mfwMakeWindowExposeEvent(struct mfwWindow * restrict window);
extern union mfwEvent mfwMakeWindowConfigureEvent(int32_t x, int32_t y, int32_t width, int32_t height, struct mfwWindow * restrict window);
extern union mfwEvent mfwMakeWindowExitEvent(struct mfwWindow * restrict window);

/*
 * Exit event
 * --
 * Event sent when a window is exiting.
 */
typedef struct {
	unsigned type;	/* Event_Exit */
	uint64_t timestamp;
} mfwExitEvent;

extern union mfwEvent mfwMakeExitEvent(uint64_t timestamp);

/*
 * Unknown event
 * --
 * Event_Unkown is returned when an unhandled
 * event is encountered.
 */
typedef struct {
	unsigned type;	/* Event_Unknown */
	uint64_t timestamp;
} mfwUnknownEvent;

extern union mfwEvent mfwMakeUnknownEvent();

typedef union mfwEvent {
	unsigned type;
	mfwNoEvent noEvent;
	mfwAnyEvent anyEvent;
	mfwKeyPressEvent keyPress;
	mfwKeyReleaseEvent keyRelease;
	mfwMouseMotionEvent mouseMotion;
	mfwMousePressEvent mousePress;
	mfwMouseReleaseEvent mouseRelease;
	mfwMouseScrollEvent mouseScroll;
	mfwWindowCreateEvent windowCreate;
	mfwWindowExposeEvent windowExpose;
	mfwWindowConfigureEvent windowConfigure;
	mfwWindowExitEvent windowExit;
	mfwExitEvent exit;
	mfwUnknownEvent unknown;
} mfwEvent;

/*
 * Event queue
 * --
 * Non-thread safe event queue for internal use.
 */
struct mfwEventQueueNode {
	mfwEvent event;
	struct mfwEventQueueNode * next;
};

struct mfwEventQueue {
	struct mfwEventQueueNode * head;
	struct mfwEventQueueNode * tail;
};

extern void mfwInitEventQueue(struct mfwEventQueue * restrict q);
extern void mfwDestroyEventQueue(struct mfwEventQueue * restrict q);

extern struct mfwEventQueue * mfwNewEventQueue();
extern void mfwDeleteEventQueue(struct mfwEventQueue * q);

extern mfwEvent mfwEventQueue_head(const struct mfwEventQueue * restrict q);
extern void mfwEventQueue_push(struct mfwEventQueue * restrict q, mfwEvent event);
extern mfwEvent mfwEventQueue_pop(struct mfwEventQueue * restrict q);
extern int mfwEventQueue_isEmpty(const struct mfwEventQueue * const restrict q);

extern int mfwEvent_sharedQueue_next(mfwEvent * event);
extern int mfwEvent_sharedQueue_peek(mfwEvent * event);
extern void mfwEvent_sharedQueue_push(mfwEvent event);

extern int mfwEvent_next(struct mfwWindow * window, mfwEvent * event);
extern int mfwEvent_peek(struct mfwWindow * window, mfwEvent * event);

#endif /* defined(__Mana__event__) */
