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
	\version \$Id: EbmlFloat.cpp,v 1.6 2003/05/08 10:09:37 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/

#include <cassert>

#include "StdInclude.h"
#include "EbmlFloat.h"

START_LIBEBML_NAMESPACE

EbmlFloat::EbmlFloat(const EbmlFloat::Precision prec)
 :EbmlElement(0, false)
{
	SetPrecision(prec);
}

EbmlFloat::EbmlFloat(const double aDefaultValue, const EbmlFloat::Precision prec)
 :EbmlElement(0, true), Value(aDefaultValue), DefaultValue(aDefaultValue)
{
	DefaultIsSet = true;
	SetPrecision(prec);
}

EbmlFloat::EbmlFloat(const EbmlElement & ElementToClone)
 :EbmlElement(0, false)
{
	*(static_cast<EbmlElement*>(this)) = ElementToClone;
}

EbmlFloat::EbmlFloat(const EbmlElement & ElementToClone, const double aDefaultValue)
 :EbmlElement(0, true), Value(DefaultValue), DefaultValue(aDefaultValue)
{
	*(static_cast<EbmlElement*>(this)) = ElementToClone;
	DefaultIsSet = true;
}

/*!
	\todo handle exception on errors
	\todo handle 10 bits precision
*/
uint32 EbmlFloat::RenderData(IOCallback & output, bool bSaveDefault)
{
#if LIBEBML_SUPPORT_FLOAT80
	assert(Size == 4 || Size == 8 || Size == 10);
#else // LIBEBML_SUPPORT_FLOAT80
	assert(Size == 4 || Size == 8);
#endif // LIBEBML_SUPPORT_FLOAT80

	if (Size == 4) {
		float val = Value;
		big_int32 TmpToWrite(*((int32 *) &val));
		output.writeFully(&TmpToWrite.endian(), Size);
	} else if (Size == 8) {
		double val = Value;
		big_int64 TmpToWrite(*((int64 *) &val));
		output.writeFully(&TmpToWrite.endian(), Size);
	} 
#if LIBEBML_SUPPORT_FLOAT80
	else {
		long double val = Value;
		assert(sizeof(long double) == 10);
		big_80bits TmpToWrite(*((bits80 *) &val));
		output.writeFully(&TmpToWrite.endian(), Size);
	}
#endif // LIBEBML_SUPPORT_FLOAT80

	return Size;
}

uint64 EbmlFloat::UpdateSize(bool bSaveDefault)
{
	if (!bSaveDefault && IsDefaultValue())
		return 0;
	return Size;
}

/*!
	\todo remove the hack for possible endianess pb (test on little & big endian)
*/
uint64 EbmlFloat::ReadData(IOCallback & input)
{
	binary Buffer[20];
	assert(Size <= 20);
	input.readFully(Buffer, Size);
	
	if (Size == 4) {
		big_int32 TmpRead;
		TmpRead.Eval(Buffer);
		int32 val32=(int32)TmpRead;
		float val = *((float *)&(val32));
		Value = val;
	} else if (Size == 8) {
		big_int64 TmpRead;
		TmpRead.Eval(Buffer);
		int64 tmpp = int64(TmpRead);
		Value = *((double *) &tmpp);
	}
#if LIBEBML_SUPPORT_FLOAT80
	else {
		big_80bits TmpRead;
		TmpRead.Eval(Buffer);
//		float val = *((float *)&(int32(TmpRead)));
	}
#endif // LIBEBML_SUPPORT_FLOAT80

	return Size;
}

END_LIBEBML_NAMESPACE
