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
	\version \$Id: KaxTrackAudio.h,v 1.4 2003/05/21 20:28:49 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_TRACK_AUDIO_H
#define LIBMATROSKA_TRACK_AUDIO_H

#include "KaxTypes.h"
#include "EbmlMaster.h"
#include "EbmlFloat.h"
#include "EbmlUInteger.h"
#include "EbmlBinary.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxTrackAudio : public EbmlMaster {
	public:
		KaxTrackAudio();
		static EbmlElement & Create() {return *(new KaxTrackAudio);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxAudioSamplingFreq : public EbmlFloat {
	public:
		KaxAudioSamplingFreq() :EbmlFloat(8000.0) {}
		static EbmlElement & Create() {return *(new KaxAudioSamplingFreq);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxAudioChannels : public EbmlUInteger {
	public:
		KaxAudioChannels() :EbmlUInteger(1) {}
		static EbmlElement & Create() {return *(new KaxAudioChannels);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxAudioPosition : public EbmlBinary {
	public:
		KaxAudioPosition() {}
		static EbmlElement & Create() {return *(new KaxAudioPosition);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		bool ValidateSize(void) const {return true;}
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxAudioBitDepth : public EbmlUInteger {
	public:
		KaxAudioBitDepth() {}
		static EbmlElement & Create() {return *(new KaxAudioBitDepth);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_TRACK_AUDIO_H
