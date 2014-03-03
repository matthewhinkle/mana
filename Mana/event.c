//
//  event.cpp
//  Mana
//
//  Created by Matthew Hinkle on 2/21/14.
//  Copyright (c) 2014 Matthew Hinkle. All rights reserved.
//

#include "event.h"

#include <stdint.h>

extern mfwEvent mfwMakeNoEvent()
{
	mfwNoEvent noEvent;
	noEvent.type = Event_None;
	noEvent.timestamp = 0;

	mfwEvent e;
	e.noEvent = noEvent;
	return e;
}

extern mfwEvent mfwMakeKeyPressEvent(mfwKey key, mfwMod modifiers, int32_t x, int32_t y, struct mfwWindow * window, uint64_t timestamp)
{
	mfwKeyPressEvent keyPress;
	keyPress.type = Event_KeyPress;
	keyPress.timestamp = timestamp;

	keyPress.key = key;
	keyPress.modifiers = modifiers;
	keyPress.x = x;
	keyPress.y = y;
	keyPress.window = window;

	mfwEvent e;
	e.keyPress = keyPress;
	return e;
}

extern mfwEvent mfwMakeKeyReleaseEvent(mfwKey key, mfwMod modifiers, int32_t x, int32_t y, struct mfwWindow * window, uint64_t timestamp)
{
	mfwKeyReleaseEvent keyRelease;
	keyRelease.type = Event_KeyRelease;
	keyRelease.timestamp = timestamp;

	keyRelease.key = key;
	keyRelease.modifiers = modifiers;
	keyRelease.x = x;
	keyRelease.y = y;
	keyRelease.window = window;

	mfwEvent e;
	e.keyRelease = keyRelease;
	return e;
}

extern mfwEvent mfwMakeMousePressEvent(int32_t x, int32_t y, mfwMod modifiers, mfwButton button, struct mfwWindow * window, uint64_t timestamp)
{
	mfwMousePressEvent mousePress;
	mousePress.type = Event_MousePress;
	mousePress.timestamp = timestamp;

	mousePress.x = x;
	mousePress.y = y;
	mousePress.modifiers = modifiers;
	mousePress.button = button;
	mousePress.window = window;

	mfwEvent e;
	e.mousePress = mousePress;
	return e;
}

extern mfwEvent mfwMakeMouseReleaseEvent(int32_t x, int32_t y, mfwMod modifiers, mfwButton button, struct mfwWindow * window, uint64_t timestamp)
{
	mfwMouseReleaseEvent mouseRelease;
	mouseRelease.type = Event_MouseRelease;
	mouseRelease.timestamp = timestamp;

	mouseRelease.x = x;
	mouseRelease.y = y;
	mouseRelease.modifiers = modifiers;
	mouseRelease.button = button;
	mouseRelease.window = window;

	mfwEvent e;
	e.mouseRelease = mouseRelease;
	return e;
}

extern mfwEvent mfwMakeMouseScrollEvent(mfwMod modifiers, int xDelta, int yDelta, int zDelta, struct mfwWindow * window, uint64_t timestamp)
{
	mfwMouseScrollEvent mouseScroll;
	mouseScroll.type = Event_MouseScroll;
	mouseScroll.timestamp = timestamp;

	mouseScroll.modifiers = modifiers;
	mouseScroll.xDelta = xDelta;
	mouseScroll.yDelta = yDelta;
	mouseScroll.zDelta = zDelta;
	mouseScroll.window = window;

	mfwEvent e;
	e.mouseScroll = mouseScroll;
	return e;
}

extern mfwEvent mfwMakeExitEvent(struct mfwWindow * window, uint64_t timestamp)
{
	mfwExitEvent exit;
	exit.type = Event_Exit;
	exit.timestamp = timestamp;

	mfwEvent e;
	e.exit = exit;
	return e;
}

extern mfwEvent mfwMakeUnknownEvent()
{
	mfwUnknownEvent unknown;
	unknown.type = Event_Unknown;
	unknown.timestamp = 0;

	mfwEvent e;
	e.unknown = unknown;
	return e;
}

extern void mfwInitEventQueue(struct mfwEventQueue * q)
{
	assert(q);

	q->head = NULL;
	q->tail = NULL;
};

extern void mfwDestroyEventQueue(struct mfwEventQueue * q)
{
	if(!q) return;

	struct mfwEventQueueNode * i = q->head;
	while(i) {
		struct mfwEventQueueNode * tmp = i;
		i = i->next;
		free(tmp);
		tmp = NULL;
	}

	q->head = NULL;
	q->tail = NULL;
}

extern struct mfwEventQueue * mfwNewEventQueue()
{
	struct mfwEventQueue * q = NULL;
	if((q = emalloc(sizeof(struct mfwEventQueue)))) {
		mfwInitEventQueue(q);
	}
	return q;
}

extern void mfwDeleteEventQueue(struct mfwEventQueue * q)
{
	if(!q) return;

	mfwDestroyEventQueue(q);
	free(q);
}

extern void mfwEventQueue_push(struct mfwEventQueue * q, mfwEvent event)
{
	assert(q);

	struct mfwEventQueueNode * node = emalloc(sizeof(struct mfwEventQueueNode));
	node->event = event;
	node->next = NULL;

	if(q->tail) {
		q->tail->next = node;
		q->tail = node;
	} else {
		q->tail = q->head = node;
	}
}

extern mfwEvent mfwEventQueue_head(const struct mfwEventQueue * const q)
{
	assert(q);

	return q->head ? q->head->event : mfwMakeNoEvent();
}

extern mfwEvent mfwEventQueue_pop(struct mfwEventQueue * q)
{
	assert(q);

	struct mfwEventQueueNode * head = q->head;
	if(head) {
		const mfwEvent event = head->event;
		q->head = q->head->next;
		free(head);
		head = NULL;
		return event;
	} else {
		return mfwMakeNoEvent();
	}
}

extern int mfwEventQueue_isEmpty(const struct mfwEventQueue * const q)
{
	assert(q);

	return !q->head;
}

#define EVENT_QUEUE_STATIC_INITIALIZER { NULL, NULL };
static struct mfwEventQueue sharedEventQueue = EVENT_QUEUE_STATIC_INITIALIZER;
#undef EVENT_QUEUE_STATIC_INITIALIZER

extern int mfwEvent_sharedQueue_next(mfwEvent * event)
{
	if(mfwEventQueue_isEmpty(&sharedEventQueue)) {
		if(event) *event = mfwMakeNoEvent();
		return 0;
	} else {
		if(event) *event = mfwEventQueue_pop(&sharedEventQueue);
		return 1;
	}
}

extern int mfwEvent_sharedQueue_peek(mfwEvent * event)
{
	if(mfwEventQueue_isEmpty(&sharedEventQueue) ) {
		if(event) *event = mfwMakeNoEvent();
		return 0;
	} else {
		if(event) *event = mfwEventQueue_head(&sharedEventQueue);
		return 1;
	}
}

extern void mfwEvent_sharedQueue_push(mfwEvent event)
{
	mfwEventQueue_push(&sharedEventQueue, event);
}
