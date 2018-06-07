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
	\version \$Id: KaxCuesData.cpp,v 1.11 2003/06/06 23:31:30 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include <cassert>

#include "StdInclude.h"
#include "KaxCuesData.h"
#include "KaxContexts.h"
#include "KaxBlock.h"
#include "KaxBlockData.h"

START_LIBMATROSKA_NAMESPACE

EbmlSemantic KaxCuePoint_ContextList[2] =
{
	EbmlSemantic(true,  true,  KaxCueTime::ClassInfos),
	EbmlSemantic(true,  false, KaxCueTrackPositions::ClassInfos),
};

EbmlSemantic KaxCueTrackPositions_ContextList[5] =
{
	EbmlSemantic(true,  true,  KaxCueTrack::ClassInfos),
	EbmlSemantic(true,  true,  KaxCueClusterPosition::ClassInfos),
	EbmlSemantic(false, true,  KaxCueBlockNumber::ClassInfos),
	EbmlSemantic(false, true,  KaxCueCodecState::ClassInfos),
	EbmlSemantic(false, false, KaxCueReference::ClassInfos),
};

EbmlSemantic KaxCueReference_ContextList[4] =
{
	EbmlSemantic(true,  true,  KaxCueRefTime::ClassInfos),
	EbmlSemantic(true,  true,  KaxCueRefCluster::ClassInfos),
	EbmlSemantic(false, true,  KaxCueRefNumber::ClassInfos),
	EbmlSemantic(false, true,  KaxCueRefCodecState::ClassInfos),
};

EbmlId KaxCuePoint_TheId          (0xBB, 1);
EbmlId KaxCueTime_TheId           (0xB3, 1);
EbmlId KaxCueTrackPositions_TheId (0xB7, 1);
EbmlId KaxCueTrack_TheId          (0xF7, 1);
EbmlId KaxCueClusterPosition_TheId(0xF1, 1);
EbmlId KaxCueBlockNumber_TheId    (0x5387, 2);
EbmlId KaxCueCodecState_TheId     (0xEA, 1);
EbmlId KaxCueReference_TheId      (0xDB, 1);
EbmlId KaxCueRefTime_TheId        (0x96, 1);
EbmlId KaxCueRefCluster_TheId     (0x97, 1);
EbmlId KaxCueRefNumber_TheId      (0x535F, 2);
EbmlId KaxCueRefCodecState_TheId  (0xEB, 1);

const EbmlSemanticContext KaxCuePoint_Context = EbmlSemanticContext(countof(KaxCuePoint_ContextList), KaxCuePoint_ContextList, &KaxCues_Context, *GetKaxGlobal_Context, &KaxCuePoint::ClassInfos);
const EbmlSemanticContext KaxCueTime_Context = EbmlSemanticContext(0, NULL, &KaxCuePoint_Context, *GetKaxGlobal_Context, &KaxCueTime::ClassInfos);
const EbmlSemanticContext KaxCueTrackPositions_Context = EbmlSemanticContext(countof(KaxCueTrackPositions_ContextList), KaxCueTrackPositions_ContextList, &KaxCuePoint_Context, *GetKaxGlobal_Context, &KaxCueTrackPositions::ClassInfos);
const EbmlSemanticContext KaxCueTrack_Context = EbmlSemanticContext(0, NULL, &KaxCueTrackPositions_Context, *GetKaxGlobal_Context, &KaxCueTrack::ClassInfos);
const EbmlSemanticContext KaxCueClusterPosition_Context = EbmlSemanticContext(0, NULL, &KaxCueTrackPositions_Context, *GetKaxGlobal_Context, &KaxCueClusterPosition::ClassInfos);
const EbmlSemanticContext KaxCueBlockNumber_Context = EbmlSemanticContext(0, NULL, &KaxCueTrackPositions_Context, *GetKaxGlobal_Context, &KaxCueBlockNumber::ClassInfos);
const EbmlSemanticContext KaxCueCodecState_Context = EbmlSemanticContext(0, NULL, &KaxCueTrackPositions_Context, *GetKaxGlobal_Context, &KaxCueCodecState::ClassInfos);
const EbmlSemanticContext KaxCueReference_Context = EbmlSemanticContext(countof(KaxCueReference_ContextList), KaxCueReference_ContextList, &KaxCueTrackPositions_Context, *GetKaxGlobal_Context, &KaxCueReference::ClassInfos);
const EbmlSemanticContext KaxCueRefTime_Context = EbmlSemanticContext(0, NULL, &KaxCueReference_Context, *GetKaxGlobal_Context, &KaxCueRefTime::ClassInfos);
const EbmlSemanticContext KaxCueRefCluster_Context = EbmlSemanticContext(0, NULL, &KaxCueRefTime_Context, *GetKaxGlobal_Context, &KaxCueRefCluster::ClassInfos);
const EbmlSemanticContext KaxCueRefNumber_Context = EbmlSemanticContext(0, NULL, &KaxCueRefTime_Context, *GetKaxGlobal_Context, &KaxCueRefNumber::ClassInfos);
const EbmlSemanticContext KaxCueRefCodecState_Context = EbmlSemanticContext(0, NULL, &KaxCueRefTime_Context, *GetKaxGlobal_Context, &KaxCueRefCodecState::ClassInfos);

const EbmlCallbacks KaxCuePoint::ClassInfos(KaxCuePoint::Create, KaxCuePoint_TheId, "CuePoint", KaxCuePoint_Context);
const EbmlCallbacks KaxCueTime::ClassInfos(KaxCueTime::Create, KaxCueTime_TheId, "CueTime", KaxCueTime_Context);
const EbmlCallbacks KaxCueTrackPositions::ClassInfos(KaxCueTrackPositions::Create, KaxCueTrackPositions_TheId, "CueTrackPositions", KaxCueTrackPositions_Context);
const EbmlCallbacks KaxCueTrack::ClassInfos(KaxCueTrack::Create, KaxCueTrack_TheId, "CueTrack", KaxCueTrack_Context);
const EbmlCallbacks KaxCueClusterPosition::ClassInfos(KaxCueClusterPosition::Create, KaxCueClusterPosition_TheId, "CueClusterPosition", KaxCueClusterPosition_Context);
const EbmlCallbacks KaxCueBlockNumber::ClassInfos(KaxCueBlockNumber::Create, KaxCueBlockNumber_TheId, "CueBlockNumber", KaxCueBlockNumber_Context);
const EbmlCallbacks KaxCueCodecState::ClassInfos(KaxCueCodecState::Create, KaxCueCodecState_TheId, "CueCodecState", KaxCueCodecState_Context);
const EbmlCallbacks KaxCueReference::ClassInfos(KaxCueReference::Create, KaxCueReference_TheId, "CueReference", KaxCueReference_Context);
const EbmlCallbacks KaxCueRefTime::ClassInfos(KaxCueRefTime::Create, KaxCueRefTime_TheId, "CueRefTime", KaxCueRefTime_Context);
const EbmlCallbacks KaxCueRefCluster::ClassInfos(KaxCueRefCluster::Create, KaxCueRefCluster_TheId, "CueRefCluster", KaxCueRefCluster_Context);
const EbmlCallbacks KaxCueRefNumber::ClassInfos(KaxCueRefNumber::Create, KaxCueRefNumber_TheId, "CueRefNumber", KaxCueRefNumber_Context);
const EbmlCallbacks KaxCueRefCodecState::ClassInfos(KaxCueRefCodecState::Create, KaxCueRefCodecState_TheId, "CueRefCodecState", KaxCueRefCodecState_Context);

KaxCuePoint::KaxCuePoint() 
 :EbmlMaster(KaxCuePoint_Context)
{}

KaxCueTrackPositions::KaxCueTrackPositions()
 :EbmlMaster(KaxCueTrackPositions_Context)
{}

KaxCueReference::KaxCueReference()
 :EbmlMaster(KaxCueReference_Context)
{}

/*!
	\todo handle codec state checking
	\todo remove duplicate references (reference to 2 frames that each reference the same frame)
*/
void KaxCuePoint::PositionSet(const KaxBlockGroup & BlockReference, uint64 GlobalTimecodeScale)
{
	// fill me
	KaxCueTime & NewTime = GetChild<KaxCueTime>(*this);
	*static_cast<EbmlUInteger*>(&NewTime) = BlockReference.GlobalTimecode() / GlobalTimecodeScale;

	KaxCueTrackPositions & NewPositions = AddNewChild<KaxCueTrackPositions>(*this);
	KaxCueTrack & TheTrack = GetChild<KaxCueTrack>(NewPositions);
	*static_cast<EbmlUInteger*>(&TheTrack) = BlockReference.TrackNumber();
	
	KaxCueClusterPosition & TheClustPos = GetChild<KaxCueClusterPosition>(NewPositions);
	*static_cast<EbmlUInteger*>(&TheClustPos) = BlockReference.ClusterPosition();

	// handle reference use
	if (BlockReference.ReferenceCount() != 0)
	{
		unsigned int i;
		for (i=0; i<BlockReference.ReferenceCount(); i++) {
			KaxCueReference & NewRefs = AddNewChild<KaxCueReference>(NewPositions);
			NewRefs.AddReference(BlockReference.Reference(i).RefBlock(), GlobalTimecodeScale);
		}
	}
   bValueIsSet = true;
}

/*!
	\todo handle codec state checking
*/
void KaxCueReference::AddReference(const KaxBlockGroup & BlockReference, uint64 GlobalTimecodeScale)
{
	KaxCueRefTime & NewTime = GetChild<KaxCueRefTime>(*this);
	*static_cast<EbmlUInteger*>(&NewTime) = BlockReference.GlobalTimecode() / GlobalTimecodeScale;

	KaxCueRefCluster & TheClustPos = GetChild<KaxCueRefCluster>(*this);
	*static_cast<EbmlUInteger*>(&TheClustPos) = BlockReference.ClusterPosition();

#ifdef OLD
	// handle recursive reference use
	if (BlockReference.ReferenceCount() != 0)
	{
		unsigned int i;
		for (i=0; i<BlockReference.ReferenceCount(); i++) {
			AddReference(BlockReference.Reference(i).RefBlock());
		}
	}
#endif /* OLD */
}

bool KaxCuePoint::operator<(const EbmlElement & EltB) const
{
	assert(EbmlId(*this) == KaxCuePoint_TheId);
	assert(EbmlId(EltB) == KaxCuePoint_TheId);

	const KaxCuePoint & theEltB = *static_cast<const KaxCuePoint *>(&EltB);

	// compare timecode
	const KaxCueTime * TimeCodeA = static_cast<const KaxCueTime *>(FindElt(KaxCueTime::ClassInfos));
	if (TimeCodeA == NULL)
		return false;

	const KaxCueTime * TimeCodeB = static_cast<const KaxCueTime *>(theEltB.FindElt(KaxCueTime::ClassInfos));
	if (TimeCodeB == NULL)
		return false;

	if (*TimeCodeA < *TimeCodeB)
		return true;

	if (*TimeCodeB < *TimeCodeA)
		return false;

	// compare tracks (timecodes are equal)
	const KaxCueTrack * TrackA = static_cast<const KaxCueTrack *>(FindElt(KaxCueTrack::ClassInfos));
	if (TrackA == NULL)
		return false;

	const KaxCueTrack * TrackB = static_cast<const KaxCueTrack *>(theEltB.FindElt(KaxCueTrack::ClassInfos));
	if (TrackB == NULL)
		return false;

	if (*TrackA < *TrackB)
		return true;

	if (*TrackB < *TrackA)
		return false;

	return false;
}

bool KaxCuePoint::Timecode(uint64 & aTimecode, uint64 GlobalTimecodeScale) const
{
	const KaxCueTime *aTime = static_cast<const KaxCueTime *>(FindFirstElt(KaxCueTime::ClassInfos));
	if (aTime == NULL)
		return false;
	aTimecode = uint64(*aTime) * GlobalTimecodeScale;
	return true;
}

/*!
	\brief return the position of the Cluster to load
*/
const KaxCueTrackPositions * KaxCuePoint::GetSeekPosition() const
{
	const KaxCueTrackPositions * result = NULL;
	uint64 aPosition = EBML_PRETTYLONGINT(0xFFFFFFFFFFFFFFF);
	// find the position of the "earlier" Cluster
	const KaxCueTrackPositions *aPoss = static_cast<const KaxCueTrackPositions *>(FindFirstElt(KaxCueTrackPositions::ClassInfos));
	while (aPoss != NULL)
	{
		const KaxCueClusterPosition *aPos = static_cast<const KaxCueClusterPosition *>(aPoss->FindFirstElt(KaxCueClusterPosition::ClassInfos));
		if (aPos != NULL && uint64(*aPos) < aPosition) {
			aPosition = uint64(*aPos);
			result = aPoss;
		}
		
		aPoss = static_cast<const KaxCueTrackPositions *>(FindNextElt(*aPoss));
	}
	return result;
}

uint64 KaxCueTrackPositions::ClusterPosition() const
{
	const KaxCueClusterPosition *aPos = static_cast<const KaxCueClusterPosition *>(FindFirstElt(KaxCueClusterPosition::ClassInfos));
	if (aPos == NULL)
		return 0;

	return uint64(*aPos);
}

uint16 KaxCueTrackPositions::TrackNumber() const
{
	const KaxCueTrack *aTrack = static_cast<const KaxCueTrack *>(FindFirstElt(KaxCueTrack::ClassInfos));
	if (aTrack == NULL)
		return 0;

	return uint16(*aTrack);
}


END_LIBMATROSKA_NAMESPACE
