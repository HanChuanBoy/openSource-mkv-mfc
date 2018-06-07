#include <streams.h>
#include <initguid.h>
#include <mmreg.h>

#include "kaxoutpin.h"
#include "codecs.h"
#include "OggDS.h"

CKaxOutPin::CKaxOutPin(CBaseFilter *pFilter, CCritSec *pLock, HRESULT *phr,
					   LPCWSTR pName, KaxTrackInfoStruct *pInfos)
: CBaseOutputPin("Output", pFilter, pLock, phr, pName)
#ifdef QUEING
, m_pOutQ(NULL)
#endif
, m_tMediaTime(0)
, m_tStrTime(0)
, m_tTrackTime(0)
, m_bRunning(false)
, m_tStrStart(0)
, m_bStrReset(false)
, m_pTmpBlock(NULL)
, m_tClusterTime(0)
, m_bSendHeader(false)
{	
	int i;
	m_pTrackInfos = pInfos;

	// create media type
	switch(pInfos->Type)
	{
	case track_video:
		{		
			BITMAPINFOHEADER	*pbmi = (BITMAPINFOHEADER*)pInfos->CodecPrivate;
			VIDEOINFOHEADER		*pvih = new VIDEOINFOHEADER;
			CMediaType			*pMediaType;

			pMediaType = &m_mt;
			pMediaType->InitMediaType();
			pMediaType->SetType(&MEDIATYPE_Video);
			pMediaType->SetFormatType(&FORMAT_VideoInfo);
			memset(pvih, 0, sizeof(VIDEOINFOHEADER));
			memcpy(&pvih->bmiHeader, pbmi, sizeof(BITMAPINFOHEADER));
			pvih->AvgTimePerFrame = 400000;
			pMediaType->SetFormat((BYTE*)pvih, sizeof(VIDEOINFOHEADER));
			
			FOURCCMap	fcc(pbmi->biCompression);
			pMediaType->SetSubtype((GUID*)&fcc);
			pMediaType->SetSampleSize(1);
		}
		break;

	case track_audio:
	{
		GUID			*pGuid = NULL;
		WAVEFORMATEX	*pwfx;
		
		if(strcmp((const char*)pInfos->CodecID, "A_VORBIS") == 0)
		{
			VORBISFORMAT vf;
			vf.nChannels = pInfos->pAudioInfo->Channels;
			vf.nSamplesPerSec = pInfos->pAudioInfo->SamplingFreq;
			vf.nMaxBitsPerSec = -1;    // CHECKME
			vf.nMinBitsPerSec = -1;    // CHECKME
			vf.nAvgBitsPerSec = -1;    // CHECKME
			vf.fQuality = 0;           // CHECKME
			
			m_mt.InitMediaType();
			m_mt.SetType(&MEDIATYPE_Audio);
			m_mt.SetSubtype(&MEDIASUBTYPE_Vorbis);
			m_mt.SetFormatType(&FORMAT_VorbisFormat);
			m_mt.SetFormat((BYTE*)&vf,sizeof(VORBISFORMAT));
			m_mt.SetSampleSize(1);

			m_bSendHeader = true;			
		}
		else if(strcmp((const char*)pInfos->CodecID, "A_MS/ACM") == 0)
		{
			WAVEFORMATEX* pwfx = (WAVEFORMATEX*)pInfos->CodecPrivate;
			CreateAudioMediaType(pwfx, &m_mt, TRUE);
		}		
		else
		{
			WAVEFORMATEX	wfx;

			memset(&wfx, 0, sizeof(WAVEFORMATEX));			
			
			if(pInfos->pAudioInfo->Channels == 0)
				wfx.nChannels = 1;
			else
				wfx.nChannels = pInfos->pAudioInfo->Channels;
			
			if(pInfos->pAudioInfo->SamplingFreq == 0)
				wfx.nSamplesPerSec = 8000;
			else
				wfx.nSamplesPerSec = pInfos->pAudioInfo->SamplingFreq;

			
			if (pInfos->pAudioInfo->BitDepth == 0)
				wfx.wBitsPerSample = 16;
			else
				wfx.wBitsPerSample = pInfos->pAudioInfo->BitDepth;

			wfx.nBlockAlign = (unsigned short)((wfx.nChannels * wfx.wBitsPerSample) / 8);
			wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

			for (i=0; i<ACM_CODEC_LIST_LEN; i++)
			{
				if (strcmp((const char*)pInfos->CodecID, ACM_CODEC_LIST[i].name) == 0)
				{
					wfx.wFormatTag = ACM_CODEC_LIST[i].id;
					if (ACM_CODEC_LIST[i].id == 0)
					{
						pGuid = (GUID*)&ACM_CODEC_LIST[i].guid;
					}
					break;
				}
			}

			pwfx = &wfx;
			CreateAudioMediaType(pwfx, &m_mt, TRUE);

			if (pGuid != NULL)
			{
				m_mt.SetSubtype(pGuid);					
			}
		}
	}
		break;

	case track_subtitle:
		m_mt.InitMediaType();
		m_mt.SetType(&MEDIATYPE_Text);
		break;
	}
}

CKaxOutPin::~CKaxOutPin() 
{	
	if(m_pTmpBlock)
	{
		delete m_pTmpBlock;
	}

	if(m_pOutQ)
	{
		delete m_pOutQ;
		m_pOutQ = NULL;
	}
}

HRESULT CKaxOutPin::Connect(IPin *pPin, const AM_MEDIA_TYPE *pmt) {
	if (pmt == NULL)
		return CBaseOutputPin::Connect(pPin, &m_mt);
	else
		return CBaseOutputPin::Connect(pPin, pmt);
}

HRESULT CKaxOutPin::CheckMediaType(const CMediaType *pmt) {
	if (m_mt == *pmt)
		return S_OK;
	return S_FALSE;
}

HRESULT CKaxOutPin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pAllocProps) {
	ALLOCATOR_PROPERTIES	Actual;
	HRESULT	hr;
	
	if (pAlloc == NULL) return E_POINTER;
	if (pAllocProps == NULL) return E_POINTER;


	pAllocProps->cBuffers = 32;

	// compute buffer size
	switch(m_pTrackInfos->Type)
	{
	case track_video:
		if (pAllocProps->cbBuffer < m_pTrackInfos->pVideoInfo->PixelHeight *
			m_pTrackInfos->pVideoInfo->PixelWidth * 4)
		{
			pAllocProps->cbBuffer = m_pTrackInfos->pVideoInfo->PixelHeight *
									m_pTrackInfos->pVideoInfo->PixelWidth * 4;
	}
		break;
	case track_audio:
	case track_subtitle:
		pAllocProps->cbBuffer = 0xFFFF;
		break;
	}

	pAllocProps->cbAlign = 4;
	
	// set properties
	hr = pAlloc->SetProperties(pAllocProps, &Actual);
	if (FAILED(hr))
		return hr;

	return NO_ERROR;
}

HRESULT CKaxOutPin::GetMediaType(int iPosition, CMediaType *pMediaType) {
	if (pMediaType == NULL) return E_INVALIDARG;
	if (iPosition != 0) return VFW_S_NO_MORE_ITEMS;

	pMediaType->InitMediaType();
	pMediaType->SetType(m_mt.Type());
	pMediaType->SetSubtype(m_mt.Subtype());
	pMediaType->SetFormatType(m_mt.FormatType());
	pMediaType->SetTemporalCompression(m_mt.IsTemporalCompressed());
	if(m_mt.FormatLength() > 0)
	{
	pMediaType->SetFormat(m_mt.Format(), m_mt.FormatLength());
	}

	return S_OK;
}

HRESULT CKaxOutPin::CompleteConnect(IPin *pReceivePin)
{
	HRESULT hr = CBaseOutputPin::CompleteConnect(pReceivePin);
	if (FAILED(hr))
	{
		return hr;
	}

#ifdef QUEING
	/**
	switch(m_pTrackInfos->Type)
	{
	case track_video:
		m_pOutQ = new COutputQueue(pReceivePin, &hr, FALSE, TRUE, 1, FALSE, 10);
		break;
	case track_audio:
		m_pOutQ = new COutputQueue(pReceivePin, &hr, FALSE, FALSE, 1, FALSE, 10);
		break;
	}
	/**/
	m_pOutQ = new COutputQueue(pReceivePin, &hr, FALSE, TRUE, 1, FALSE, 8);
	/**
	m_pOutQ = new COutputQueue(pReceivePin, &hr, FALSE, TRUE, 1, FALSE, 10);
	/**/
		
	if (m_pOutQ == NULL) return E_OUTOFMEMORY;
	if (FAILED(hr)) {
		delete m_pOutQ;
		m_pOutQ = NULL;
		return hr;
	}
#endif

	return S_OK;
}

#ifdef QUEING
HRESULT CKaxOutPin::Active() {
	m_bDiscontinuity = TRUE;	
	if (!IsConnected()) return S_OK;	
	HRESULT hr = CBaseOutputPin::Active();
	if (FAILED(hr)) return hr;
	return hr;
}

HRESULT CKaxOutPin::Inactive() {
	HRESULT hr = CBaseOutputPin::Inactive();
	if (FAILED(hr)) return hr;
	return S_OK;
}
#else
HRESULT CKaxOutPin::Deliver(IMediaSample *pSample) {
	if (m_bDiscontinuity) {
		m_bDiscontinuity = FALSE;
		pSample->SetDiscontinuity(TRUE);
	}
	return CBaseOutputPin::Deliver(pSample);
}
#endif


// Thanks to Cyrius
void CKaxOutPin::SendOneHeaderPerSample(binary* CodecPrivateData, int DataLen)
{
	IMediaSample	*pSample;
	BYTE			*pData;
	REFERENCE_TIME	tTime = 0;
	binary *codecPrivate = CodecPrivateData;
	uint32 lastHeaderSize = DataLen - 1;
	uint16 nbHeaders = (*(uint8 *)codecPrivate);
	CGenericList<uint32> headerSizeList("Headers"); 	
	codecPrivate++;
	
	while(nbHeaders--)
	{
		uint32 currentHeaderSize = 0;
		do{
			currentHeaderSize += *(uint8 *)codecPrivate;
			lastHeaderSize--;
		} while((*codecPrivate++) == 0xFF);
		lastHeaderSize -= currentHeaderSize;
		headerSizeList.AddTail((uint32*)currentHeaderSize);
	}
	headerSizeList.AddTail((uint32*)lastHeaderSize);
	
	while(headerSizeList.GetCount()) 
	{
		uint32 currentHeaderSize = (uint32)headerSizeList.GetHead();
		headerSizeList.RemoveHead();

		if (FAILED(GetDeliveryBuffer(&pSample, NULL, NULL, 0)))
			return;

		pSample->GetPointer(&pData);
		memcpy(pData, codecPrivate, currentHeaderSize);
		pSample->SetActualDataLength(currentHeaderSize);
		pSample->SetTime(&tTime,&tTime);
		pSample->SetMediaTime(&tTime,&tTime);
		m_pOutQ->Receive(pSample);
		
		codecPrivate += currentHeaderSize;
	}
}

/*!
	\todo avoid the memcpy by having a DataBuffer class for DirectShow
*/
BOOL CKaxOutPin::DeliverBlock(KaxBlock *pBlock) {
	IMediaSample	*pSample;
	BYTE			*pData;
	uint64			tBlkA, tBlkB;
	REFERENCE_TIME	tStrStart, tMediaStart, tEnd, tDelta;	
	
	
	// There is no block in memory
	if (m_pTmpBlock == NULL) {
		m_pTmpBlock = pBlock;

		if(m_bSendHeader)
		{
			SendOneHeaderPerSample(m_pTrackInfos->CodecPrivate, m_pTrackInfos->CodecPrivateLen);
		}
		return TRUE;
	}
/*
	if (m_bStrReset) {
		m_bStrReset = false;
		m_tStrStart = m_pTmpBlock->GlobalTimecode() / 100;
	}
*/
	//NOTE2("CKaxOutPin::DeliverBlock(%s, m_tStrStart = %I64d)",m_pTrackInfos->Type==track_video?"video":"audio", m_tStrStart);

	// calculate starting-times (stream & media) and duration
	tBlkA = m_pTmpBlock->GlobalTimecode() / 100;
	tBlkB = pBlock->GlobalTimecode() / 100;

	tStrStart = (tBlkA - m_tStrStart);
	tMediaStart = tBlkA;
	tDelta = ((tBlkB - tBlkA)) / m_pTmpBlock->NumberFrames();

	for (int n=0; n<m_pTmpBlock->NumberFrames(); n++) {
		// fill sample-buffer
		if (FAILED(GetDeliveryBuffer(&pSample, NULL, NULL, 0))) return FALSE;
		pSample->GetPointer(&pData);
		memcpy(pData, m_pTmpBlock->GetBuffer(n).Buffer(), m_pTmpBlock->GetBuffer(n).Size());
		pSample->SetActualDataLength(m_pTmpBlock->GetBuffer(n).Size());

		// set stream-time
		tEnd = tStrStart + tDelta;
		pSample->SetTime(&tStrStart, &tEnd);
		NOTE3("CKaxOutPin::DeliverBlock Track(%d) SetTime(%I64d, %I64d);", m_pTmpBlock->TrackNum(), tStrStart, tEnd);
		tStrStart += tDelta;

		/**
		// set media-time
		// TODO : media time for video stream is the frame number!
		tEnd = tMediaStart + tDelta;
		pSample->SetMediaTime(&tMediaStart, &tEnd);
		//NOTE2("CKaxOutPin::DeliverBlock SetMediaTime(%I64d, %I64d);", tMediaStart, tEnd);
		tMediaStart += tDelta;
		/**/
		pSample->SetMediaTime(NULL, NULL);
		/**
		tEnd = m_tMediaTime+1;
		pSample->SetMediaTime(&m_tMediaTime, &tEnd);
		m_tMediaTime++;			
		/**/
		
#ifdef QUEING
		m_pOutQ->Receive(pSample);
#else
		Deliver(pSample);
		pSample->Release();
#endif
	}	
	
	delete m_pTmpBlock;
	m_pTmpBlock = pBlock;

	return TRUE;
}

void CKaxOutPin::FlushBlocks()
{
	IMediaSample	*pSample;
	BYTE			*pData;
	uint64			tBlkA;
	REFERENCE_TIME	tStrStart, tMediaStart, tEnd, tDelta;	
	
	
	// There is no block in memory
	if (m_pTmpBlock == NULL) {
		return;
	}

	tBlkA = m_pTmpBlock->GlobalTimecode() / 100;

	tStrStart = (tBlkA - m_tStrStart);
	tMediaStart = tBlkA;
	tDelta = 1;

	for (int n=0; n<m_pTmpBlock->NumberFrames(); n++) {
		// fill sample-buffer
		if (FAILED(GetDeliveryBuffer(&pSample, NULL, NULL, 0))) return;
		pSample->GetPointer(&pData);
		memcpy(pData, m_pTmpBlock->GetBuffer(n).Buffer(), m_pTmpBlock->GetBuffer(n).Size());
		pSample->SetActualDataLength(m_pTmpBlock->GetBuffer(n).Size());

		// set stream-time
		tEnd = tStrStart + tDelta;
		pSample->SetTime(&tStrStart, &tEnd);
		NOTE2("CKaxOutPin::DeliverBlock SetTime(%I64d, %I64d);", tStrStart, tEnd);
		tStrStart += tDelta;

		/**
		// set media-time
		// TODO : media time for video stream is the frame number!
		tEnd = tMediaStart + tDelta;
		pSample->SetMediaTime(&tMediaStart, &tEnd);
		//NOTE2("CKaxOutPin::DeliverBlock SetMediaTime(%I64d, %I64d);", tMediaStart, tEnd);
		tMediaStart += tDelta;
		/**/
		pSample->SetMediaTime(NULL, NULL);
		/**
		tEnd = m_tMediaTime+1;
		pSample->SetMediaTime(&m_tMediaTime, &tEnd);
		m_tMediaTime++;			
		/**/
		
#ifdef QUEING
		m_pOutQ->Receive(pSample);
#else
		Deliver(pSample);
		pSample->Release();
#endif
	}
	
	delete m_pTmpBlock;
	m_pTmpBlock = NULL;
}
