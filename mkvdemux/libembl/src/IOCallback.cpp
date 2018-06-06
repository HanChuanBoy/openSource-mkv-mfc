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
	\version \$Id: IOCallback.cpp,v 1.2 2003/04/13 13:47:22 mosu Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Moritz Bunkus <moritz @ bunkus.org>
*/

#ifndef LIBEBML_GCC2
#include <sstream>
#endif // LIBEBML_GCC2
#include <stdexcept>


#include "StdInclude.h"
#include "IOCallback.h"

using namespace std;

START_LIBEBML_NAMESPACE

void IOCallback::writeFully(const void*Buffer,size_t Size)
{
	if (Size == 0)
		return;

	if (Buffer == NULL)
		throw;

	if(write(Buffer,Size) != Size)
	{
#ifndef LIBEBML_GCC2
		stringstream Msg;
		Msg<<"EOF in writeFully("<<Buffer<<","<<Size<<")";
		throw runtime_error(Msg.str());
#endif // LIBEBML_GCC2
	}
}



void IOCallback::readFully(void*Buffer,size_t Size)
{
	if(Buffer == NULL)
		throw;

	if(read(Buffer,Size) != Size)
	{
#ifndef LIBEBML_GCC2
		stringstream Msg;
		Msg<<"EOF in readFully("<<Buffer<<","<<Size<<")";
		throw runtime_error(Msg.str());
#endif // LIBEBML_GCC2
	}
}

END_LIBEBML_NAMESPACE
