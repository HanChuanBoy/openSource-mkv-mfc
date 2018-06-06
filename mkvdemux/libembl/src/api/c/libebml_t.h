/****************************************************************************
** LIBEBML : parse EBML files, see http://ebml.sourceforge.net/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Steve Lhomme.  All rights reserved.
**
** This file is part of LIBEBML.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding an other license may use this file in accordance with 
** the Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.matroska.org/license/qpl/ for QPL licensing information.
** See http://www.matroska.org/license/gpl/ for GPL licensing information.
**
** Contact license@matroska.org if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

/*!
    \file libebml_t.h
    \version \$Id: libebml_t.h,v 1.4 2003/05/27 11:12:23 mosu Exp $
    \author Steve Lhomme     <robux4 @ users.sf.net>
    \author Ingo Ralf Blum   <ingoralfblum @ users.sf.net>

    \brief Misc type definitions for the C API of LIBEBML

    \note These types should be compiler/language independant (just platform dependant)
    \todo recover the sized types (uint16, int32, etc) here too (or maybe here only)
*/

#ifndef _LIBEBML_T_H_INCLUDED_
#define _LIBEBML_T_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

// There are special implementations for certain platforms. For example on Windows
// we use the Win32 file API. here we set the appropriate macros.
#if defined(_WIN32)||defined(WIN32)
#define LIBEBML_WIN32
#else
#define LIBEBML_UNIX
#endif

// We assume, that the user uses the DLL by default. If he wants to use the static library, the LIBEBML_STATIC value
// has to be specified to the preprocessor
#ifdef LIBEBML_WIN32
	// Either LIBEBML_STATIC or LIBEBML_EXPORT should be defined here.
	#ifdef LIBEBML_STATIC
		#define LIBEBML_EXPORT
	#else
		#ifndef LIBEBML_EXPORT
			#define LIBEBML_EXPORT __declspec(dllimport) // This should work on Visual C++ and GCC (MinGW and Cygwin)
		#endif
	#endif // LIBEBML_STATIC
#else
	// On other platforms we usually don't need the dllimport and dllexport directives
	#define LIBEBML_EXPORT
#endif // LIBEBML_WIN32

// Changed char is unsigned now (signedness was causing trouble in endil)
#if defined(_WIN32) && !defined(__GNUC__)		// Microsoft Visual C++
    typedef signed __int64 int64;
    typedef signed __int32 int32;
    typedef signed __int16 int16;
    typedef signed __int8 int8;
    typedef __int8 character;
    typedef unsigned __int64 uint64;
    typedef unsigned __int32 uint32;
    typedef unsigned __int16 uint16;
    typedef unsigned __int8 uint8;
#else
#ifdef DJGPP				/* SL : DJGPP doesn't support POSIX types ???? */
    typedef signed long long int64;
    typedef signed long int32;
    typedef signed short int16;
    typedef signed char int8;
    typedef char character;
    typedef unsigned long long uint64;
    typedef unsigned long uint32;
    typedef unsigned short uint16;
    typedef unsigned char uint8;
#else	/* DJGPP -> anything else */
#include <sys/types.h>
#if defined(sun) && (defined(__svr4__) || defined(__SVR4)) // SOLARIS
    typedef uint64_t u_int64_t;
    typedef uint32_t u_int32_t;
    typedef uint16_t u_int16_t;
    typedef uint8_t  u_int8_t;
#endif // SOLARIS
    typedef int64_t int64;
    typedef int32_t int32;
    typedef int16_t int16;
    typedef int8_t int8;
    typedef int8_t character;
    typedef u_int64_t uint64;
    typedef u_int32_t uint32;
    typedef u_int16_t uint16;
    typedef u_int8_t uint8;
#endif /* anything else */
#endif /* _MSC_VER */

typedef uint8  binary;

typedef enum open_mode {
    MODE_READ,
    MODE_WRITE,
    MODE_CREATE,
    MODE_SAFE
} open_mode;

using namespace std;
#ifdef __cplusplus
}
#endif

#endif /* _LIBEBML_T_H_INCLUDED_ */
