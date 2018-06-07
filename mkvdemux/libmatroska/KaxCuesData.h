/****************************************************************************
** libmatroska : parse Matroska files, see http://www.matroska.org/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Steve Lhomme.  All rights reserved.
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
	\version \$Id: KaxCuesData.h,v 1.5 2003/06/06 23:31:30 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_CUES_DATA_H
#define LIBMATROSKA_CUES_DATA_H

#include "KaxTypes.h"
#include "EbmlUInteger.h"
#include "EbmlMaster.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxBlockGroup;
class KaxCueTrackPositions;

class KaxCuePoint : public EbmlMaster {
	public:
		KaxCuePoint();
		static EbmlElement & Create() {return *(new KaxCuePoint);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		void PositionSet(const KaxBlockGroup & BlockReference, uint64 GlobalTimecodeScale);

		bool operator<(const EbmlElement & EltB) const;

		const KaxCueTrackPositions * GetSeekPosition() const;
		bool Timecode(uint64 & aTimecode, uint64 GlobalTimecodeScale) const;
};

class KaxCueTime : public EbmlUInteger {
	public:
		KaxCueTime() {}
		static EbmlElement & Create() {return *(new KaxCueTime);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCueTrackPositions : public EbmlMaster {
	public:
		KaxCueTrackPositions();
		static EbmlElement & Create() {return *(new KaxCueTrackPositions);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}

		uint64 ClusterPosition() const;
		uint16 TrackNumber() const;
};

class KaxCueTrack : public EbmlUInteger {
	public:
		KaxCueTrack() {}
		static EbmlElement & Create() {return *(new KaxCueTrack);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCueClusterPosition : public EbmlUInteger {
	public:
		KaxCueClusterPosition() {}
		static EbmlElement & Create() {return *(new KaxCueClusterPosition);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCueBlockNumber : public EbmlUInteger {
	public:
		KaxCueBlockNumber() :EbmlUInteger(1) {}
		static EbmlElement & Create() {return *(new KaxCueBlockNumber);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCueCodecState : public EbmlUInteger {
	public:
		KaxCueCodecState() :EbmlUInteger(0) {}
		static EbmlElement & Create() {return *(new KaxCueCodecState);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCueReference : public EbmlMaster {
	public:
		KaxCueReference();
		static EbmlElement & Create() {return *(new KaxCueReference);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		
		void AddReference(const KaxBlockGroup & BlockReferenced, uint64 GlobalTimecodeScale);
};

class KaxCueRefTime : public EbmlUInteger {
	public:
		KaxCueRefTime() {}
		static EbmlElement & Create() {return *(new KaxCueRefTime);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCueRefCluster : public EbmlUInteger {
	public:
		KaxCueRefCluster() {}
		static EbmlElement & Create() {return *(new KaxCueRefCluster);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCueRefNumber : public EbmlUInteger {
	public:
		KaxCueRefNumber() :EbmlUInteger(1) {}
		static EbmlElement & Create() {return *(new KaxCueRefNumber);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxCueRefCodecState : public EbmlUInteger {
	public:
		KaxCueRefCodecState() :EbmlUInteger(0) {}
		static EbmlElement & Create() {return *(new KaxCueRefCodecState);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_CUES_DATA_H
