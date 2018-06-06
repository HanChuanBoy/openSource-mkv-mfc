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
	\version \$Id: EbmlSubHead.h,v 1.1 2003/04/03 16:40:48 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBEBML_SUBHEAD_H
#define LIBEBML_SUBHEAD_H

#include <string>

#include "EbmlUInteger.h"
#include "EbmlString.h"

START_LIBEBML_NAMESPACE

class EVersion : public EbmlUInteger {
	public:
		EVersion() :EbmlUInteger(1) {}
		static EbmlElement & Create() {return *(new EVersion);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class EReadVersion : public EbmlUInteger {
	public:
		EReadVersion() :EbmlUInteger(1) {}
		static EbmlElement & Create() {return *(new EReadVersion);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class EMaxIdLength : public EbmlUInteger {
	public:
		EMaxIdLength() :EbmlUInteger(4) {}
		static EbmlElement & Create() {return *(new EMaxIdLength);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class EMaxSizeLength : public EbmlUInteger {
	public:
		EMaxSizeLength() :EbmlUInteger(8) {}
		static EbmlElement & Create() {return *(new EMaxSizeLength);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class EDocType : public EbmlString {
	public:
		EDocType() {}
		static EbmlElement & Create() {return *(new EDocType);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class EDocTypeVersion : public EbmlUInteger {
	public:
		EDocTypeVersion() {}
		static EbmlElement & Create() {return *(new EDocTypeVersion);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class EDocTypeReadVersion : public EbmlUInteger {
	public:
		EDocTypeReadVersion() {}
		static EbmlElement & Create() {return *(new EDocTypeReadVersion);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_SUBHEAD_H
