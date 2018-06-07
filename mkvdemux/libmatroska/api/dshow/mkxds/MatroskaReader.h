/****************************************************************************
** libmatroska : parse Matroska files, see http://www.matroska.org/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Christophe Paris.  All rights reserved.
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
	\version \$Id: MatroskaReader.h,v 1.16 2003/06/09 00:08:08 robux4 Exp $
	\author Christophe Paris     <toffparis @ users.sf.net>
*/

#if !defined(AFX_MATROSKAREADER_H__F60FAEB3_2387_430F_88AF_7877B5F1328A__INCLUDED_)
#define AFX_MATROSKAREADER_H__F60FAEB3_2387_430F_88AF_7877B5F1328A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <vector>

#include "global.h"

using namespace LIBMATROSKA_NAMESPACE;

class SegmentInfo {
public:
	SegmentInfo(KaxSegment & aSegment)
		:mSegment(&aSegment)
		,bMinIsSet(false)
		,bMaxIsSet(false)
	{}

	KaxSegment *Segment() const {return mSegment;}

	void ContainsTimecode(uint64 aTimecode);
	bool IsTimecodeInside(uint64 aTimecode) const;

protected:
	KaxSegment *mSegment;
	bool bMinIsSet;
	uint64 mMinTimecode;
	bool bMaxIsSet;
	uint64 mMaxTimecode;
};

class SegmentList {
public:
	virtual ~SegmentList();
	SegmentInfo * FindTimecode(uint64 aTimecode) const;
	SegmentInfo * AddSegment(KaxSegment & aSegment);
protected:
	std::vector<SegmentInfo *> mList;
};

class MatroskaReader  
{
public:
	MatroskaReader();
	virtual ~MatroskaReader();
	int InitKaxFile(IOCallback *pIOCb);	
	int GetTrackNumber() const;
	KaxTrackInfoStruct& GetTrackInfo(int i);
	KaxBlock *GetNextBlock();
	KaxBlockGroup *GetNextBlockG();
	void  Reset();

	uint64 GetDuration() { return m_qwDuration;	}

	void JumpToTimecode(uint64 aTimecode, bool bSeekToTheCorrectBlock);

protected:
	KaxCues * GetCueEntry(EbmlStream & inDataStream, const KaxSeekHead & aMetaSeek, const KaxSegment & FirstSegment);
	int64 FindClusterLocated(int64 aLocation, const KaxSeekHead & aMetaSeek);

private:
	int InitTrack(EbmlElement **elems, int &UEL, KaxTrackInfoStruct *pTrackInfos);
	int IndexForTrack(uint16 TrackNum);

	IOCallback	*m_pKaxIO;
	EbmlStream	*m_pStream;
	EbmlElement	*m_pElems[6];
	EbmlElement	*m_pTmpElems[2];
	uint64      m_TimeCodeScale;
//	uint64		m_qwKaxSegStart;
	int			m_iLevel, m_iUpElLev, m_iTmpUpElLev;
	const bool  bReadDummyElements;
	uint64      m_qwDuration;
	
	std::vector<KaxTrackInfoStruct*> mTracks;
	std::vector<KaxTrackEntry *>     mTracksEntries;

	KaxCues     * m_CueEntries;
	KaxSeekHead * m_MetaSeeks;

	bool   bLookForBlock;
	uint64 BlockTimecode;
	uint16 BlockTrack;

	SegmentList  m_Segments;
	SegmentInfo *mCurSegment;
};

#endif // !defined(AFX_MATROSKAREADER_H__F60FAEB3_2387_430F_88AF_7877B5F1328A__INCLUDED_)
