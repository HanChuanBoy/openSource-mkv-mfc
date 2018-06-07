
#ifndef __KAXOUTPIN_H__
#define __KAXOUTPIN_H__

#include "global.h"

using namespace LIBMATROSKA_NAMESPACE;

#define QUEING

class CKaxOutPin : public CBaseOutputPin
{
private:
	COutputQueue		*m_pOutQ;
	KaxTrackInfoStruct	*m_pTrackInfos;
	BOOL				m_bDiscontinuity;

	// timing issues
	REFERENCE_TIME	m_tStrTime;
	REFERENCE_TIME	m_tMediaTime;
	uint32			m_tTrackTime;
	uint32			m_tBlockDuration;
// ###
	KaxBlock		*m_pTmpBlock;
	bool			m_bStrReset;
	uint64			m_tStrStart, m_tClusterTime;
// ###
	bool			m_bRunning;
	
	bool            m_bSendHeader;
	void SendOneHeaderPerSample(binary* CodecPrivateData, int DataLen);
public:
//	DECLARE_IUNKNOWN;

	CKaxOutPin(CBaseFilter *pFilter, CCritSec *pLock, HRESULT *phr,
		LPCWSTR pName, KaxTrackInfoStruct *pInfos);
	~CKaxOutPin();

	STDMETHODIMP Connect(IPin *pPin, const AM_MEDIA_TYPE *pmt);

	HRESULT CheckMediaType(const CMediaType *pmt);
	HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pAllocProps);
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
#ifdef QUEING
	HRESULT Deliver(IMediaSample *pSample) { if (m_bDiscontinuity) pSample->SetDiscontinuity(true); return m_pOutQ->Receive(pSample); }

	// queueing stuff
	HRESULT Active();
	HRESULT Inactive();
	HRESULT DeliverBeginFlush() { m_pOutQ->BeginFlush(); return S_OK; }
	HRESULT DeliverEndFlush() { m_bDiscontinuity = TRUE; m_pOutQ->EndFlush(); return S_OK; }
	HRESULT DeliverEndOfStream() { m_pOutQ->EOS(); return S_OK; }
	HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tEnd, double dRate) { m_pOutQ->NewSegment(tStart, tEnd, dRate); m_pOutQ->SendAnyway(); return S_OK; }
	void SendAnyway() { if (m_pOutQ != NULL) m_pOutQ->SendAnyway(); }
#else
	void SendAnyway() { return; }
	HRESULT Deliver(IMediaSample *pSample);
#endif

	HRESULT CompleteConnect(IPin *pReceivePin);

	void GetStrTime(REFERENCE_TIME *t) { *t = m_tStrTime; }
	void SetStrTime(REFERENCE_TIME *t) { m_tStrTime = *t; }
	void GetMediaTime(REFERENCE_TIME *t) { *t = m_tMediaTime; }
	void SetMediaTime(REFERENCE_TIME *t) { m_tMediaTime = *t; }
	uint32 GetTrackTime() { return m_tTrackTime; }
	void SetTrackTime(uint32 t) { m_tTrackTime = t; }
	void SetRunning(bool running) { m_bRunning = running; }
	void SetBlockDuration(uint32 tBlockDuration) { m_tBlockDuration = tBlockDuration; }
	uint32 GetBlockDuration() { return m_tBlockDuration; }
	bool IsRunning() { return m_bRunning; }

	// resets the stream time to start from "zero"
	void ResetStreamTime() { m_bStrReset = true; }
	void SetClusterTime(uint32 tClusterTime) { m_tClusterTime = tClusterTime; }
	BOOL DeliverBlock(KaxBlock *pBlock);
	void FlushBlocks();
};

#endif __KAXOUTPIN_H__
