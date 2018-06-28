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
	\version \$Id: EbmlDate.h,v 1.2 2003/04/28 19:48:37 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBEBML_DATE_H
#define LIBEBML_DATE_H

#include "EbmlConfig.h"
#include "EbmlTypes.h"
#include "EbmlElement.h"

START_LIBEBML_NAMESPACE

/*!
    \class EbmlDate
    \brief Handle all operations related to an EBML date
*/
class EbmlDate : public EbmlElement {
	public:
		EbmlDate() :EbmlElement(8, false), myDate(0) {}

		/*!
			\brief set the date with a UNIX/C/EPOCH form
			\param NewDate UNIX/C date in UTC (no timezone)
		*/
		void SetEpochDate(int32 NewDate) {bValueIsSet = true; myDate = int64(NewDate - UnixEpochDelay) * 1000000000; bValueIsSet = true;}

		/*!
			\brief get the date with a UNIX/C/EPOCH form
			\note the date is in UTC (no timezone)
		*/
		int32 GetEpochDate() const {return myDate/1000000000 + UnixEpochDelay;}
	
		bool ValidateSize() const {return ((Size == 8) || (Size == 0));}

		/*!
			\note no Default date handled
		*/
		uint64 UpdateSize(bool bSaveDefault = false) {
			if(!bValueIsSet) 
				Size = 0;
			else
				Size = 8;
			return Size;
		}
		
		bool operator<(const EbmlDate & EltCmp) const {return myDate < EltCmp.myDate;}
		
		uint64 ReadData(IOCallback & input);

		bool IsDefaultValue() const {
			return false;
		}

	protected:
		uint32 RenderData(IOCallback & output, bool bSaveDefault = false);
		
		int64 myDate; ///< internal format of the date
	
		static const uint64 UnixEpochDelay;
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_DATE_H
