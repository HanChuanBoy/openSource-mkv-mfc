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
	\version \$Id: KaxTag.h,v 1.13 2003/05/30 16:12:49 robux4 Exp $
	\author Jory Stone     <jcsston @ toughguy.net>
	\author Steve Lhomme   <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_TAG_H
#define LIBMATROSKA_TAG_H

#include "KaxTypes.h"
#include "EbmlMaster.h"
#include "EbmlFloat.h"
#include "EbmlSInteger.h"
#include "EbmlUInteger.h"
#include "EbmlString.h"
#include "EbmlUnicodeString.h"
#include "EbmlBinary.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxTag : public EbmlMaster {
	public:
		KaxTag();
		static EbmlElement & Create() {return *(new KaxTag);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagTargets : public EbmlMaster {
	public:
		KaxTagTargets();
		static EbmlElement & Create() {return *(new KaxTagTargets);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagGeneral : public EbmlMaster {
	public:
		KaxTagGeneral();
		static EbmlElement & Create() {return *(new KaxTagGeneral);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagGenres : public EbmlMaster {
	public:
		KaxTagGenres();
		static EbmlElement & Create() {return *(new KaxTagGenres);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagAudioSpecific : public EbmlMaster {
	public:
		KaxTagAudioSpecific();
		static EbmlElement & Create() {return *(new KaxTagAudioSpecific);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagImageSpecific : public EbmlMaster {
	public:
		KaxTagImageSpecific();
		static EbmlElement & Create() {return *(new KaxTagImageSpecific);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagTrackUID : public EbmlUInteger {
	public:
		KaxTagTrackUID() :EbmlUInteger(0) {}
		static EbmlElement & Create() {return *(new KaxTagTrackUID);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagChapterUID : public EbmlUInteger {
	public:
		KaxTagChapterUID() :EbmlUInteger(0) {}
		static EbmlElement & Create() {return *(new KaxTagChapterUID);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagArchivalLocation : public EbmlUnicodeString {
	public:
		KaxTagArchivalLocation() {}
		static EbmlElement & Create() {return *(new KaxTagArchivalLocation);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagAudioEncryption : public EbmlBinary {
	public:
		KaxTagAudioEncryption() {}
		static EbmlElement & Create() {return *(new KaxTagAudioEncryption);}
		bool ValidateSize() const {return true;} // we don't mind about what's inside
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagAudioGain : public EbmlFloat {
	public:
		KaxTagAudioGain() {}
		static EbmlElement & Create() {return *(new KaxTagAudioGain);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagAudioGenre : public EbmlString {
	public:
		KaxTagAudioGenre() {}
		static EbmlElement & Create() {return *(new KaxTagAudioGenre);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagAudioPeak : public EbmlFloat {
	public:
		KaxTagAudioPeak() {}
		static EbmlElement & Create() {return *(new KaxTagAudioPeak);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagBibliography : public EbmlUnicodeString {
	public:
		KaxTagBibliography() {}
		static EbmlElement & Create() {return *(new KaxTagBibliography);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagBPM : public EbmlFloat {
	public:
		KaxTagBPM() {}
		static EbmlElement & Create() {return *(new KaxTagBPM);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagCaptureDPI : public EbmlUInteger {
	public:
		KaxTagCaptureDPI() {}
		static EbmlElement & Create() {return *(new KaxTagCaptureDPI);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagCaptureLightness : public EbmlBinary {
	public:
		KaxTagCaptureLightness() {}
		static EbmlElement & Create() {return *(new KaxTagCaptureLightness);}
		bool ValidateSize() const {return true;} // we don't mind about what's inside
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagCapturePaletteSetting : public EbmlUInteger {
	public:
		KaxTagCapturePaletteSetting() {}
		static EbmlElement & Create() {return *(new KaxTagCapturePaletteSetting);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagCaptureSharpness : public EbmlBinary {
	public:
		KaxTagCaptureSharpness() {}
		static EbmlElement & Create() {return *(new KaxTagCaptureSharpness);}
		bool ValidateSize() const {return true;} // we don't mind about what's inside
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagCropped : public EbmlUnicodeString {
	public:
		KaxTagCropped() {}
		static EbmlElement & Create() {return *(new KaxTagCropped);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagDiscTrack : public EbmlUInteger {
	public:
		KaxTagDiscTrack() {}
		static EbmlElement & Create() {return *(new KaxTagDiscTrack);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagEncoder : public EbmlUnicodeString {
	public:
		KaxTagEncoder() {}
		static EbmlElement & Create() {return *(new KaxTagEncoder);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagEncodeSettings : public EbmlUnicodeString {
	public:
		KaxTagEncodeSettings() {}
		static EbmlElement & Create() {return *(new KaxTagEncodeSettings);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagEqualisation : public EbmlBinary {
	public:
		KaxTagEqualisation() {}
		static EbmlElement & Create() {return *(new KaxTagEqualisation);}
		bool ValidateSize() const {return true;} // we don't mind about what's inside
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagFile : public EbmlUnicodeString {
	public:
		KaxTagFile() {}
		static EbmlElement & Create() {return *(new KaxTagFile);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagInitialKey : public EbmlString {
	public:
		KaxTagInitialKey() {}
		static EbmlElement & Create() {return *(new KaxTagInitialKey);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagKeywords : public EbmlUnicodeString {
	public:
		KaxTagKeywords() {}
		static EbmlElement & Create() {return *(new KaxTagKeywords);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagLanguage : public EbmlString {
	public:
		KaxTagLanguage() {}
		static EbmlElement & Create() {return *(new KaxTagLanguage);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagLength : public EbmlUInteger {
	public:
		KaxTagLength() {}
		static EbmlElement & Create() {return *(new KaxTagLength);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMood : public EbmlUnicodeString {
	public:
		KaxTagMood() {}
		static EbmlElement & Create() {return *(new KaxTagMood);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagOfficialAudioFileURL : public EbmlString {
	public:
		KaxTagOfficialAudioFileURL() {}
		static EbmlElement & Create() {return *(new KaxTagOfficialAudioFileURL);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagOfficialAudioSourceURL : public EbmlString {
	public:
		KaxTagOfficialAudioSourceURL() {}
		static EbmlElement & Create() {return *(new KaxTagOfficialAudioSourceURL);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagOriginalDimensions : public EbmlString {
	public:
		KaxTagOriginalDimensions() {}
		static EbmlElement & Create() {return *(new KaxTagOriginalDimensions);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagOriginalMediaType : public EbmlUnicodeString {
	public:
		KaxTagOriginalMediaType() {}
		static EbmlElement & Create() {return *(new KaxTagOriginalMediaType);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagPlayCounter : public EbmlUInteger {
	public:
		KaxTagPlayCounter() {}
		static EbmlElement & Create() {return *(new KaxTagPlayCounter);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};			 

class KaxTagPlaylistDelay : public EbmlUInteger {
	public:
		KaxTagPlaylistDelay() {}
		static EbmlElement & Create() {return *(new KaxTagPlaylistDelay);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagPopularimeter : public EbmlSInteger {
	public:
		KaxTagPopularimeter() {}
		static EbmlElement & Create() {return *(new KaxTagPopularimeter);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagProduct : public EbmlUnicodeString {
	public:
		KaxTagProduct() {}
		static EbmlElement & Create() {return *(new KaxTagProduct);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagRating : public EbmlBinary {
	public:
		KaxTagRating() {}
		static EbmlElement & Create() {return *(new KaxTagRating);}
		bool ValidateSize() const {return true;} // we don't mind about what's inside
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagRecordLocation : public EbmlString {
	public:
		KaxTagRecordLocation() {}
		static EbmlElement & Create() {return *(new KaxTagRecordLocation);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagSetPart : public EbmlUInteger {
	public:
		KaxTagSetPart() {}
		static EbmlElement & Create() {return *(new KaxTagSetPart);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagSource : public EbmlUnicodeString {
	public:
		KaxTagSource() {}
		static EbmlElement & Create() {return *(new KaxTagSource);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagSourceForm : public EbmlUnicodeString {
	public:
		KaxTagSourceForm() {}
		static EbmlElement & Create() {return *(new KaxTagSourceForm);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagSubGenre : public EbmlString {
	public:
		KaxTagSubGenre() {}
		static EbmlElement & Create() {return *(new KaxTagSubGenre);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagSubject : public EbmlUnicodeString {
	public:
		KaxTagSubject() {}
		static EbmlElement & Create() {return *(new KaxTagSubject);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagUnsynchronisedText : public EbmlUnicodeString {
	public:
		KaxTagUnsynchronisedText() {}
		static EbmlElement & Create() {return *(new KaxTagUnsynchronisedText);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagUserDefinedURL : public EbmlString {
	public:
		KaxTagUserDefinedURL() {}
		static EbmlElement & Create() {return *(new KaxTagUserDefinedURL);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagVideoGenre : public EbmlBinary {
	public:
		KaxTagVideoGenre() {}
		static EbmlElement & Create() {return *(new KaxTagVideoGenre);}
		bool ValidateSize() const {return (Size >= 4);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_TAG_H
