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
	\version \$Id: EbmlBinary.cpp,v 1.3 2003/04/28 19:48:37 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Julien Coloos	<suiryc @ users.sf.net>
*/
#include <cassert>

#include "StdInclude.h"
#include "EbmlBinary.h"

START_LIBEBML_NAMESPACE

EbmlBinary::EbmlBinary()
 :EbmlElement(0, false), Data(NULL)
{}

/*!
	\todo shouldn't we copy the Data if they exist ???
*/
EbmlBinary::EbmlBinary(const EbmlElement & ElementToClone)
 :EbmlElement(0), Data(NULL)
{
	*(static_cast<EbmlElement*>(this)) = ElementToClone;
}

EbmlBinary::~EbmlBinary(void) {
	if(Data)
		delete[] Data;
}

uint32 EbmlBinary::RenderData(IOCallback & output, bool bSaveDefault)
{
	output.writeFully(Data,Size);

	return Size;
}
	
/*!
	\note no Default binary value handled
*/
uint64 EbmlBinary::UpdateSize(bool bSaveDefault)
{
	return Size;
}

uint64 EbmlBinary::ReadData(IOCallback & input)
{
	if (Data != NULL)
		delete Data;
	
	Data = new binary[Size];
	return input.read(Data, Size);
}

END_LIBEBML_NAMESPACE
