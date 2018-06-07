/****************************************************************************
** libmatroska : parse Matroska files, see http://www.matroska.org/
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
	\version \$Id: KaxConfig.h,v 1.2 2003/04/28 10:06:10 robux4 Exp $
*/
#ifndef LIBMATROSKA_CONFIG_H
#define LIBMATROSKA_CONFIG_H

// We allow the user to set the namespace for the library for cases, where the name conflicts
#ifdef NO_NAMESPACE // for older GCC

#define LIBMATROSKA_NAMESPACE libmatroska
#define START_LIBMATROSKA_NAMESPACE
#define END_LIBMATROSKA_NAMESPACE
#else // NO_NAMESPACE

#ifndef LIBMATROSKA_NAMESPACE
#define LIBMATROSKA_NAMESPACE libmatroska
#define START_LIBMATROSKA_NAMESPACE namespace LIBMATROSKA_NAMESPACE {
#define END_LIBMATROSKA_NAMESPACE   };
#endif // LIBMATROSKA_NAMESPACE

#endif // NO_NAMESPACE

// There are special implementations for certain platforms. For example on Windows
// we use the Win32 file API. here we set the appropriate macros.
#if defined(_WIN32)||defined(WIN32)
#define LIBMATROSKA_WIN32
#else
#define LIBMATROSKA_UNIX
#endif



// We assume, that the user uses the DLL by default. If he wants to use the static library, the MATROSKA_STATIC value
// has to be specified to the preprocessor
#ifdef LIBMATROSKA_WIN32
	// Either MATROSKA_STATIC or MATROSKA_EXPORT should be defined here.
	#ifdef LIBMATROSKA_STATIC
		#define LIBMATROSKA_EXPORT
	#else
		#ifndef LIBMATROSKA_EXPORT
//			#define LIBMATROSKA_EXPORT __declspec(dllimport) // This should work on Visual C++ and GCC (MinGW and Cygwin)
			#define LIBMATROSKA_EXPORT __declspec(dllexport) // This should work on Visual C++ and GCC (MinGW and Cygwin)
		#endif
	#endif // LIBMATROSKA_STATIC
#else
	// On other platforms we usually don't need the dllimport and dllexport directives
	#define LIBMATROSKA_EXPORT
#endif // LIBMATROSKA_WIN32


// The MATROSKA_DEBUG symbol is defined, when we are creating a debug build. In this
// case the debug logging code is compiled in.
#if (defined(DEBUG)||defined(_DEBUG))&&!defined(LIBMATROSKA_DEBUG)
#define LIBMATROSKA_DEBUG
#endif

#endif // LIBMATROSKA_CONFIG_H
