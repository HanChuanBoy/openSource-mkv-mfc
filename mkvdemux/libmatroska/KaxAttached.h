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
	\version \$Id: KaxAttached.h,v 1.2 2003/04/28 10:06:10 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_ATTACHED_H
#define LIBMATROSKA_ATTACHED_H

#include "KaxTypes.h"
#include "EbmlMaster.h"
#include "EbmlUnicodeString.h"
#include "EbmlString.h"
#include "EbmlBinary.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxAttached : public EbmlMaster {
	public:
		KaxAttached();
		static EbmlElement & Create() {return *(new KaxAttached);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;

		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		bool IsYourId(const EbmlId & TestId) const;
};

class KaxFileDescription : public EbmlUnicodeString {
	public:
		KaxFileDescription() {}
		static EbmlElement & Create() {return *(new KaxFileDescription);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;

		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		bool IsYourId(const EbmlId & TestId) const;
};

class KaxFileName : public EbmlUnicodeString {
	public:
		KaxFileName() {}
		static EbmlElement & Create() {return *(new KaxFileName);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;

		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		bool IsYourId(const EbmlId & TestId) const;
};

class KaxMimeType : public EbmlString {
	public:
		KaxMimeType() {}
		static EbmlElement & Create() {return *(new KaxMimeType);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;

		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		bool IsYourId(const EbmlId & TestId) const;
};

class KaxFileData : public EbmlBinary {
	public:
		KaxFileData() {}
		static EbmlElement & Create() {return *(new KaxFileData);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		bool ValidateSize() const {return true;} // we don't mind about what's inside
		static const EbmlCallbacks ClassInfos;

		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		bool IsYourId(const EbmlId & TestId) const;
};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_ATTACHED_H
