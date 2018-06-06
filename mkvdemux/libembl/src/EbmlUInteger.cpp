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
	\version \$Id: EbmlUInteger.cpp,v 1.9 2003/05/27 16:56:17 mosu Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Moritz Bunkus <moritz @ bunkus.org>
*/
#include <cassert>

#include "StdInclude.h"
#include "EbmlUInteger.h"

START_LIBEBML_NAMESPACE

EbmlUInteger::EbmlUInteger()
 :EbmlElement(DEFAULT_UINT_SIZE, false)
{}

EbmlUInteger::EbmlUInteger(const uint64 aDefaultValue)
 :EbmlElement(DEFAULT_UINT_SIZE, true), Value(aDefaultValue), DefaultValue(aDefaultValue)
{
	DefaultIsSet = true;
}

EbmlUInteger::EbmlUInteger(const EbmlElement & ElementToClone)
 :EbmlElement(DEFAULT_UINT_SIZE, false)
{
	*(static_cast<EbmlElement*>(this)) = ElementToClone;
}

EbmlUInteger::EbmlUInteger(const EbmlElement & ElementToClone, const uint64 aDefaultValue)
 :EbmlElement(DEFAULT_UINT_SIZE, true), Value(aDefaultValue), DefaultValue(aDefaultValue)
{
	*(static_cast<EbmlElement*>(this)) = ElementToClone;
	DefaultIsSet = true;
}

/*!
	\todo handle exception on errors
*/
uint32 EbmlUInteger::RenderData(IOCallback & output, bool bSaveDefault)
{
	binary FinalData[8]; // we don't handle more than 64 bits integers
	
	if (SizeLength > 8)
		return 0; // integer bigger coded on more than 64 bits are not supported
	
	uint64 TempValue = Value;
	for (unsigned int i=0; i<Size;i++) {
		FinalData[Size-i-1] = TempValue & 0xFF;
		TempValue >>= 8;
	}
	
	output.writeFully(FinalData,Size);

	return Size;
}

uint64 EbmlUInteger::UpdateSize(bool bSaveDefault)
{
	if (!bSaveDefault && IsDefaultValue())
		return 0;

	if (Value <= 0xFF) {
		Size = 1;
	} else if (Value <= 0xFFFF) {
		Size = 2;
	} else if (Value <= 0xFFFFFF) {
		Size = 3;
	} else if (Value <= 0xFFFFFFFF) {
		Size = 4;
	} else if (Value <= EBML_PRETTYLONGINT(0xFFFFFFFFFF)) {
		Size = 5;
	} else if (Value <= EBML_PRETTYLONGINT(0xFFFFFFFFFFFF)) {
		Size = 6;
	} else if (Value <= EBML_PRETTYLONGINT(0xFFFFFFFFFFFFFF)) {
		Size = 7;
	} else {
		Size = 8;
	}

	if (DefaultSize > Size) {
		Size = DefaultSize;
	}

	return Size;
}

uint64 EbmlUInteger::ReadData(IOCallback & input)
{
	binary Buffer[8];
	input.readFully(Buffer, Size);
	Value = 0;
	
	for (unsigned int i=0; i<Size; i++)
	{
		Value <<= 8;
		Value |= Buffer[i];
	}

	return Size;
}

END_LIBEBML_NAMESPACE
