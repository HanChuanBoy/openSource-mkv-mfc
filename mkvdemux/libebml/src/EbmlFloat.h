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
	\version \$Id: EbmlFloat.h,v 1.5 2003/04/28 19:48:37 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBEBML_FLOAT_H
#define LIBEBML_FLOAT_H

#include "EbmlConfig.h"
#include "EbmlTypes.h"
#include "EbmlElement.h"

START_LIBEBML_NAMESPACE

/*!
    \class EbmlFloat
    \brief Handle all operations on a float EBML element
*/
class EbmlFloat : public EbmlElement {
	public:
		enum Precision {
			 FLOAT_32
			,FLOAT_64
#if LIBEBML_SUPPORT_FLOAT80
			,FLOAT_80
#endif // LIBEBML_SUPPORT_FLOAT80
		};

		EbmlFloat(const Precision prec = FLOAT_32);
		EbmlFloat(const double DefaultValue, const Precision prec = FLOAT_32);
		EbmlFloat(const EbmlElement & ElementToClone);
		EbmlFloat(const EbmlElement & ElementToClone, double DefaultValue);
		bool ValidateSize() const 
		{
			return (Size == 4 || Size == 8
#if LIBEBML_SUPPORT_FLOAT80
			|| Size == 10
#endif // LIBEBML_SUPPORT_FLOAT80
				);
		}
	
		uint32 RenderData(IOCallback & output, bool bSaveDefault = false);
		uint64 ReadData(IOCallback & input);
		uint64 UpdateSize(bool bSaveDefault = false);

		void SetPrecision(const EbmlFloat::Precision prec = FLOAT_32) 
		{
			if (prec == FLOAT_64)
				Size = 8;
#if LIBEBML_SUPPORT_FLOAT80
			else if (prec == FLOAT_80)
				Size = 10;
#endif // LIBEBML_SUPPORT_FLOAT80
			else
				Size = 4; // default size
		}

	
//		EbmlFloat & operator=(const float NewValue) { Value = NewValue; return *this;}
		EbmlFloat & operator=(const double NewValue) { Value = NewValue; bValueIsSet = true; return *this;}

		bool operator<(const EbmlFloat & EltCmp) const {return Value < EltCmp.Value;}
		
		operator const float() const {return float(Value);}
		operator const double() const {return double(Value);}

		const double DefaultVal() const {assert(DefaultIsSet); return DefaultValue;}

		bool IsDefaultValue() const {
			return (DefaultISset() && Value == DefaultValue);
		}

	protected:
		double Value; /// The actual value of the element
		double DefaultValue;
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_FLOAT_H
