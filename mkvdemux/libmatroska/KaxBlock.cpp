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
	\version \$Id: KaxBlock.cpp,v 1.23 2003/06/11 21:04:22 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Julien Coloos    <suiryc @ users.sf.net>
*/
#include <cassert>

#include "StdInclude.h"
#include "KaxBlock.h"
#include "KaxContexts.h"
#include "KaxBlockData.h"
#include "KaxCluster.h"

START_LIBMATROSKA_NAMESPACE

const EbmlSemantic KaxBlockGroup_ContextList[7] =
{
	EbmlSemantic(true,  true,  KaxBlock::ClassInfos),
	EbmlSemantic(false, true,  KaxBlockVirtual::ClassInfos),
	EbmlSemantic(false, true,  KaxBlockDuration::ClassInfos),
	EbmlSemantic(false, true,  KaxSlices::ClassInfos),
	EbmlSemantic(true,  true,  KaxReferencePriority::ClassInfos),
	EbmlSemantic(false, false, KaxReferenceBlock::ClassInfos),
	EbmlSemantic(false, true,  KaxReferenceVirtual::ClassInfos),
};

EbmlId KaxBlockGroup_TheId   (0xA0, 1);
EbmlId KaxBlock_TheId        (0xA1, 1);
EbmlId KaxBlockVirtual_TheId (0xA2, 1);
EbmlId KaxBlockDuration_TheId(0x9B, 1);

const EbmlSemanticContext KaxBlockGroup_Context = EbmlSemanticContext(countof(KaxBlockGroup_ContextList), KaxBlockGroup_ContextList, &KaxCluster_Context, *GetKaxGlobal_Context, &KaxBlockGroup::ClassInfos);
const EbmlSemanticContext KaxBlock_Context = EbmlSemanticContext(0, NULL, &KaxBlockGroup_Context, *GetKaxGlobal_Context, &KaxBlock::ClassInfos);
const EbmlSemanticContext KaxBlockVirtual_Context = EbmlSemanticContext(0, NULL, &KaxBlockGroup_Context, *GetKaxGlobal_Context, &KaxBlockVirtual::ClassInfos);
const EbmlSemanticContext KaxBlockDuration_Context = EbmlSemanticContext(0, NULL, &KaxBlockGroup_Context, *GetKaxGlobal_Context, &KaxBlockDuration::ClassInfos);

const EbmlCallbacks KaxBlockGroup::ClassInfos(KaxBlockGroup::Create, KaxBlockGroup_TheId, "BlockGroup", KaxBlockGroup_Context);
const EbmlCallbacks KaxBlock::ClassInfos(KaxBlock::Create, KaxBlock_TheId, "Block", KaxBlock_Context);
const EbmlCallbacks KaxBlockVirtual::ClassInfos(KaxBlockVirtual::Create, KaxBlockVirtual_TheId, "BlockVirtual", KaxBlockVirtual_Context);
const EbmlCallbacks KaxBlockDuration::ClassInfos(KaxBlockDuration::Create, KaxBlockDuration_TheId, "BlockDuration", KaxBlockDuration_Context);

bool KaxBlock::ValidateSize() const
{
	return (Size >= 4); /// for the moment
}

KaxBlock::~KaxBlock()
{
	ReleaseFrames();
}

KaxBlockGroup::KaxBlockGroup()
 :EbmlMaster(KaxBlockGroup_Context)
 ,ParentCluster(NULL)
 ,ParentTrack(NULL)
{}

/*!
	\todo handle flags
	\todo hardcoded limit of the number of frames in a lace should be a parameter
	\return true if more frames can be added to this Block
*/
bool KaxBlock::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer)
{
	bValueIsSet = true;
	if (myBuffers.size() == 0) {
		// first frame
		Timecode = timecode;
		TrackNumber = track.TrackNumber();
	}
	myBuffers.push_back(&buffer);

	// we don't allow more than 8 frames in a Block because the overhead improvement is minimal
	if (myBuffers.size() >= 8)
		return false;

	// decide wether a new frame can be added or not
	// a frame in a lace is not efficient when the place necessary to code it in a lace is bigger 
	// than the size of a simple Block. That means more than 6 bytes (4 in struct + 2 for EBML) to code the size
	return (buffer.Size() < 6*0xFF);
}

/*!
	\todo handle gap flag
*/
uint64 KaxBlock::UpdateSize(bool bSaveDefault)
{
	assert(Data == NULL); // Data is not used for KaxBlock
	assert(TrackNumber < 0x4000); // no more allowed for the moment
	unsigned int i;

	// compute the final size of the data
	switch (myBuffers.size()) {
		case 0:
			Size = 0;
			break;
		case 1:
			Size = 4 + myBuffers[0]->Size();
			break;
		default:
			Size = 4 + 1; // 1 for the lacing head
			for (i=0; i<myBuffers.size()-1; i++) {
				Size += myBuffers[i]->Size() + (myBuffers[i]->Size() / 0xFF + 1);
			}
			// Size of the last frame (not in lace)
			Size += myBuffers[i]->Size();
			break;
	}

	if (TrackNumber >= 0x80)
		Size++; // the size will be coded with one more octet

	return Size;
}

/*!
	\todo handle the gap flag
*/
uint64 KaxBlockVirtual::UpdateSize(bool bSaveDefault)
{
	assert(TrackNumber < 0x4000);
	binary *cursor = Data;
	// fill data
	if (TrackNumber < 0x80) {
		*cursor++ = TrackNumber | 0x80; // set the first bit to 1 
	} else {
		*cursor++ = (TrackNumber >> 8) | 0x40; // set the second bit to 1
		*cursor++ = TrackNumber & 0xFF;
	}

	assert(ParentCluster != NULL);
	int16 ActualTimecode = ParentCluster->GetBlockLocalTimecode(Timecode);
	big_int16 b16(ActualTimecode);
	b16.Fill(cursor);
	cursor += 2;

	*cursor++ = (0 << 0); // no gap

	return Size;
}

/*!
	\todo more optimisation is possible (render the Block head and don't copy the buffer in memory, care should be taken with the allocation of Data)
	\todo the actual timecode to write should be retrieved from the Cluster from here
*/
uint32 KaxBlock::RenderData(IOCallback & output, bool bSaveDefault)
{
	if (myBuffers.size() == 0) {
		return 0;
	} else {
		assert(TrackNumber < 0x4000);
		binary BlockHead[5], *cursor = BlockHead;
		unsigned int i;
		bool bLacing = false;

		if (myBuffers.size() == 1) {
			Size = 4;
		} else {
			bLacing = true;
			Size = 4 + 1; // 1 for the lacing head
		}
		if (TrackNumber > 0x80)
			Size++;

		// write Block Head
		if (TrackNumber < 0x80) {
			*cursor++ = TrackNumber | 0x80; // set the first bit to 1 
		} else {
			*cursor++ = (TrackNumber >> 8) | 0x40; // set the second bit to 1
			*cursor++ = TrackNumber & 0xFF;
		}

		assert(ParentCluster != NULL);
		int16 ActualTimecode = ParentCluster->GetBlockLocalTimecode(Timecode);
		big_int16 b16(ActualTimecode);
		b16.Fill(cursor);
		cursor += 2;

		*cursor = (0 << 0); // no gap
		*cursor++ |= (bLacing << 1); // bit 6

		output.writeFully(BlockHead, 4 + ((TrackNumber > 0x80) ? 1 : 0));

		if (bLacing) {
			binary tmpValue;

			// number of laces
			tmpValue = myBuffers.size()-1;
			output.writeFully(&tmpValue, 1);

			// set the size of each member in the lace
			for (i=0; i<myBuffers.size()-1; i++) {
				tmpValue = 0xFF;
				uint16 tmpSize = myBuffers[i]->Size();
				while (tmpSize >= 0xFF) {
					output.writeFully(&tmpValue, 1);
					Size++;
					tmpSize -= 0xFF;
				}
				tmpValue = tmpSize;
				output.writeFully(&tmpValue, 1);
				Size++;
			}
		}

		// put the data of each frame
		for (i=0; i<myBuffers.size(); i++) {
			output.writeFully(myBuffers[i]->Buffer(), myBuffers[i]->Size());
			Size += myBuffers[i]->Size();
		}

	}

	return Size;
}

uint64 KaxBlock::ReadInternalHead(IOCallback & input)
{
	binary Buffer[5], *cursor = Buffer;
	uint64 Result = input.read(cursor, 4);
	if (Result != 4)
		return Result;
	
	// update internal values
	TrackNumber = *cursor++;
	if ((TrackNumber & 0x80) == 0) {
		// there is extra data
		if ((TrackNumber & 0x40) == 0) {
			// We don't support track numbers that large !
			return Result;
		}
		Result += input.read(&Buffer[4], 1);
		TrackNumber = (TrackNumber & 0x3F) << 8;
		TrackNumber += *cursor++;
	} else {
		TrackNumber &= 0x7F;
	}

    
	big_int16 b16;
	b16.Eval(cursor);
	assert(ParentCluster != NULL);
	Timecode = ParentCluster->GetBlockGlobalTimecode(int16(b16));
	bLocalTimecodeUsed = false;
	cursor += 2;

	bGap = (*cursor && 0x01);
	return Result;
}

/*!
	\todo better zero copy handling
*/
uint64 KaxBlock::ReadData(IOCallback & input)
{
	uint64 Result = EbmlBinary::ReadData(input);
	binary *cursor = Data;
	uint8 BlockHeadSize = 4;

	// update internal values
	TrackNumber = *cursor++;
	if ((TrackNumber & 0x80) == 0) {
		// there is extra data
		if ((TrackNumber & 0x40) == 0) {
			// We don't support track numbers that large !
			return Result;
		}
		TrackNumber = (TrackNumber & 0x3F) << 8;
		TrackNumber += *cursor++;
		BlockHeadSize++;
	} else {
		TrackNumber &= 0x7F;
	}
    
	big_int16 b16;
	b16.Eval(cursor);
	LocalTimecode = int16(b16);
	bLocalTimecodeUsed = true;
	cursor += 2;

	bGap = (*cursor && 0x01);
	bool bLacing = (*cursor++ && 0x02);

	// put all Frames in the list
	if (!bLacing) {
		DataBuffer * soloFrame = new DataBuffer(cursor, Size - BlockHeadSize);
		myBuffers.push_back(soloFrame);
	} else {
		// read the number of frames in the lace
		uint32 LastBufferSize = Size - BlockHeadSize - 1; // 1 for frame number
		uint8 FrameNum = *cursor++; // number of frames in the lace - 1

		uint8 Index;
		uint16 FrameSize;
		std::vector<uint16> SizeList;
		// read the list of frame sizes
		for (Index=0; Index<FrameNum; Index++) {
			// get the size of the frame
			FrameSize = 0;
			do {
				FrameSize += uint8(*cursor);
				LastBufferSize--;
			} while (*cursor++ == 0xFF);
			SizeList.push_back(FrameSize);
			LastBufferSize -= FrameSize;
		}
		SizeList.push_back(LastBufferSize);

		for (Index=0; Index<FrameNum+1; Index++) {
			DataBuffer * lacedFrame = new DataBuffer(cursor, SizeList[Index]);
			myBuffers.push_back(lacedFrame);
			cursor += SizeList[Index];
		}
	}

	return Result;
}

bool KaxBlockGroup::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer)
{
	KaxBlock & theBlock = GetChild<KaxBlock>(*this);
	assert(ParentCluster != NULL);
	theBlock.SetParent(*ParentCluster);
	ParentTrack = &track;
	return theBlock.AddFrame(track, timecode, buffer);
}

bool KaxBlockGroup::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, const KaxBlockGroup & PastBlock)
{
//	assert(past_timecode < 0);

	KaxBlock & theBlock = GetChild<KaxBlock>(*this);
	assert(ParentCluster != NULL);
	theBlock.SetParent(*ParentCluster);
	ParentTrack = &track;
	bool bRes = theBlock.AddFrame(track, timecode, buffer);

	KaxReferenceBlock & thePastRef = GetChild<KaxReferenceBlock>(*this);
	thePastRef.SetReferencedBlock(PastBlock);
	thePastRef.SetParentBlock(*this);

	return bRes;
}

bool KaxBlockGroup::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, const KaxBlockGroup & PastBlock, const KaxBlockGroup & ForwBlock)
{
//	assert(past_timecode < 0);

//	assert(forw_timecode > 0);
	
	KaxBlock & theBlock = GetChild<KaxBlock>(*this);
	assert(ParentCluster != NULL);
	theBlock.SetParent(*ParentCluster);
	ParentTrack = &track;
	bool bRes = theBlock.AddFrame(track, timecode, buffer);

	KaxReferenceBlock & thePastRef = GetChild<KaxReferenceBlock>(*this);
	thePastRef.SetReferencedBlock(PastBlock);
	thePastRef.SetParentBlock(*this);

	KaxReferenceBlock & theFutureRef = AddNewChild<KaxReferenceBlock>(*this);
	theFutureRef.SetReferencedBlock(ForwBlock);
	theFutureRef.SetParentBlock(*this);

	return bRes;
}

/*!
	\todo we may cache the reference to the timecode block
*/
uint64 KaxBlockGroup::GlobalTimecode() const
{
	assert(ParentCluster != NULL); // impossible otherwise
	KaxBlock & MyBlock = *static_cast<KaxBlock *>(this->FindElt(KaxBlock::ClassInfos));
	return MyBlock.GlobalTimecode();

}

uint16 KaxBlockGroup::TrackNumber() const
{
	KaxBlock & MyBlock = *static_cast<KaxBlock *>(this->FindElt(KaxBlock::ClassInfos));
	return MyBlock.TrackNum();
}

uint64 KaxBlockGroup::ClusterPosition() const
{
	assert(ParentCluster != NULL); // impossible otherwise
	return ParentCluster->GetPosition();
}

unsigned int KaxBlockGroup::ReferenceCount() const
{
	unsigned int Result = 0;
	KaxReferenceBlock * MyBlockAdds = static_cast<KaxReferenceBlock *>(FindFirstElt(KaxReferenceBlock::ClassInfos));
	if (MyBlockAdds != NULL) {
		Result++;
		while ((MyBlockAdds = static_cast<KaxReferenceBlock *>(FindNextElt(*MyBlockAdds))) != NULL)
		{
			Result++;
		}
	}
	return Result;
}

const KaxReferenceBlock & KaxBlockGroup::Reference(unsigned int Index) const
{
	KaxReferenceBlock * MyBlockAdds = static_cast<KaxReferenceBlock *>(FindFirstElt(KaxReferenceBlock::ClassInfos));
	assert(MyBlockAdds != NULL); // call of a non existing reference
	
	while (Index != 0) {
		MyBlockAdds = static_cast<KaxReferenceBlock *>(FindNextElt(*MyBlockAdds));
		assert(MyBlockAdds != NULL);
		Index--;
	}
	return *MyBlockAdds;
}

void KaxBlockGroup::ReleaseFrames()
{
	KaxBlock & MyBlock = *static_cast<KaxBlock *>(this->FindElt(KaxBlock::ClassInfos));
	MyBlock.ReleaseFrames();
}

void KaxBlock::ReleaseFrames()
{
	// free the allocated Frames
	int i;
	for (i=myBuffers.size()-1; i>=0; i--) {
		if (myBuffers[i] != NULL) {
			myBuffers[i]->FreeBuffer(*myBuffers[i]);
			delete myBuffers[i];
			myBuffers[i] = NULL;
		}
	}
}

void KaxBlockGroup::SetBlockDuration(uint64 TimeLength)
{
	assert(ParentTrack != NULL);
	int64 scale = ParentTrack->GlobalTimecodeScale();
	KaxBlockDuration & myDuration = *static_cast<KaxBlockDuration *>(FindFirstElt(KaxBlockDuration::ClassInfos, true));
	*(static_cast<EbmlUInteger *>(&myDuration)) = TimeLength / uint64(scale);
}

bool KaxBlockGroup::GetBlockDuration(uint64 &TheTimecode) const
{
	KaxBlockDuration * myDuration = static_cast<KaxBlockDuration *>(FindElt(KaxBlockDuration::ClassInfos));
	if (myDuration == NULL) {
		return false;
	}

	assert(ParentTrack != NULL);
	TheTimecode = uint64(*myDuration) * ParentTrack->GlobalTimecodeScale();
	return true;
}

void KaxBlock::SetParent(KaxCluster & aParentCluster)
{
	ParentCluster = &aParentCluster;
	if (bLocalTimecodeUsed) {
		Timecode = aParentCluster.GetBlockGlobalTimecode(LocalTimecode);
		bLocalTimecodeUsed = false;
	}
}

END_LIBMATROSKA_NAMESPACE
