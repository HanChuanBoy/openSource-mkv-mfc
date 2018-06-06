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
	\version \$Id: EbmlString.cpp,v 1.8 2003/05/30 10:15:38 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include <cassert>

#include "StdInclude.h"
#include "EbmlString.h"

START_LIBEBML_NAMESPACE

EbmlString::EbmlString()
 :EbmlElement(0, false)
{
	DefaultSize = 0;
/* done automatically	
	Size = Value.length();
	if (DefaultSize > Size)
		Size = DefaultSize;*/
}

EbmlString::EbmlString(const std::string & aDefaultValue)
 :EbmlElement(0, true), Value(aDefaultValue), DefaultValue(aDefaultValue)
{
	DefaultSize = 0;
	DefaultIsSet = true;
/* done automatically	
	Size = Value.length();
	if (DefaultSize > Size)
		Size = DefaultSize;*/
}

/*!
	\todo Cloning should be on the same exact type !
*/
EbmlString::EbmlString(const EbmlElement & ElementToClone)
 :EbmlElement(0, false)
{
	*(static_cast<EbmlElement*>(this)) = ElementToClone;
}

/*!
	\todo Cloning should be on the same exact type !
*/
EbmlString::EbmlString(const EbmlElement & ElementToClone, const std::string & aDefaultValue)
 :EbmlElement(0, true), Value(aDefaultValue), DefaultValue(aDefaultValue)
{
	*(static_cast<EbmlElement*>(this)) = ElementToClone;
	DefaultIsSet = true;
}

/*!
	\todo handle exception on errors
*/
uint32 EbmlString::RenderData(IOCallback & output, bool bSaveDefault)
{
	uint32 Result;
	output.writeFully(Value.c_str(), Value.length());
	Result = Value.length();
	
	if (Result < DefaultSize) {
		// pad the rest with 0
		binary *Pad = new binary[DefaultSize - Result];
		if (Pad == NULL)
		{
			return Result;
		}
		memset(Pad, 0x00, DefaultSize - Result);
		output.writeFully(Pad, DefaultSize - Result);
		Result = DefaultSize;
		delete [] Pad;
	}
	
	return Result;
}

EbmlString & EbmlString::operator=(const std::string NewString)
{
	Value = NewString;
	bValueIsSet = true;
/* done automatically	
	Size = Value.length();
	if (DefaultSize > Size)
		Size = DefaultSize;*/
	return *this;
}

uint64 EbmlString::UpdateSize(bool bSaveDefault)
{
	if (!bSaveDefault && IsDefaultValue())
		return 0;

	if (Value.length() < DefaultSize) {
		Size = DefaultSize;
	} else {
		Size = Value.length();
	}
	return Size;
}

uint64 EbmlString::ReadData(IOCallback & input)
{
	if (Size == 0) {
		Value = "";
	} else {
		char *Buffer = new char[Size + 1];
		if (Buffer == NULL) {
			// unable to store the data, skip it
			input.setFilePointer(Size, seek_current);
		} else {
			input.readFully(Buffer, Size);
			if (Buffer[Size-1] != '\0') {
				Buffer[Size] = '\0';
			}
			Value = Buffer;
			delete [] Buffer;
		}
	}

	return Size;
}

END_LIBEBML_NAMESPACE
