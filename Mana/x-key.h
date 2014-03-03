//
//  x-key.h
//  Mana
//
//  Created by Matthew Hinkle on 2/28/14.
//  Copyright (c) 2014 Matthew Hinkle. All rights reserved.
//

#ifndef __Mana__x_key__
#define __Mana__x_key__

#if defined (__unix__) || defined (__linux__) || (defined (__APPLE__) && defined (__MACH__) && defined (MANA_USE_X11))

#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xlib.h>

#include "input.h"

extern mfwKey makeKey(long key);

#endif /* defined(__Mana__x_key__) */

#endif
