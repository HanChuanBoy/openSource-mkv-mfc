/****************************************************************************
** libebml : parse EBML files, see http://embl.sourceforge.net/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Ingo Ralf Blum.  All rights reserved.
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
	\file
	\version \$Id: EbmlConfig.h,v 1.3 2003/05/27 16:56:17 mosu Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Moritz Bunkus <moritz @ bunkus.org>
*/

#ifndef LIBEBML_CONFIG_H
#define LIBEBML_CONFIG_H

#ifndef BIG_ENDIAN
#define LIL_ENDIAN 0x0102
#define BIG_ENDIAN 0x0201
#endif
#include <string.h>
// automatic endianess detection working on GCC
#if !defined(WORDS_BIGENDIAN)
#if (defined (__arm__) && ! defined (__ARMEB__)) || defined (__i386__) || defined (__i860__) || defined (__ns32000__) || defined (__vax__)
#define WORDS_BIGENDIAN 0 // for my testing platform (x86)
#elif defined (__sparc__) || defined (__alpha__) || defined (__PPC__) || defined (__mips__)
#define WORDS_BIGENDIAN 1 // for my testing platform (x86)
#else
// not automatically detected, put it yourself
#define WORDS_BIGENDIAN 0 // for my testing platform (x86)
#endif
#endif // not autoconf

// We allow the user to set the namespace for the library for cases, where the name conflicts
#ifdef NO_NAMESPACE // for older GCC
#define LIBEBML_NAMESPACE libebml
#define START_LIBEBML_NAMESPACE
#define END_LIBEBML_NAMESPACE
#else // NO_NAMESPACE


#ifndef LIBEBML_NAMESPACE
#define LIBEBML_NAMESPACE libebml
#define START_LIBEBML_NAMESPACE namespace LIBEBML_NAMESPACE {
#define END_LIBEBML_NAMESPACE   };
#endif // LIBEBML_NAMESPACE

#endif // NO_NAMESPACE


// There are special implementations for certain platforms. For example on Windows
// we use the Win32 file API. here we set the appropriate macros.
#if defined(_WIN32)||defined(WIN32)
#define LIBEBML_WIN32
#ifdef _MSC_VER
#pragma warning(disable:4786)  // length of internal identifiers
#endif // _MSC_VER
#else
#define LIBEBML_UNIX
#endif // WIN32 || _WIN32




// We assume, that the user uses the DLL by default. If he wants to use the static library, the MATROSKA_STATIC value
// has to be specified to the preprocessor
#ifdef LIBEBML_WIN32
	// Either MATROSKA_STATIC or MATROSKA_EXPORT should be defined here.
	#ifdef LIBEBML_STATIC
		#define LIBEBML_EXPORT
	#else
		#ifndef LIBEBML_EXPORT
//			#define LIBEBML_EXPORT __declspec(dllimport) // This should work on Visual C++ and GCC (MinGW and Cygwin)
			#define LIBEBML_EXPORT __declspec(dllexport) // This should work on Visual C++ and GCC (MinGW and Cygwin)
		#endif
	#endif // LIBEBML_STATIC
#else
	// On other platforms we usually don't need the dllimport and dllexport directives
	#define LIBEBML_EXPORT
#endif // LIBEBML_WIN32


#ifndef countof
#define countof(x) (sizeof(x)/sizeof(x[0]))
#endif


// The LIBEBML_DEBUG symbol is defined, when we are creating a debug build. In this
// case the debug logging code is compiled in.
#if (defined(DEBUG)||defined(_DEBUG))&&!defined(LIBEBML_DEBUG)
#define LIBEBML_DEBUG
#endif

// For the moment 10 bytes Float format is not supported
#define LIBEBML_SUPPORT_FLOAT80 0


// For compilers that don't define __TIMSTAMP__ (e.g. gcc 2.95, gcc 3.2)
#ifndef __TIMESTAMP__
#define __TIMESTAMP__ __DATE__ " " __TIME__
#endif

#ifdef __GNUC__
#define EBML_PRETTYLONGINT(c) (c ## ll)
#else // __GNUC__
#define EBML_PRETTYLONGINT(c) (c)
#endif // __GNUC__

#endif // LIBEBML_CONFIG_H
