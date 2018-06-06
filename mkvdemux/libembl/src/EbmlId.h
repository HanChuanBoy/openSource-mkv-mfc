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
	\version \$Id: EbmlId.h,v 1.3 2003/04/23 21:54:36 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBEBML_ID_H
#define LIBEBML_ID_H

#include "EbmlConfig.h"
#include "EbmlTypes.h"

START_LIBEBML_NAMESPACE

/*!
	\class EbmlId
*/
class EbmlId {
	public:
		uint32 Value;
		unsigned int Length;

		EbmlId(const binary aValue[4], const unsigned int aLength)
			:Length(aLength)
		{
			Value = 0;
			unsigned int i;
			for (i=0; i<aLength; i++) {
				Value <<= 8;
				Value += aValue[i];
			}
		}

		EbmlId(const uint32 aValue, const unsigned int aLength)
			:Value(aValue), Length(aLength) {}

		inline bool operator==(const EbmlId & TestId) const
		{
			return ((TestId.Length == Length) && (TestId.Value == Value));
		}

		inline void Fill(binary * Buffer) const {
			unsigned int i;
			for (i = 0; i<Length; i++) {
				Buffer[i] = (Value >> (8*(Length-i-1))) & 0xFF;
			}
			//把数组看作是一个房子，低层的住着高位(数字的高位);
		}
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_ID_H
