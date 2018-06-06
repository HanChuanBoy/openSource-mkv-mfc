/****************************************************************************
** libebml : parse EBML files, see http://embl.sourceforge.net/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Steve Lhomme.  All rights reserved.
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
	\version \$Id: EbmlDate.cpp,v 1.3 2003/06/10 22:15:23 mosu Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include <cassert>

#include "StdInclude.h"
#include "EbmlDate.h"

START_LIBEBML_NAMESPACE

const uint64 EbmlDate::UnixEpochDelay = 978307200; // 2001/01/01 00:00:00 UTC

uint64 EbmlDate::ReadData(IOCallback & input)
{
	if (Size != 0) {
		assert(Size == 8);
		binary Buffer[8];
		input.readFully(Buffer, Size);
	
		big_int64 b64;
		b64.Eval(Buffer);
		
		myDate = b64;
	}
	
	return Size;
}

uint32 EbmlDate::RenderData(IOCallback & output, bool bSaveDefault)
{
	if (Size != 0) {
		assert(Size == 8);
		big_int64 b64(myDate);
		
		output.writeFully(&b64.endian(),Size);
	}

	return Size;
}

END_LIBEBML_NAMESPACE
