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
	\version \$Id: KaxTrackEntryData.h,v 1.7 2003/06/07 11:54:06 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author John Cannon      <spyder2555 @ users.sf.net>
*/
#ifndef LIBMATROSKA_TRACK_ENTRY_DATA_H
#define LIBMATROSKA_TRACK_ENTRY_DATA_H

#include "KaxTypes.h"
#include "EbmlUInteger.h"
#include "EbmlFloat.h"
#include "EbmlString.h"
#include "EbmlUnicodeString.h"
#include "EbmlBinary.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxTrackNumber : public EbmlUInteger {
	public:
		KaxTrackNumber() {}
		static EbmlElement & Create() {return *(new KaxTrackNumber);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackUID : public EbmlUInteger {
	public:
		KaxTrackUID() {}
		static EbmlElement & Create() {return *(new KaxTrackUID);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackType : public EbmlUInteger {
	public:
		KaxTrackType() {}
		static EbmlElement & Create() {return *(new KaxTrackType);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackFlagEnabled : public EbmlUInteger {
	public:
		KaxTrackFlagEnabled() :EbmlUInteger(1) {}
		static EbmlElement & Create() {return *(new KaxTrackFlagEnabled);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackFlagDefault : public EbmlUInteger {
	public:
		KaxTrackFlagDefault() :EbmlUInteger(1) {}
		static EbmlElement & Create() {return *(new KaxTrackFlagDefault);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackFlagLacing : public EbmlUInteger {
	public:
		KaxTrackFlagLacing() :EbmlUInteger(1) {}
		static EbmlElement & Create() {return *(new KaxTrackFlagLacing);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackMinCache : public EbmlUInteger {
	public:
		KaxTrackMinCache() :EbmlUInteger(2) {}
		static EbmlElement & Create() {return *(new KaxTrackMinCache);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackMaxCache : public EbmlUInteger {
	public:
		KaxTrackMaxCache() {}
		static EbmlElement & Create() {return *(new KaxTrackMaxCache);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackDefaultDuration : public EbmlUInteger {
	public:
		KaxTrackDefaultDuration() {}
		static EbmlElement & Create() {return *(new KaxTrackDefaultDuration);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackTimecodeScale : public EbmlFloat {
	public:
		KaxTrackTimecodeScale() :EbmlFloat(1.0) {}
		static EbmlElement & Create() {return *(new KaxTrackTimecodeScale);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackName : public EbmlUnicodeString {
	public:
		KaxTrackName() {}
		static EbmlElement & Create() {return *(new KaxTrackName);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackLanguage : public EbmlString {
	public:
		KaxTrackLanguage() :EbmlString("eng") {}
		static EbmlElement & Create() {return *(new KaxTrackLanguage);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCodecID : public EbmlString {
	public:
		KaxCodecID() {}
		static EbmlElement & Create() {return *(new KaxCodecID);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCodecPrivate : public EbmlBinary {
	public:
		KaxCodecPrivate() {}
		static EbmlElement & Create() {return *(new KaxCodecPrivate);}
		bool ValidateSize() const {return true;} // we don't mind about what's inside
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCodecName : public EbmlUnicodeString {
	public:
		KaxCodecName() {}
		static EbmlElement & Create() {return *(new KaxCodecName);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCodecSettings : public EbmlUnicodeString {
	public:
		KaxCodecSettings() {}
		static EbmlElement & Create() {return *(new KaxCodecSettings);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCodecInfoURL : public EbmlString {
	public:
		KaxCodecInfoURL() {}
		static EbmlElement & Create() {return *(new KaxCodecInfoURL);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCodecDownloadURL : public EbmlString {
	public:
		KaxCodecDownloadURL() {}
		static EbmlElement & Create() {return *(new KaxCodecDownloadURL);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCodecDecodeAll : public EbmlUInteger {
	public:
		KaxCodecDecodeAll() :EbmlUInteger(1) {}
		static EbmlElement & Create() {return *(new KaxCodecDecodeAll);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTrackOverlay : public EbmlUInteger {
	public:
		KaxTrackOverlay() {}
		static EbmlElement & Create() {return *(new KaxTrackOverlay);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_TRACK_ENTRY_DATA_H
