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
	\version \$Id: StdIOCallback.cpp,v 1.4 2003/05/21 20:52:06 jcsston Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Moritz Bunkus <moritz @ bunkus.org>
*/

#include <cassert>
#include <climits>
#ifndef LIBEBML_GCC2
#include <sstream>
#endif // LIBEBML_GCC2

#include "StdInclude.h"
#include "StdIOCallback.h"
#include "Debug.h"
#include "EbmlConfig.h"

using namespace std;

START_LIBEBML_NAMESPACE

CRTError::CRTError(int Error,const std::string&Description)
	:std::runtime_error(Description+": "+strerror(Error))
	,Error(Error)
{
}

CRTError::CRTError(const std::string&Description,int Error)
	:std::runtime_error(Description+": "+strerror(Error))
	,Error(Error)
{
}


StdIOCallback::StdIOCallback(const char*Path, const open_mode aMode)
{
	assert(Path!=0);

	char *Mode;
	switch (aMode)
	{
	case MODE_READ:
		Mode = "rb";
		break;
	case MODE_SAFE:
		Mode = "rb+";
		break;
	case MODE_WRITE:
		Mode = "wb";
		break;
	case MODE_CREATE:
		Mode = "wb+";
		break;
	default:
		throw 0;
	}

	File=fopen(Path,Mode);
	if(File==0)
	{
#ifndef LIBEBML_GCC2
		stringstream Msg;
		Msg<<"Can't open stdio file \""<<Path<<"\" in mode \""<<Mode<<"\"";
		throw CRTError(Msg.str());
#endif // LIBEBML_GCC2
	}

	EBML_TRACE("Successfully opened file \"%hs\" in mode \"%hs\". The handle is %p\n",Path,Mode,File);
}


StdIOCallback::~StdIOCallback()throw()
{
	try
	{
		close();
	}
	catch(std::exception&Ex)
	{
		EBML_TRACE("Error in ~StdIOCallback: %hs\n",Ex.what());
	}
	catch(...)
	{
		EBML_TRACE("Unknown error in ~StdIOCallback\n");
	}
}



uint32 StdIOCallback::read(void*Buffer,size_t Size)
{
	assert(File!=0);
	
	size_t result = fread(Buffer, 1, Size, File);
	return result;
}

void StdIOCallback::setFilePointer(int64 Offset,seek_mode Mode)
{
	assert(File!=0);

	// There is a numeric cast in the boost library, which would be quite nice for this checking
/* 
	SL : replaced because unknown class in cygwin
	assert(Offset <= numeric_limits<long>::max());
	assert(Offset >= numeric_limits<long>::min());
*/

	assert(Offset <= LONG_MAX);
	assert(Offset >= LONG_MIN);

	assert(Mode==SEEK_CUR||Mode==SEEK_END||Mode==SEEK_SET);

	if(fseek(File,Offset,Mode)!=0)
	{
#ifndef LIBEBML_GCC2
		ostringstream Msg;
		Msg<<"Failed to seek file "<<File<<" to offset "<<(unsigned long)Offset<<" in mode "<<Mode;
		throw CRTError(Msg.str());
#endif // LIBEBML_GCC2
	}
}

size_t StdIOCallback::write(const void*Buffer,size_t Size)
{
	assert(File!=0);

	return fwrite(Buffer,1,Size,File);
}

uint64 StdIOCallback::getFilePointer()
{
	assert(File!=0);

	long Result=ftell(File);
	if(Result<0)
	{
#ifndef LIBEBML_GCC2
		stringstream Msg;
		Msg<<"Can't tell the current file pointer position for "<<File;
		throw CRTError(Msg.str());
#endif // LIBEBML_GCC2
	}

	return Result;
}

void StdIOCallback::close()
{
	if(File==0)
		return;

	if(fclose(File)!=0)
	{
#ifndef LIBEBML_GCC2
		stringstream Msg;
		Msg<<"Can't close file "<<File;
		throw CRTError(Msg.str());
#endif // LIBEBML_GCC2
	}

	File=0;
}

END_LIBEBML_NAMESPACE
