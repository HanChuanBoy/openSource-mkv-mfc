// MatroskaReader.h: interface for the MatroskaReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATROSKAREADER_H__F60FAEB3_2387_430F_88AF_7877B5F1328A__INCLUDED_)
#define AFX_MATROSKAREADER_H__F60FAEB3_2387_430F_88AF_7877B5F1328A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "global.h"

using namespace LIBMATROSKA_NAMESPACE;

class MatroskaReader  
{
public:
	MatroskaReader();
	virtual ~MatroskaReader();
	int InitKaxFile(IOCallback *pIOCb);	
	int InitTrack(EbmlElement **elems, int &UEL, KaxTrackInfoStruct *pTrackInfos);
	int GetTrackNumber();
	KaxTrackInfoStruct* GetTrackInfo(int i);
	KaxBlock *GetNextBlock();
	void  Reset();

	uint64 GetDuration() { return m_qwDuration;	}

private:
	IOCallback	*m_pKaxIO;
	EbmlStream	*m_pStream;
	EbmlElement	*m_pElems[6];
	EbmlElement	*m_pTmpElems[2];
	double		m_TimeScale;
	uint64		m_qwKaxSegStart;
	int			m_iLevel, m_iUpElLev, m_iTmpUpElLev;
	const bool  bReadDummyElements;
	uint64      m_qwDuration;
	
	// TODO : remove limitation n°
	KaxTrackInfoStruct* m_Tracks[10];
	int m_TrackCount;
};

#endif // !defined(AFX_MATROSKAREADER_H__F60FAEB3_2387_430F_88AF_7877B5F1328A__INCLUDED_)
