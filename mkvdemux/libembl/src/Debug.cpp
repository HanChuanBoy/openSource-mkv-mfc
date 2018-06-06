/****************************************************************************
** libebml : parse EBML files, see http://embl.sourceforge.net/
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
	\file
	\version \$Id: Debug.cpp,v 1.1 2003/04/03 16:40:48 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Moritz Bunkus <moritz @ bunkus.org>
*/
#include <stdio.h>

#ifdef WIN32
#include <windows.h> // For OutputDebugString
#else
#include <time.h>
#include <sys/time.h>
#endif // WIN32

#include "StdInclude.h"
#include "Debug.h"
#include "EbmlConfig.h"

START_LIBEBML_NAMESPACE

class ADbg globalDebug;

#if !defined(NDEBUG)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ADbg::ADbg(int level)
:my_level(level)
,my_time_included(false)
,my_use_file(false)
,my_debug_output(true)
,hFile(NULL)
{
	prefix[0] = '\0';
	OutPut(-1,"ADbg Creation at debug level = %d (0x%08X)",my_level,this);
}

ADbg::~ADbg()
{
	unsetDebugFile();
	OutPut(-1,"ADbg Deletion (0x%08X)",this);
}

inline int ADbg::_OutPut(const char * format,va_list params) const
{
	int result;

	char tst[1000];
	char myformat[256];

#ifdef WIN32
	if (my_time_included) {
		SYSTEMTIME time;
		GetSystemTime(&time);
		if (prefix[0] == '\0')
			wsprintf(myformat,"%04d/%02d/%02d %02d:%02d:%02d.%03d UTC : %s\r\n",
							time.wYear,
							time.wMonth,
							time.wDay,
							time.wHour,
							time.wMinute,
							time.wSecond,
							time.wMilliseconds,
							format);
		else
			wsprintf(myformat,"%04d/%02d/%02d %02d:%02d:%02d.%03d UTC : %s - %s\r\n",
							time.wYear,
							time.wMonth,
							time.wDay,
							time.wHour,
							time.wMinute,
							time.wSecond,
							time.wMilliseconds,
							prefix,
							format);
	} else {
		if (prefix[0] == '\0')
			wsprintf( myformat, "%s\r\n", format);
		else
			wsprintf( myformat, "%s - %s\r\n", prefix, format);
	}
	result = vsprintf(tst,myformat,params);
	
	if (my_debug_output)
		OutputDebugString(tst);

	if (my_use_file && (hFile != NULL)) {
		SetFilePointer( hFile, 0, 0, FILE_END );
		DWORD written;
		WriteFile( hFile, tst, lstrlen(tst), &written, NULL );
	}
#else
	if (my_time_included) {
		time_t nowSecs;
		struct tm *now;
		struct timeval tv;
		
		nowSecs = time(NULL);
		gettimeofday(&tv, NULL);
		now = gmtime(&nowSecs);
		if (prefix[0] == '\0')
			sprintf(myformat,"%04d/%02d/%02d %02d:%02d:%02ld.%03ld UTC : %s\r\n",
					now->tm_year, now->tm_mon, now->tm_mday,
					now->tm_hour, now->tm_min, tv.tv_sec,
					tv.tv_usec / 1000, format);
		else
			sprintf(myformat,"%04d/%02d/%02d %02d:%02d:%02ld.%03ld UTC : %s - %s\r\n",
					now->tm_year, now->tm_mon, now->tm_mday,
					now->tm_hour, now->tm_min, tv.tv_sec,
					tv.tv_usec / 1000, prefix, format);
		
	} else {
		if (prefix[0] == '\0')
			sprintf( myformat, "%s\r\n", format);
		else
			sprintf( myformat, "%s - %s\r\n", prefix, format);
	}

	result = vsprintf(tst,myformat,params);
	
	if (my_debug_output)
		fputs(tst, stderr);

	if (my_use_file && (hFile != NULL))
		fputs(tst, hFile);
#endif

	return result;
}

int ADbg::OutPut(int forLevel, const char * format,...) const
{
	int result=0;
	
	if (forLevel >= my_level) {
		va_list tstlist;
		int result;

		va_start(tstlist, format);

		result = _OutPut(format,tstlist);

	}

	return result;
}

int ADbg::OutPut(const char * format,...) const
{
	va_list tstlist;

	va_start(tstlist, format);

	return _OutPut(format,tstlist);
}

bool ADbg::setDebugFile(const char * NewFilename) {
	bool result;
	result = unsetDebugFile();

	if (result) {
		result = false;

#ifdef WIN32
		hFile = CreateFile(NewFilename, GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		
		if (hFile != INVALID_HANDLE_VALUE) {
			SetFilePointer( hFile, 0, 0, FILE_END );

			result = true;
#else
		hFile = fopen(NewFilename, "w+");
		if (hFile != NULL) {
			fseek(hFile, 0, SEEK_END);
#endif
			OutPut(-1,"Debug hFile Opening succeeded");

		}
		else
			OutPut(-1,"Debug hFile %s Opening failed",NewFilename);
	}

	return result;
}

bool ADbg::unsetDebugFile() {
	bool result = (hFile == NULL);
	
#ifdef WIN32
	if (hFile != NULL) {
		result = (CloseHandle(hFile) != 0);
#else
	if (hFile != NULL) {
		result = (fclose(hFile) == 0);
#endif

		if (result) {
			OutPut(-1,"Debug hFile Closing succeeded");
			hFile = NULL;
		}
	}
	return result;
}

#endif // !defined(NDEBUG)

END_LIBEBML_NAMESPACE
