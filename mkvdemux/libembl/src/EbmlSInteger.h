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
	\version \$Id: EbmlSInteger.h,v 1.4 2003/04/28 19:48:37 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Julien Coloos    <suiryc @ users.sf.net>
	\author Moritz Bunkus    <moritz @ bunkus.org>
*/
#ifndef LIBEBML_SINTEGER_H
#define LIBEBML_SINTEGER_H

#include "EbmlConfig.h"
#include "EbmlTypes.h"
#include "EbmlElement.h"

START_LIBEBML_NAMESPACE

const int DEFAULT_INT_SIZE = 1; ///< optimal size stored

/*!
    \class EbmlSInteger
    \brief Handle all operations on a signed integer EBML element
*/
class EbmlSInteger : public EbmlElement {
	public:
		EbmlSInteger();
		EbmlSInteger(const int64 DefaultValue);
		EbmlSInteger(const EbmlElement & ElementToClone);
		EbmlSInteger(const EbmlElement & ElementToClone, const int64 DefaultValue);
	
		EbmlSInteger & operator=(const uint64 NewValue) {Value = NewValue; bValueIsSet = true; return *this;}

		/*!
			Set the default size of the integer (usually 1,2,4 or 8)
		*/
		void SetDefaultSize(const int DefaultSize = DEFAULT_INT_SIZE) {Size = DefaultSize;}

		bool ValidateSize() const {return (Size <= 8);}
		uint32 RenderData(IOCallback & output, bool bSaveDefault = false);
		uint64 ReadData(IOCallback & input);
		uint64 UpdateSize(bool bSaveDefault = false);
	
		bool operator<(const EbmlSInteger & EltCmp) const {return Value < EltCmp.Value;}
		
		operator int8() {return Value;}
		operator int16() {return Value;}
		operator int32() {return Value;}
		operator int64() {return Value;}

		const int64 DefaultVal() const {assert(DefaultIsSet); return DefaultValue;}

		bool IsDefaultValue() const {
			return (DefaultISset() && Value == DefaultValue);
		}

	protected:
		int64 Value; /// The actual value of the element
		int64 DefaultValue;
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_SINTEGER_H
