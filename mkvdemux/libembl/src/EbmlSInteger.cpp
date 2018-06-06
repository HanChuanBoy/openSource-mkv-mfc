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
	\version \$Id: EbmlSInteger.cpp,v 1.9 2003/05/27 16:56:17 mosu Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Moritz Bunkus <moritz @ bunkus.org>
*/
#include <cassert>

#include "StdInclude.h"
#include "EbmlSInteger.h"

START_LIBEBML_NAMESPACE

EbmlSInteger::EbmlSInteger()
 :EbmlElement(DEFAULT_INT_SIZE, false)
{}

EbmlSInteger::EbmlSInteger(const int64 aDefaultValue)
 :EbmlElement(DEFAULT_INT_SIZE, true), Value(aDefaultValue)
{
	DefaultIsSet = true;
}

EbmlSInteger::EbmlSInteger(const EbmlElement & ElementToClone)
 :EbmlElement(DEFAULT_INT_SIZE, false)
{
	*(static_cast<EbmlElement*>(this)) = ElementToClone;
}

EbmlSInteger::EbmlSInteger(const EbmlElement & ElementToClone, const int64 aDefaultValue)
 :EbmlElement(DEFAULT_INT_SIZE, true), Value(aDefaultValue), DefaultValue(aDefaultValue)
{
	*(static_cast<EbmlElement*>(this)) = ElementToClone;
	DefaultIsSet = true;
}

/*!
	\todo handle exception on errors
*/
uint32 EbmlSInteger::RenderData(IOCallback & output, bool bSaveDefault)
{
	binary FinalData[8]; // we don't handle more than 64 bits integers
	unsigned int i;
	
	if (SizeLength > 8)
		return 0; // integer bigger coded on more than 64 bits are not supported
	
	int64 TempValue = Value;
	for (i=0; i<Size;i++) {
		FinalData[Size-i-1] = TempValue & 0xFF;
		TempValue >>= 8;
	}
	
	output.writeFully(FinalData,Size);

	return Size;
}

uint64 EbmlSInteger::UpdateSize(bool bSaveDefault)
{
	if (!bSaveDefault && IsDefaultValue())
		return 0;

	if (Value <= 0x7F && Value >= (-0x80)) {
		Size = 1;
	} else if (Value <= 0x7FFF && Value >= (-0x8000)) {
		Size = 2;
	} else if (Value <= 0x7FFFFF && Value >= (-0x800000)) {
		Size = 3;
	} else if (Value <= 0x7FFFFFFF && Value >= (-0x80000000)) {
		Size = 4;
	} else if (Value <= EBML_PRETTYLONGINT(0x7FFFFFFFFF) &&
		   Value >= EBML_PRETTYLONGINT(-0x8000000000)) {
		Size = 5;
	} else if (Value <= EBML_PRETTYLONGINT(0x7FFFFFFFFFFF) &&
		   Value >= EBML_PRETTYLONGINT(-0x800000000000)) {
		Size = 6;
	} else if (Value <= EBML_PRETTYLONGINT(0x7FFFFFFFFFFFFF) &&
		   Value >= EBML_PRETTYLONGINT(-0x80000000000000)) {
		Size = 7;
	} else {
		Size = 8;
	}

	if (DefaultSize > Size) {
		Size = DefaultSize;
	}

	return Size;
}

uint64 EbmlSInteger::ReadData(IOCallback & input)
{
	binary Buffer[8];
	input.readFully(Buffer, Size);
	
	if (Buffer[0] & 0x80)
		Value = -1; // this is a negative value
	else
		Value = 0; // this is a positive value
	
	for (unsigned int i=0; i<Size; i++)
	{
		Value <<= 8;
		Value |= Buffer[i];
	}

	return Size;
}

END_LIBEBML_NAMESPACE
