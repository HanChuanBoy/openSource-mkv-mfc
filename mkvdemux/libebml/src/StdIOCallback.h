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
	\version \$Id: StdIOCallback.h,v 1.1 2003/04/03 16:40:48 robux4 Exp $
*/
#ifndef LIBEBML_STDIOCALLBACK_H
#define LIBEBML_STDIOCALLBACK_H

#include "IOCallback.h"

#include <stdexcept>
#include <cerrno>

START_LIBEBML_NAMESPACE

class CRTError:public std::runtime_error
{
// Variablen...
private:
	int Error;

// Methoden...
public:
	CRTError(int Error,const std::string&Description);
	CRTError(const std::string&Description,int Error=errno);

	int getError()const throw(){return Error;}
};

// This class is currently private to the library, so there's no MATROSKA_EXPORT.
class StdIOCallback:public IOCallback
{
    private:
    	FILE*File;

    public:
//	StdIOCallback(const char*Path,const char*Mode);
	StdIOCallback(const char*Path, const open_mode Mode);
	virtual ~StdIOCallback()throw();

	virtual uint32 read(void*Buffer,size_t Size);

	// Seek to the specified position. The mode can have either SEEK_SET, SEEK_CUR
	// or SEEK_END. The callback should return true(1) if the seek operation succeeded
	// or false (0), when the seek fails.
	virtual void setFilePointer(int64 Offset,seek_mode Mode=seek_beginning);

	// This callback just works like its read pendant. It returns the number of bytes written.
	virtual size_t write(const void*Buffer,size_t Size);

	// Although the position is always positive, the return value of this callback is signed to
	// easily allow negative values for returning errors. When an error occurs, the implementor
	// should return -1 and the file pointer otherwise.
	//
	// If an error occurs, an exception should be thrown.
	virtual uint64 getFilePointer();

	// The close callback flushes the file buffers to disk and closes the file. When using the stdio
	// library, this is equivalent to calling fclose. When the close is not successful, an exception
	// should be thrown.
	virtual void close();
};

END_LIBEBML_NAMESPACE

#endif
