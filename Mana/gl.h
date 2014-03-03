//
//  gl.h
//  Mana
//
//  Created by Matthew Hinkle on 11/17/13.
//  Copyright (c) 2013 Matthew Hinkle. All rights reserved.
//

#ifndef __Mana__mfw_gl__
#define __Mana__mfw_gl__

/**
 * Platform agnostic OpenGL include
 */

#if defined (__APPLE__) && defined (__MACH__)
#	if defined (MANA_USE_X11)
#		define GL_GLEXT_LEGACY
#		include <GL/gl.h>
#	else
#		if !defined (GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED)
#			define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#		endif
#		include <OpenGL/gl3.h>
#	endif
#elif defined (__unix__) || defined (__linux__)
#	include <GL/gl.h>
#elif defined (_WIN32) || defined (_WIN64)
#   include <gl/gl.h>
#else
#   error "OpenGL platform target not supported"
#endif

#endif /* defined(__Mana__mfw_gl__) */
