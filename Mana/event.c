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

extern mfwEvent mfwMakeKeyPressEvent(mfwKey key, mfwMod modifiers, int32_t x, int32_t y, struct mfwWindow * restrict window, uint64_t timestamp)
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

extern mfwEvent mfwMakeKeyReleaseEvent(mfwKey key, mfwMod modifiers, int32_t x, int32_t y, struct mfwWindow * restrict window, uint64_t timestamp)
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

extern mfwEvent mfwMakeMouseMotionEvent(int32_t x, int32_t y, mfwMod modifiers, struct mfwWindow * restrict window, uint64_t timestamp)
{
	mfwMouseMotionEvent mouseMotion;
	mouseMotion.type = Event_MouseMotion;
	mouseMotion.timestamp = timestamp;

	mouseMotion.x = x;
	mouseMotion.y = y;
	mouseMotion.window = window;

	mfwEvent e;
	e.mouseMotion = mouseMotion;
	return e;
}

extern mfwEvent mfwMakeMousePressEvent(int32_t x, int32_t y, mfwMod modifiers, mfwButton button, struct mfwWindow * restrict window, uint64_t timestamp)
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

extern mfwEvent mfwMakeMouseReleaseEvent(int32_t x, int32_t y, mfwMod modifiers, mfwButton button, struct mfwWindow * restrict window, uint64_t timestamp)
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

extern mfwEvent mfwMakeMouseScrollEvent(mfwMod modifiers, int xDelta, int yDelta, int zDelta, struct mfwWindow * restrict window, uint64_t timestamp)
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

extern mfwEvent mfwMakeWindowCreateEvent(struct mfwWindow * restrict window)
{
	mfwWindowCreateEvent windowCreate;
	windowCreate.type = Event_WindowCreate;

	windowCreate.window = window;

	mfwEvent e;
	e.windowCreate = windowCreate;
	return e;
}

extern mfwEvent mfwMakeWindowExposeEvent(struct mfwWindow * restrict window)
{
	mfwWindowExposeEvent windowExpose;
	windowExpose.type = Event_WindowExpose;

	windowExpose.window = window;

	mfwEvent e;
	e.windowExpose = windowExpose;
	return e;
}

extern mfwEvent mfwMakeWindowConfigureEvent(int32_t x, int32_t y, int32_t width, int32_t height, struct mfwWindow * restrict window)
{
	mfwWindowConfigureEvent windowConfigure;
	windowConfigure.type = Event_WindowConfigure;

	windowConfigure.x = x;
	windowConfigure.y = y;
	windowConfigure.width = width;
	windowConfigure.height = height;
	windowConfigure.window = window;

	mfwEvent e;
	e.windowConfigure = windowConfigure;
	return e;
}

extern mfwEvent mfwMakeWindowExitEvent(struct mfwWindow * restrict window)
{
	mfwWindowExitEvent windowExit;
	windowExit.type = Event_WindowExit;

	windowExit.window = window;

	mfwEvent e;
	e.windowExit = windowExit;
	return e;
}

extern mfwEvent mfwMakeExitEvent(uint64_t timestamp)
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

extern void mfwInitEventQueue(struct mfwEventQueue * restrict q)
{
	assert(q);

	q->head = NULL;
	q->tail = NULL;
};

extern void mfwDestroyEventQueue(struct mfwEventQueue * restrict q)
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

extern void mfwDeleteEventQueue(struct mfwEventQueue * restrict q)
{
	if(!q) return;

	mfwDestroyEventQueue(q);
	free(q);
}

extern void mfwEventQueue_push(struct mfwEventQueue * restrict q, mfwEvent event)
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

extern mfwEvent mfwEventQueue_head(const struct mfwEventQueue * restrict const q)
{
	assert(q);

	return q->head ? q->head->event : mfwMakeNoEvent();
}

extern mfwEvent mfwEventQueue_pop(struct mfwEventQueue * restrict q)
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

extern int mfwEventQueue_isEmpty(const struct mfwEventQueue * restrict const q)
{
	assert(q);

	return !q->head;
}

#define EVENT_QUEUE_STATIC_INITIALIZER { NULL, NULL };
static struct mfwEventQueue sharedEventQueue = EVENT_QUEUE_STATIC_INITIALIZER;
#undef EVENT_QUEUE_STATIC_INITIALIZER

extern int mfwEvent_sharedQueue_next(mfwEvent * restrict event)
{
	if(mfwEventQueue_isEmpty(&sharedEventQueue)) {
		if(event) *event = mfwMakeNoEvent();
		return 0;
	} else {
		if(event) *event = mfwEventQueue_pop(&sharedEventQueue);
		return 1;
	}
}

extern int mfwEvent_sharedQueue_peek(mfwEvent * restrict event)
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
