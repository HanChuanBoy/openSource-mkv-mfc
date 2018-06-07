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
	\version \$Id: KaxInfoData.h,v 1.4 2003/05/21 20:28:49 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author John Cannon      <spyder2555 @ users.sf.net>
	\author Moritz Bunkus    <moritz @ bunkus.org>
*/
#ifndef LIBMATROSKA_INFO_DATA_H
#define LIBMATROSKA_INFO_DATA_H

#include "KaxTypes.h"
#include "EbmlUInteger.h"
#include "EbmlString.h"
#include "EbmlFloat.h"
#include "EbmlUnicodeString.h"
#include "EbmlBinary.h"
#include "EbmlDate.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxSegmentUID : public EbmlBinary {
	public:
		KaxSegmentUID() {}
		static EbmlElement & Create() {return *(new KaxSegmentUID);}
		bool ValidateSize() const { return (Size == 16);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxSegmentFilename : public EbmlUnicodeString {
	public:
		KaxSegmentFilename() {}
		static EbmlElement & Create() {return *(new KaxSegmentFilename);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxPrevUID : public EbmlBinary {
	public:
		KaxPrevUID() {}
		static EbmlElement & Create() {return *(new KaxPrevUID);}
		bool ValidateSize() const { return (Size == 16);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxPrevFilename : public EbmlUnicodeString {
	public:
		KaxPrevFilename() :EbmlUnicodeString() {}
		static EbmlElement & Create() {return *(new KaxPrevFilename);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxNextUID : public EbmlBinary {
	public:
		KaxNextUID() {}
		static EbmlElement & Create() {return *(new KaxNextUID);}
		bool ValidateSize() const { return (Size == 16);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxNextFilename : public EbmlUnicodeString {
	public:
		KaxNextFilename() {}
		static EbmlElement & Create() {return *(new KaxNextFilename);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTimecodeScale : public EbmlUInteger {
	public:
		KaxTimecodeScale() :EbmlUInteger(1000000) {}
		static EbmlElement & Create() {return *(new KaxTimecodeScale);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxDuration : public EbmlFloat {
	public:
		KaxDuration() {}
		static EbmlElement & Create() {return *(new KaxDuration);}		
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxDateUTC : public EbmlDate {
	public:
		KaxDateUTC() {}
		static EbmlElement & Create() {return *(new KaxDateUTC);}		
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTitle : public EbmlUnicodeString {
	public:
		KaxTitle() {}
		static EbmlElement & Create() {return *(new KaxTitle);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_INFO_DATA_H
