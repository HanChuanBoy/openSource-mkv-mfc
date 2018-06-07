/****************************************************************************
** libmatroska : parse Matroska files, see http://www.matroska.org/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Steve Lhomme.  All rights reserved.
**
** This file is part of libmatroska.
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
    \file libmatroska_t.h
    \version \$Id: libmatroska_t.h,v 1.3 2003/05/27 11:17:23 mosu Exp $
    \author Steve Lhomme     <robux4 @ users.sf.net>
    \author Ingo Ralf Blum   <ingoralfblum @ users.sf.net>

    \brief Misc type definitions for the C API of libmatroska

    \note These types should be compiler/language independant (just platform dependant)
    \todo recover the sized types (uint16, int32, etc) here too (or maybe here only)
*/

#ifndef _LIBMATROSKA_T_H_INCLUDED_
#define _LIBMATROSKA_T_H_INCLUDED_

#include "api/c/libebml_t.h"

#ifdef __cplusplus
extern "C" {
#endif

// There are special implementations for certain platforms. For example on Windows
// we use the Win32 file API. here we set the appropriate macros.
#if defined(_WIN32)||defined(WIN32)
#define LIBMATROSKA_WIN32
#else
#define LIBMATROSKA_UNIX
#endif

// We assume, that the user uses the DLL by default. If he wants to use the static library, the LIBMATROSKA_STATIC value
// has to be specified to the preprocessor
#ifdef LIBMATROSKA_WIN32
	// Either LIBMATROSKA_STATIC or LIBMATROSKA_EXPORT should be defined here.
	#ifdef LIBMATROSKA_STATIC
		#define LIBMATROSKA_EXPORT
	#else
		#ifndef LIBMATROSKA_EXPORT
			#define LIBMATROSKA_EXPORT __declspec(dllimport) // This should work on Visual C++ and GCC (MinGW and Cygwin)
		#endif
	#endif // LIBMATROSKA_STATIC
#else
	// On other platforms we usually don't need the dllimport and dllexport directives
	#define LIBMATROSKA_EXPORT
#endif // LIBMATROSKA_WIN32

/*!
    \enum track_type
*/
typedef enum track_type {
    track_video       = 0x01, ///< Rectangle-shaped non-transparent pictures aka video
    track_audio       = 0x02, ///< Anything you can hear
    track_complex     = 0x03, ///< Audio and video in same track, used by DV

    track_logo        = 0x10, ///< Overlay-pictures, displayed over video
    track_subtitle    = 0x11, ///< Text-subtitles. One track contains one language and only one track can be active (player-side configuration)

    track_control     = 0x20  ///< Control-codes for menus and other stuff
} track_type;

/*!
    \enum matroska_error_t
    \brief a callback that the library use to inform of errors happening
    \note this should be used by the libmatroska internals
*/
typedef enum {
	error_null_pointer  ///< NULL pointer where something else is expected
} matroska_error_t;

typedef void *matroska_stream;

/*!
    \var void* matroska_id
    \brief UID used to access an Matroska file instance
*/
typedef void* matroska_id;
/*!
    \var void* matroska_track
    \brief UID used to access a track
*/
typedef void* matroska_track;
/*!
    \var char* c_string
    \brief C-String, ie a buffer with characters terminated by \0
*/
typedef char* c_string;
/*!
    \var unsigned int matroska_file_mode
    \brief A bit buffer, each bit representing a value for the file opening
    \todo replace the unsigned int by a sized type (8 or 16 bits)
*/
typedef char * matroska_file_mode;
/*!
    \var void (*matroska_error_callback)(matroska_error_t error_code, char* error_message)
    \brief a callback that the library use to inform of errors happening
*/
typedef void (*matroska_error_callback)(matroska_error_t error_code, char* error_message);

#ifdef __cplusplus
}
#endif

#endif /* _LIBMATROSKA_T_H_INCLUDED_ */
