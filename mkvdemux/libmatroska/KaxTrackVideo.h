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
	\version \$Id: KaxTrackVideo.h,v 1.5 2003/06/05 18:34:19 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_TRACK_VIDEO_H
#define LIBMATROSKA_TRACK_VIDEO_H

#include "KaxTypes.h"
#include "EbmlMaster.h"
#include "EbmlUInteger.h"
#include "EbmlBinary.h"
#include "EbmlFloat.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxTrackVideo : public EbmlMaster {
	public:
		KaxTrackVideo();
		static EbmlElement & Create() {return *(new KaxTrackVideo);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoFlagInterlaced : public EbmlUInteger {
	public:
		KaxVideoFlagInterlaced() :EbmlUInteger(0) {}
		static EbmlElement & Create() {return *(new KaxVideoFlagInterlaced);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoStereoMode : public EbmlUInteger {
	public:
		KaxVideoStereoMode() :EbmlUInteger(0) {}
		static EbmlElement & Create() {return *(new KaxVideoStereoMode);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoPixelWidth : public EbmlUInteger {
	public:
		KaxVideoPixelWidth() {}
		static EbmlElement & Create() {return *(new KaxVideoPixelWidth);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoPixelHeight : public EbmlUInteger {
	public:
		KaxVideoPixelHeight() {}
		static EbmlElement & Create() {return *(new KaxVideoPixelHeight);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoDisplayWidth : public EbmlUInteger {
	public:
		KaxVideoDisplayWidth() {}
		static EbmlElement & Create() {return *(new KaxVideoDisplayWidth);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoDisplayHeight : public EbmlUInteger {
	public:
		KaxVideoDisplayHeight() {}
		static EbmlElement & Create() {return *(new KaxVideoDisplayHeight);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoDisplayUnit : public EbmlUInteger {
	public:
		KaxVideoDisplayUnit() {}
		static EbmlElement & Create() {return *(new KaxVideoDisplayUnit);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoAspectRatio : public EbmlUInteger {
	public:
		KaxVideoAspectRatio() {}
		static EbmlElement & Create() {return *(new KaxVideoAspectRatio);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoColourSpace : public EbmlBinary {
	public:
		KaxVideoColourSpace() {}
		static EbmlElement & Create() {return *(new KaxVideoColourSpace);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		bool ValidateSize(void) const {return (Size == 4);}
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoGamma : public EbmlFloat {
	public:
		KaxVideoGamma() {}
		static EbmlElement & Create() {return *(new KaxVideoGamma);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxVideoFrameRate : public EbmlFloat {
	public:
		KaxVideoFrameRate() {}
		static EbmlElement & Create() {return *(new KaxVideoFrameRate);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		uint32 RenderData(IOCallback & output, bool bSaveDefault);
};


END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_TRACK_VIDEO_H
