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

static const uint16_t Event_None = 0;
static const uint16_t Event_Any = 1 << 0;
static const uint16_t Event_KeyPress = 1 << 1;
static const uint16_t Event_KeyRelease = 1 << 2;
static const uint16_t Event_MousePress = 1 << 3;
static const uint16_t Event_MouseRelease = 1 << 4;
static const uint16_t Event_MouseScroll = 1 << 5;
static const uint16_t Event_Exit = 1 << 6;
static const uint16_t Event_Unknown = 1 << 15;

/*
 * No event
 * --
 * EventType_NoEvent is returned in the absense of events.
 */
typedef struct {
	uint16_t type;	/* Event_None */
	uint64_t timestamp;
} mfwNoEvent;

extern union mfwEvent mfwMakeNoEvent();

/*
 * Any event
 * --
 * Represents any event.
 */
typedef struct {
	uint16_t type;	/* Event_Any */
	uint64_t timestamp;
} mfwAnyEvent;

/*
 * Key event
 * --
 * Keyboard press and release events.
 */
typedef struct {
	uint16_t type; /* Event_KeyPress */
	uint64_t timestamp;
	mfwKey key;
	mfwMod modifiers;
	int32_t x;
	int32_t y;
	struct mfwWindow * window;
} mfwKeyPressEvent;

typedef struct {
	uint16_t type;	/* Event_KeyRelease */
	uint64_t timestamp;
	mfwKey key;
	mfwMod modifiers;
	int32_t x;
	int32_t y;
	struct mfwWindow * window;
} mfwKeyReleaseEvent;

extern union mfwEvent mfwMakeKeyPressEvent(mfwKey key, mfwMod modifiers, int32_t x, int32_t y, struct mfwWindow * window, uint64_t timestamp);
extern union mfwEvent mfwMakeKeyReleaseEvent(mfwKey key, mfwMod modifiers, int32_t x, int32_t y, struct mfwWindow * window, uint64_t timestamp);

/*
 * Mouse event
 * --
 * Mouse press and release events.
 */
typedef struct {
	uint16_t type;	/* Event_MousePress */
	uint64_t timestamp;
	int32_t x;
	int32_t y;
	mfwMod modifiers;
	mfwButton button;
	struct mfwWindow * window;
} mfwMousePressEvent;

typedef struct {
	uint16_t type;	/* Event_MouseRelease */
	uint64_t timestamp;
	int32_t x;
	int32_t y;
	mfwMod modifiers;
	mfwButton button;
	struct mfwWindow * window;
} mfwMouseReleaseEvent;

typedef struct {
	uint16_t type;	/* Event_MouseScroll */
	uint64_t timestamp;
	int xDelta;
	int yDelta;
	int zDelta;
	mfwMod modifiers;
	struct mfwWindow * window;
} mfwMouseScrollEvent;

extern union mfwEvent mfwMakeMousePressEvent(int32_t x, int32_t y, mfwMod modifiers, mfwButton button, struct mfwWindow * window, uint64_t timestamp);
extern union mfwEvent mfwMakeMouseReleaseEvent(int32_t x, int32_t y, mfwMod modifiers, mfwButton button, struct mfwWindow * window, uint64_t timestamp);
extern union mfwEvent mfwMakeMouseScrollEvent(mfwMod modifiers, int xDelta, int yDelta, int zDelta, struct mfwWindow * window, uint64_t timestamp);

/*
 * Exit event
 * --
 * Event sent when a window is exiting.
 */
typedef struct {
	uint16_t type;	/* Event_Exit */
	uint64_t timestamp;
	struct mfwWindow * window;
} mfwExitEvent;

extern union mfwEvent mfwMakeExitEvent(struct mfwWindow * window, uint64_t timestamp);

/*
 * Unknown event
 * --
 * Event_Unkown is returned when an unhandled
 * event is encountered.
 */
typedef struct {
	uint16_t type;	/* Event_Unknown */
	uint64_t timestamp;
} mfwUnknownEvent;

extern union mfwEvent mfwMakeUnknownEvent();

typedef union mfwEvent {
	uint16_t type;
	mfwNoEvent noEvent;
	mfwAnyEvent anyEvent;
	mfwKeyPressEvent keyPress;
	mfwKeyReleaseEvent keyRelease;
	mfwMousePressEvent mousePress;
	mfwMouseReleaseEvent mouseRelease;
	mfwMouseScrollEvent mouseScroll;
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
