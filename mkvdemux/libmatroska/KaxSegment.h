/****************************************************************************
** libmatroska : parse Matroska files, see http://www.matroska.org/
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
	\version \$Id: KaxSegment.h,v 1.4 2003/04/28 10:06:10 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_SEGMENT_H
#define LIBMATROSKA_SEGMENT_H

#include "KaxTypes.h"
#include "EbmlMaster.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxSegment : public EbmlMaster {
	public:
		KaxSegment();
		static EbmlElement & Create() {return *(new KaxSegment);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}

		/*!
			\brief give the position of the element in the segment
		*/
		uint64 GetRelativePosition(const EbmlElement & Elt) const;
		uint64 GetRelativePosition(uint64 aGlobalPosition) const;

		/*!
			\brief give the position of the element in the file
		*/
		uint64 GetGlobalPosition(uint64 aRelativePosition) const;
};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_SEGMENT_H
