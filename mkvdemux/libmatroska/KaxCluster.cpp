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
	\version \$Id: KaxCluster.cpp,v 1.18 2003/06/07 11:36:14 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxCluster.h"
#include "KaxClusterData.h"
#include "KaxBlock.h"
#include "KaxContexts.h"
#include "KaxSegment.h"

// sub elements
START_LIBMATROSKA_NAMESPACE

EbmlSemantic KaxCluster_ContextList[] =
{
	EbmlSemantic(true,  true,  KaxClusterTimecode::ClassInfos),
	EbmlSemantic(false, true,  KaxClusterPosition::ClassInfos),
	EbmlSemantic(false, true,  KaxClusterPrevSize::ClassInfos),
	EbmlSemantic(true,  false, KaxBlockGroup::ClassInfos),
};

const EbmlSemanticContext KaxCluster_Context = EbmlSemanticContext(countof(KaxCluster_ContextList), KaxCluster_ContextList, &KaxSegment_Context, *GetKaxGlobal_Context, &KaxCluster::ClassInfos);

EbmlId KaxCluster_TheId(0x1F43B675, 4);
const EbmlCallbacks KaxCluster::ClassInfos(KaxCluster::Create, KaxCluster_TheId, "Cluster", KaxCluster_Context);

KaxCluster::KaxCluster()
	:EbmlMaster(KaxCluster_Context)
	,currentNewBlock(NULL)
	,ParentSegment(NULL)
	,bFirstFrameInside(false)
	,bPreviousTimecodeIsSet(false)
	,bTimecodeScaleIsSet(false)
{}

bool KaxCluster::AddFrameInternal(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, KaxBlockGroup * & MyNewBlock, const KaxBlockGroup * PastBlock, const KaxBlockGroup * ForwBlock)
{
	if (!bFirstFrameInside) {
		bFirstFrameInside = true;
		MinTimecode = MaxTimecode = timecode;
	} else {
		if (timecode < MinTimecode)
			MinTimecode = timecode;
		if (timecode > MaxTimecode)
			MaxTimecode = timecode;
	}

	MyNewBlock = NULL;

	if (!track.LacingEnabled()) {
		currentNewBlock = NULL;
	}

	// force creation of a new block
	if (currentNewBlock == NULL || uint32(track.TrackNumber()) != uint32(currentNewBlock->TrackNumber()) || PastBlock != NULL || ForwBlock != NULL) {
		KaxBlockGroup & aNewBlock = GetNewBlock();
		MyNewBlock = currentNewBlock = &aNewBlock;
		currentNewBlock = &aNewBlock;
	}

	if (PastBlock != NULL) {
		if (ForwBlock != NULL) {
			if (currentNewBlock->AddFrame(track, timecode, buffer, *PastBlock, *ForwBlock)) {
				// more data are allowed in this Block
				return true;
			} else {
				currentNewBlock = NULL;
				return false;
			}
		} else {
			if (currentNewBlock->AddFrame(track, timecode, buffer, *PastBlock)) {
				// more data are allowed in this Block
				return true;
			} else {
				currentNewBlock = NULL;
				return false;
			}
		}
	} else {
		if (currentNewBlock->AddFrame(track, timecode, buffer)) {
			// more data are allowed in this Block
			return true;
		} else {
			currentNewBlock = NULL;
			return false;
		}
	}
}

bool KaxCluster::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, KaxBlockGroup * & MyNewBlock)
{
	return AddFrameInternal(track, timecode, buffer, MyNewBlock, NULL, NULL);
}

bool KaxCluster::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, KaxBlockGroup * & MyNewBlock, const KaxBlockGroup & PastBlock)
{
	return AddFrameInternal(track, timecode, buffer, MyNewBlock, &PastBlock, NULL);
}

bool KaxCluster::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, KaxBlockGroup * & MyNewBlock, const KaxBlockGroup & PastBlock, const KaxBlockGroup & ForwBlock)
{
	return AddFrameInternal(track, timecode, buffer, MyNewBlock, &PastBlock, &ForwBlock);
}

/*!
	\todo only put the Blocks written in the cue entries
*/
uint32 KaxCluster::Render(IOCallback & output, KaxCues & CueToUpdate, bool bSaveDefault)
{
	// update the Timecode of the Cluster before writing
	KaxClusterTimecode * Timecode = static_cast<KaxClusterTimecode *>(this->FindElt(KaxClusterTimecode::ClassInfos));
	*static_cast<EbmlUInteger *>(Timecode) = GlobalTimecode() / GlobalTimecodeScale();

	uint32 Result = EbmlMaster::Render(output, bSaveDefault);
	// For all Blocks add their position on the CueEntry
	size_t Index;
	
	for (Index = 0; Index < ElementList.size(); Index++) {
		if (EbmlId(*ElementList[Index]) == KaxBlockGroup::ClassInfos.GlobalId) {
			CueToUpdate.PositionSet(*static_cast<const KaxBlockGroup *>(ElementList[Index]));
		}
	}
	return Result;
}

/*!
	\todo automatically choose valid timecode for the Cluster based on the previous cluster timecode (must be incremental)
*/
uint64 KaxCluster::GlobalTimecode() const
{
	assert(bPreviousTimecodeIsSet);
	uint64 result = MinTimecode;

	if (result < PreviousTimecode)
		result = PreviousTimecode + 1;
	
	return result;
}

/*!
	\brief retrieve the relative 
	\todo !!! We need a way to know the TimecodeScale
*/
int16 KaxCluster::GetBlockLocalTimecode(uint64 aGlobalTimecode) const
{
	int64 TimecodeDelay = aGlobalTimecode - GlobalTimecode();
	return int16(TimecodeDelay / GlobalTimecodeScale());
}

uint64 KaxCluster::GetBlockGlobalTimecode(int16 GlobalSavedTimecode)
{
	if (!bFirstFrameInside) {
		KaxClusterTimecode * Timecode = static_cast<KaxClusterTimecode *>(this->FindElt(KaxClusterTimecode::ClassInfos));
		assert (bFirstFrameInside); // use the InitTimecode() hack for now
		MinTimecode = MaxTimecode = PreviousTimecode = *static_cast<EbmlUInteger *>(Timecode);
		bFirstFrameInside = true;
		bPreviousTimecodeIsSet = true;
	}
	return int64(GlobalSavedTimecode * GlobalTimecodeScale()) + GlobalTimecode();
}

KaxBlockGroup & KaxCluster::GetNewBlock()
{
	KaxBlockGroup & MyBlock = AddNewChild<KaxBlockGroup>(*this);
	MyBlock.SetParent(*this);
	return MyBlock;
}

void KaxCluster::ReleaseFrames()
{
	size_t Index;
	
	for (Index = 0; Index < ElementList.size(); Index++) {
		if (EbmlId(*ElementList[Index]) == KaxBlockGroup::ClassInfos.GlobalId) {
			static_cast<KaxBlockGroup*>(ElementList[Index])->ReleaseFrames();
		}
	}
}

uint64 KaxCluster::GetPosition() const
{
	assert(ParentSegment != NULL);
	return ParentSegment->GetRelativePosition(*this);
}

END_LIBMATROSKA_NAMESPACE
