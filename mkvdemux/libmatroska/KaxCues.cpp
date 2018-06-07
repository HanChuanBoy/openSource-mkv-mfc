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
	\version \$Id: KaxCues.cpp,v 1.10 2003/06/06 23:31:30 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include <cassert>

#include "StdInclude.h"
#include "KaxCues.h"
#include "KaxCuesData.h"
#include "KaxContexts.h"
#include "EbmlStream.h"

// sub elements
START_LIBMATROSKA_NAMESPACE

EbmlSemantic KaxCues_ContextList[1] = 
{
	EbmlSemantic(true,  false,  KaxCuePoint::ClassInfos),
};

const EbmlSemanticContext KaxCues_Context = EbmlSemanticContext(countof(KaxCues_ContextList), KaxCues_ContextList, &KaxSegment_Context, *GetKaxGlobal_Context, &KaxCues::ClassInfos);

EbmlId KaxCues_TheId(0x1C53BB6B, 4);
const EbmlCallbacks KaxCues::ClassInfos(KaxCues::Create, KaxCues_TheId, "Cues", KaxCues_Context);

KaxCues::KaxCues()
	:EbmlMaster(KaxCues_Context)
{}

KaxCues::~KaxCues()
{
	assert(myTempReferences.size() == 0); // otherwise that means you have added references and forgot to set the position
}

bool KaxCues::AddBlockGroup(const KaxBlockGroup & BlockReference)
{
	myTempReferences.push_back(&BlockReference);
	return true;
}

void KaxCues::PositionSet(const KaxBlockGroup & BlockReference)
{
	// look for the element in the temporary references
	std::vector<const KaxBlockGroup *>::iterator ListIdx;

	for (ListIdx = myTempReferences.begin(); ListIdx != myTempReferences.end(); ListIdx++) {
		if (*ListIdx == &BlockReference) {
			// found, now add the element to the entry list
			KaxCuePoint & NewPoint = AddNewChild<KaxCuePoint>(*this);
			NewPoint.PositionSet(BlockReference, GlobalTimecodeScale());
			myTempReferences.erase(ListIdx);
			break;
		}
	}
}

/*!
	\warning Assume that the list has been sorted (Sort())
*/
const KaxCuePoint * KaxCues::GetTimecodePoint(uint64 aTimecode) const
{
	uint64 TimecodeToLocate = aTimecode / GlobalTimecodeScale();
	const KaxCuePoint * aPointPrev = NULL;
	uint64 aPrevTime = 0;
	const KaxCuePoint * aPointNext = NULL;
	uint64 aNextTime = EBML_PRETTYLONGINT(0xFFFFFFFFFFFF);

	for (unsigned int i=0; i<ListSize(); i++)
	{
		if (EbmlId(*(*this)[i]) == KaxCuePoint::ClassInfos.GlobalId) {
			const KaxCuePoint *tmp = static_cast<const KaxCuePoint *>((*this)[i]);
			// check the tile
			const KaxCueTime *aTime = static_cast<const KaxCueTime *>(tmp->FindFirstElt(KaxCueTime::ClassInfos));
			if (aTime != NULL)
			{
				uint64 _Time = uint64(*aTime);
				if (_Time > aPrevTime && _Time < TimecodeToLocate) {
					aPrevTime = _Time;
					aPointPrev = tmp;
				}
				if (_Time < aNextTime && _Time > TimecodeToLocate) {
					aNextTime= _Time;
					aPointNext = tmp;
				}
			}
		}
	}

	return aPointPrev;
}

uint64 KaxCues::GetTimecodePosition(uint64 aTimecode) const
{
	const KaxCuePoint * aPoint = GetTimecodePoint(aTimecode);
	if (aPoint == NULL)
		return 0;

	const KaxCueTrackPositions * aTrack = aPoint->GetSeekPosition();
	if (aTrack == NULL)
		return 0;

	return aTrack->ClusterPosition();
}

END_LIBMATROSKA_NAMESPACE
