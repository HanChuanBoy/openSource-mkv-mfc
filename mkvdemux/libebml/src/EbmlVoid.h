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
	\version \$Id: EbmlVoid.h,v 1.3 2003/04/28 20:16:06 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBEBML_VOID_H
#define LIBEBML_VOID_H

#include "EbmlConfig.h"
#include "EbmlTypes.h"
#include "EbmlBinary.h"

START_LIBEBML_NAMESPACE

class EbmlVoid : public EbmlBinary {
	public:
		EbmlVoid();
		static EbmlElement & Create() {return *(new EbmlVoid);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		bool ValidateSize() const {return true;} // any void element is accepted
		static const EbmlCallbacks ClassInfos;

		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		bool IsYourId(const EbmlId & TestId) const;

		void SetSize(uint64 aSize) {Size = aSize;}

		/*!
			\note overwrite to write fake data 
		*/
		uint32 RenderData(IOCallback & output, bool bSaveDefault = false);

		/*!
			\brief Replace the void element content (written) with this one
		*/
		uint32 ReplaceWith(EbmlElement & EltToReplaceWith, IOCallback & output, bool ComeBackAfterward = true, bool bSaveDefault = false);

		/*!
			\brief Void the content of an element
		*/
		uint32 Overwrite(const EbmlElement & EltToVoid, IOCallback & output, bool ComeBackAfterward = true, bool bSaveDefault = false);
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_VOID_H
