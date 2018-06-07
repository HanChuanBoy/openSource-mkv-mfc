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
	\version \$Id: KaxSeekHead.h,v 1.8 2003/05/21 20:28:49 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_SEEK_HEAD_H
#define LIBMATROSKA_SEEK_HEAD_H

#include "KaxTypes.h"
#include "EbmlMaster.h"
#include "EbmlBinary.h"
#include "EbmlUInteger.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxSegment;
class KaxSeek;

class KaxSeekHead : public EbmlMaster {
	public:
		KaxSeekHead();
		static EbmlElement & Create() {return *(new KaxSeekHead);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}

		/*!
			\brief add an element to index in the Meta Seek data
			\note the element should already be written in the file
		*/
		void IndexThis(const EbmlElement & aElt, const KaxSegment & ParentSegment);

		KaxSeek * FindFirstOf(const EbmlCallbacks & Callbacks) const;
		KaxSeek * FindNextOf(const KaxSeek &aPrev) const;
};

class KaxSeek : public EbmlMaster {
	public:
		KaxSeek();
		static EbmlElement & Create() {return *(new KaxSeek);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}

		int64 Location() const;
		bool IsEbmlId(const EbmlId & aId) const;
		bool IsEbmlId(const KaxSeek & aPoint) const;
};

class KaxSeekID : public EbmlBinary {
	public:
		KaxSeekID() {}
		static EbmlElement & Create() {return *(new KaxSeekID);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		bool ValidateSize() const {return Size <= 4;}
};

class KaxSeekPosition : public EbmlUInteger {
	public:
		KaxSeekPosition() {}
		static EbmlElement & Create() {return *(new KaxSeekPosition);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_SEEK_HEAD_H
