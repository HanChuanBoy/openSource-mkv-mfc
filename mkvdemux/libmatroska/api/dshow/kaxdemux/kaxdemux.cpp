//
// kaxdemux.cpp
//
// kaxdemux

#include <streams.h>     // DirectShow (includes windows.h)

#include <initguid.h>    // declares DEFINE_GUID to declare an EXTERN_C const.
                         // Use once per project.
#include <wxdebug.h>

#include "kaxoutpin.h"
#include "kaxinpin.h"
#include "kaxdemux.h"
#include "MatroskaReader.h"

// worker-thread control messages (0 not allowed)
#define Streaming_Start		1
#define Streaming_Pause		2
#define Streaming_Stop		3

//
// Common DLL routines and tables
//

// Object table - all com objects in this DLL
CFactoryTemplate g_Templates[]=
{   { L"Matroska Demuxer Filter"
    , &CLSID_KAXDEMUX
    , CKaxDemuxFilter::CreateInstance
    , NULL
    , &CKaxDemuxFilter::sudFilter 
    }
};
int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

// The streams.h DLL entrypoint.
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

// The entrypoint required by the MSVC runtimes. This is used instead
// of DllEntryPoint directly to ensure global C++ classes get initialised.
BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {

    return DllEntryPoint(reinterpret_cast<HINSTANCE>(hDllHandle), dwReason, lpreserved);
}


STDAPI DllRegisterServer()
{
	HKEY key;
	DWORD disp;
   
        // TODO : use StringFromGUID2 on CLSID instead of hardcoded GUID string

	// Identification on content :
	// HKEY_CLASSES_ROOT\MediaType\{major type}\{subtype}
	// This is the type that will be available on the source filter output pin
	// Major type : {e436eb83-524f-11ce-9f53-0020af0ba770} MEDIATYPE_Stream
	// Minor type : {E73A985A-29B1-44db-BB6A-C88DA3B9E3B2} MEDIASUBTYPE_Matroska
	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CLASSES_ROOT,
			"Media Type\\{e436eb83-524f-11ce-9f53-0020af0ba770}\\{E73A985A-29B1-44db-BB6A-C88DA3B9E3B2}",
			0, "REG_SZ", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &disp))
	{
		// We use the asynchronous file reader (CLSID_AsyncReader) as the source filter
		char* guidSource = "{E436EBB5-524F-11CE-9F53-0020AF0BA770}";
		RegSetValueEx(key, "Source Filter", 0, REG_SZ, (CONST BYTE *) guidSource, strlen(guidSource));
   
		// The pattern use the following format : offset,cb,mask,val
		char* pattern = "0,4,,1A45DFA3,18,8,,6D6174726F736B61";
		// In the pattern we check that we have
		//   - an EBML file (offset 0, 1A45DFA3)
		//   - with matroska inside (offset 18, 6D6174726F736B61 = "matroska")
		RegSetValueEx(key, "0", 0, REG_SZ, (CONST BYTE *) pattern, strlen(pattern));
		RegCloseKey(key);
	}

	// Identification by extension
	// HKEY_CLASSES_ROOT\Media Type\Extensions\.ext
	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CLASSES_ROOT,
			"Media Type\\Extensions\\.mkv", 0, "REG_SZ",
			REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &disp))
	{
		// We use the asynchronous file reader (CLSID_AsyncReader) as the source filter
		char* guidSource = "{E436EBB5-524F-11CE-9F53-0020AF0BA770}";
		RegSetValueEx(key, "Source Filter", 0, REG_SZ, (CONST BYTE *) guidSource, strlen(guidSource));
   
		// Now we define the Mediatype (MEDIATYPE_Stream) and Subtype (MEDIASUBTYPE_Matroska)
		char* mediaType = "{e436eb83-524f-11ce-9f53-0020af0ba770}";
		RegSetValueEx(key, "Media Type", 0, REG_SZ, (CONST BYTE *) mediaType, strlen(mediaType));
   
		char* subType = "{E73A985A-29B1-44db-BB6A-C88DA3B9E3B2}";
		RegSetValueEx(key, "Subtype", 0, REG_SZ, (CONST BYTE *) subType, strlen(subType));
		RegCloseKey(key);
	}

	return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
	RegDeleteKey(HKEY_CLASSES_ROOT, "Media Type\\{e436eb83-524f-11ce-9f53-0020af0ba770}\\{E73A985A-29B1-44db-BB6A-C88DA3B9E3B2}");
	RegDeleteKey(HKEY_CLASSES_ROOT, "Media Type\\Extensions\\.mkv");

    return AMovieDllRegisterServer2(FALSE);
}

//
// Filter kaxdemux routines
//

const AMOVIESETUP_MEDIATYPE sudInPinTypes = {
	&MEDIATYPE_Stream,
	&MEDIASUBTYPE_Matroska
};
const AMOVIESETUP_MEDIATYPE sudOutPinTypes = {
	&MEDIATYPE_NULL,
	&MEDIASUBTYPE_NULL
};

const AMOVIESETUP_PIN psudPins[] = {
	{ L"Input",
	FALSE, FALSE, FALSE, FALSE,
	&CLSID_NULL,
	L"Output",
	1,
	&sudInPinTypes },
	{ L"Output",
	FALSE, TRUE, FALSE, FALSE, //TRUE, TRUE,
	&CLSID_NULL,
	L"Input",
	1,
	&sudOutPinTypes }
};

// setup data - allows the self-registration to work.
const AMOVIESETUP_FILTER CKaxDemuxFilter::sudFilter = {
    g_Templates[0].m_ClsID		// clsID
  , g_Templates[0].m_Name		// strName
  , MERIT_NORMAL				// dwMerit
  , 2							// nPins
  , psudPins					// lpPin
};

CKaxDemuxFilter::CKaxDemuxFilter(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
: CBaseFilter(tszName, punk, &m_FilterLock, *sudFilter.clsID)
, m_FilterLock()
, m_pInPin(NULL)
, m_OutPins(NAME("OutputPins"))
, m_nPins(1)
, m_pStreamer(NULL)
, m_rtStart((long)0)
, m_MKReader(NULL)
{
    NOTE("CKaxDemuxFilter::CKaxDemuxFilter");

	m_dwSeekingCaps = AM_SEEKING_CanGetDuration
		/*
		| AM_SEEKING_CanSeekForwards
        | AM_SEEKING_CanSeekBackwards
        | AM_SEEKING_CanSeekAbsolute
		*/
        | AM_SEEKING_CanGetStopPos;
    m_rtStop = _I64_MAX / 2;
    m_rtDuration = m_rtStop;
    m_dRateSeeking = 1.0;
	
	m_MKReader = new MatroskaReader;
}

CKaxDemuxFilter::~CKaxDemuxFilter()
{
    NOTE("CKaxDemuxFilter::~CKaxDemuxFilter");

	// SHOULD WE FREE m_pElems ???

	if(m_pStreamer)
	{
		delete m_pStreamer;
		m_pStreamer = NULL;
	}

	delete m_MKReader;
	m_MKReader = NULL;

	if (m_pInPin != NULL) delete m_pInPin;	
	if (m_OutPins.GetCount() != 0) {
		POSITION pos = m_OutPins.GetHeadPosition();
		while (pos) {
			CKaxOutPin *pPin = m_OutPins.GetNext(pos);
			delete pPin;
		}
	}
}


// Provide the way for the COM support routines in <streams.h>
// to create a CKaxDemuxFilter object
CUnknown * WINAPI CKaxDemuxFilter::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
	DbgSetModuleLevel(LOG_ERROR,5);
    DbgSetModuleLevel(LOG_TRACE,5); // comment this to remove trace
	//DbgSetModuleLevel(LOG_MEMORY,2);
	DbgSetModuleLevel(LOG_LOCKING,5);
	
    NOTE("CKaxDemuxFilter::CreateInstance");

    CUnknown *pNewObject = new CKaxDemuxFilter(NAME("kaxdemux Object"), punk, phr );
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}

// return pointer to a pin.
CBasePin* CKaxDemuxFilter::GetPin(int n)
{
	if (n == 0) {
		if (m_pInPin == NULL) {
			HRESULT hr = S_OK;
			m_pInPin = new CKaxInPin(this, m_pLock, &hr);
			if (FAILED(hr)) return NULL;
		}

		return m_pInPin;
	}
	else {
		n--;
		if ((n < 0) || (m_OutPins.GetCount() <= n)) return NULL;
		POSITION pos = m_OutPins.GetHeadPosition();
		while (n-- > 0) m_OutPins.GetNext(pos);
		return m_OutPins.GetNext(pos);
	}
    
    return NULL;
}

HRESULT CKaxDemuxFilter::CreateOutputPin(IOCallback *pIOCb)
{
	HRESULT	hr = S_OK;

	if(m_MKReader->InitKaxFile(pIOCb) < 0)
	{
		return E_FAIL;
	}

	m_rtDuration = m_MKReader->GetDuration();

	for(int i=0; i < m_MKReader->GetTrackNumber(); i++)
	{
		
		wchar_t szPinName[32];		
		KaxTrackInfoStruct* pTrackInfo = m_MKReader->GetTrackInfo(i);
		
		// create a new outpin
		switch(pTrackInfo->Type)
		{
		case track_video:
			swprintf(szPinName, L"Video %02d", m_nPins); break;
		case track_audio:
			swprintf(szPinName, L"Audio %02d", m_nPins); break;
		case track_subtitle:
			swprintf(szPinName, L"Subtitle %02d", m_nPins); break;
		}					
		
		CKaxOutPin *pPin = new CKaxOutPin(this, GetStreamLock(), &hr, szPinName, pTrackInfo);
		
		if(FAILED(hr))
		{
			break;
		}

		m_nPins++;
		m_OutPins.AddTail(pPin);
	}

	return hr;
}


HRESULT CKaxDemuxFilter::ActivateOutPins(bool bActivate)
{
    NOTE1("CKaxDemuxFilter::ActivateOutPins(%s) in...",bActivate?"true":"false");

	HRESULT	 hr = S_OK;
	POSITION pos = m_OutPins.GetHeadPosition();
	while (pos) {
		CKaxOutPin *pPin = m_OutPins.GetNext(pos);
		if (pPin == NULL) continue;
		if (!pPin->IsConnected()) continue;
		pPin->SendAnyway();
		if (bActivate) hr = pPin->Active();
		else hr = pPin->Inactive();
		if (FAILED(hr)) return hr;
	}

	NOTE("CKaxDemuxFilter::ActivateOutPins out.");

	return S_OK;
}

HRESULT CKaxDemuxFilter::FlushOutPins(bool bBeginFlush)
{
    NOTE1("CKaxDemuxFilter::FlushOutPins(%s) in...",bBeginFlush?"true":"false");

	HRESULT	 hr = S_OK;
	POSITION pos = m_OutPins.GetHeadPosition();
	while (pos) {
		CKaxOutPin *pPin = m_OutPins.GetNext(pos);
		if (pPin == NULL) continue;
		if (!pPin->IsConnected()) continue;
		pPin->SendAnyway();
		if (bBeginFlush) hr = pPin->DeliverBeginFlush();
		else hr = pPin->DeliverEndFlush();
		if (FAILED(hr)) return hr;
	}

	NOTE("CKaxDemuxFilter::FlushOutPins out.");

	return S_OK;
}
/*
// nTrack must not be zero-based
HRESULT CKaxDemuxFilter::DeliverSample(int nTrack, IMediaSample *pSample, uint32 timecode, int nSample) {
	CAutoLock		StreamLock(&m_StreamLock);
	REFERENCE_TIME	tStart, tEnd;
	uint32			tTrack = nTrack;

	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (nTrack--) pPin = m_OutPins.GetNext(pos);
	if (pPin == NULL) return E_FAIL;
	if (!pPin->IsConnected()) return E_FAIL;
/*	if (!pPin->IsRunning()) pPin->SetTrackTime(timecode);	// stream-time is zero before Run() is called

	if (timecode == 0) {
		pSample->SetSyncPoint(FALSE);
		pSample->SetDiscontinuity(TRUE);
	}
/
	// set the stream- and mediatimes
	tTrack = timecode - pPin->GetTrackTime();
	pPin->SetTrackTime(timecode);

	// 1. stream time
	pPin->GetStrTime(&tStart);
	tStart += tTrack;
	pPin->SetStrTime(&tStart);
	tStart += nSample << 1;
	tEnd = tStart + 1;
	pSample->SetTime(&tStart, &tEnd);

	// 2. media time
	pPin->GetMediaTime(&tStart);
	tStart += timecode;
	pPin->SetMediaTime(&tStart);
	tStart += nSample << 1;
	tEnd = tStart + 1;
	pSample->SetMediaTime(&tStart, &tEnd);

	return pPin->Deliver(pSample);
}
*/
HRESULT CKaxDemuxFilter::NewSegment(int nTrack, REFERENCE_TIME tStart, REFERENCE_TIME tEnd) {
	CAutoLock		StreamLock(&m_StreamLock);
	REFERENCE_TIME	t = 0;

	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (nTrack--) pPin = m_OutPins.GetNext(pos);
	if (pPin == NULL) return E_FAIL;
	if (!pPin->IsConnected()) return E_FAIL;
	pPin->SetStrTime(&t);
	pPin->SendAnyway();
	return pPin->DeliverNewSegment(tStart, tEnd, 1.0);
}

HRESULT CKaxDemuxFilter::EndOfStream(int nTrack) {
	CAutoLock	StreamLock(&m_StreamLock);

	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (nTrack--) pPin = m_OutPins.GetNext(pos);
	if (pPin == NULL) return E_FAIL;
	if (!pPin->IsConnected()) return E_FAIL;
	pPin->SendAnyway();
	return pPin->DeliverEndOfStream();
}
/*
HRESULT CKaxDemuxFilter::GetSampleBuff(int nTrack, IMediaSample **ppSample) {
	CAutoLock	StreamLock(&m_StreamLock);

	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (nTrack--) pPin = m_OutPins.GetNext(pos);
	if (pPin == NULL) return E_FAIL;
	if (!pPin->IsConnected()) return E_FAIL;
	return pPin->GetDeliveryBuffer(ppSample, NULL, NULL, 0);
}

BOOL CKaxDemuxFilter::SetTrackBlockDuration(int nTrack, uint32 tSampleDuration) {
	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (nTrack--) pPin = m_OutPins.GetNext(pos);
	if (pPin == NULL) return FALSE;
	pPin->SetBlockDuration(tSampleDuration);
	return TRUE;
}

uint32 CKaxDemuxFilter::GetTrackBlockDuration(int nTrack) {
	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (nTrack--) pPin = m_OutPins.GetNext(pos);
	if (pPin == NULL) return 0;
	return pPin->GetBlockDuration();
}

void CKaxDemuxFilter::SetClusterTime(uint32 tClusterTime) {
	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (pos) {
		pPin = m_OutPins.GetNext(pos);
		pPin->SetClusterTime(tClusterTime);
	}
}
*/
/*!
	\todo memorise the pin in a table (instead of the while)
*/
BOOL CKaxDemuxFilter::DeliverBlock(int nTrack, KaxBlock *pBlock) {
	CAutoLock	StreamLock(&m_StreamLock);

	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (nTrack--) pPin = m_OutPins.GetNext(pos);
	if (pPin == NULL) return FALSE;
	return pPin->DeliverBlock(pBlock);
}

void CKaxDemuxFilter::FlushBlocks()
{
	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (1) {
		pPin = m_OutPins.GetNext(pos);
		if (pPin == NULL) break;
		pPin->FlushBlocks();
	}
}


HRESULT CKaxDemuxFilter::Run(REFERENCE_TIME tStart)
{
	NOTE("CKaxDemuxFilter::Run");

	CAutoLock FilterLock(&m_FilterLock);
	REFERENCE_TIME	t = 0;
	HRESULT	hr;

	hr = CBaseFilter::Run(tStart);
	if (FAILED(hr)) return hr;

	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (pos) {
		pPin = m_OutPins.GetNext(pos);
//		pPin->SetStrTime(&t);
		pPin->SetRunning(true);
		pPin->ResetStreamTime(); ///< \todo remove
	}

	return S_OK;
}

HRESULT CKaxDemuxFilter::Pause()
{
    NOTE("CKaxDemuxFilter::Pause");
    
	CAutoLock FilterLock(&m_FilterLock);
	FILTER_STATE	State;

//	ActivateOutPins(true);
	State = m_State;
	if (CBaseFilter::Pause() != S_OK) {
		return S_FALSE;
	}

	if (State == State_Paused) return S_OK;
	else if (State == State_Stopped)
	{
		for (int i=1; i<m_nPins; i++) NewSegment(i, 0, 0);

		// initialize & start worker thread
		CStrInit	strinit;
		strinit.pLock = GetStreamLock();
		strinit.pOutPins = &m_OutPins;
		strinit.pFilter = this;

		m_pStreamer = new CStreamer;
		if (!m_pStreamer->Create()) return S_FALSE;
		if (!m_pStreamer->CallWorker(Streaming_Start)) return S_FALSE;
		if (!m_pStreamer->CallWorker((DWORD)&strinit)) return S_FALSE;
	}
	else if (State == State_Running) {
		if (m_pStreamer == NULL) return S_OK;
		if (!m_pStreamer->CallWorker(Streaming_Pause)) return S_FALSE;
	}

	POSITION pos = m_OutPins.GetHeadPosition();
	CKaxOutPin *pPin;
	while (pos) {
		pPin = m_OutPins.GetNext(pos);
		pPin->SetRunning(false);
		pPin->ResetStreamTime(); ///< \todo remove
	}

	return S_OK;
}

HRESULT CKaxDemuxFilter::Stop()
{
	CAutoLock FilterLock(&m_FilterLock);

    NOTE("CKaxDemuxFilter::Stop in...");


	if (m_State == State_Stopped) return S_OK;

	// shutdown worker-thread
	if (m_pStreamer == NULL) return S_OK;
	if (m_pStreamer->IsRunning())
	{
		if (!m_pStreamer->CallWorker(Streaming_Stop)) return S_FALSE;
		for (int i=1; i<m_nPins; i++) EndOfStream(i);
		ActivateOutPins(false);
	}

	m_pStreamer->Close();
	delete m_pStreamer;
	m_pStreamer = NULL;

	// reset matroska stuff (search to 00:00:00)
	m_MKReader->Reset();
	
	m_State = State_Stopped;	

	NOTE("CKaxDemuxFilter::Stop out.");
	return S_OK;
}

DWORD CKaxDemuxFilter::CStreamer::ThreadProc() {
	CStrInit	*pStrInit;
	KaxBlock	*pBlock;
	DWORD		cmd;

	NOTE("CKaxDemuxFilter::CStreamer::ThreadProc in...");

	running = FALSE;
	while (1) {
		if ((running && CheckRequest(&cmd)) || (!running && (cmd = GetRequest()))) {
			switch (cmd) {
				case Streaming_Start:
				{
					Reply(1);
					pStrInit = (CStrInit*)GetRequest();
					if (pStrInit == NULL) {
						Reply(0);
						return 0;
					}

					// initialize internals
					memcpy(pElems, pStrInit->pElems, sizeof(EbmlElement*)*6);
					iLevel = pStrInit->iLevel;
					iUpElLev = pStrInit->iUpElLev;
					TimeScale = pStrInit->TimeScale;
					if ((pKaxIO = pStrInit->pKaxIO) == NULL) { Reply(0); return 0; }
					if ((pLock = pStrInit->pLock) == NULL) { Reply(0); return 0; }
					if ((pStream = pStrInit->pStream) == NULL) { Reply(0); return 0; }
					if ((pOutPins = pStrInit->pOutPins) == NULL) { Reply(0); return 0; }
					if ((pFilter = pStrInit->pFilter) == NULL) { Reply(0); return 0; }
					dwClusterTime = 0;
					tStr = 0;
					running = TRUE;
					Reply(1);

					break;
				}
				case Streaming_Stop:
				{
					running = FALSE;
					NOTE("CKaxDemuxFilter::CStreamer::ThreadProc out1.");
					Reply(1);
					return 1;
				}

				case Streaming_Pause:
				{
					running = FALSE;
					Reply(1);
					break;
				}
				default:
				{
					NOTE("Unknown message !!!.");
				}
			}
		}


		// deliver samples downstream (one block during each loop)

		
		if ((pBlock = pFilter->m_MKReader->GetNextBlock()) == NULL)
		{
			// End of file
			NOTE("CKaxDemuxFilter::CStreamer::ThreadProc before pLock->Lock();");
			pLock->Lock();
			pFilter->FlushBlocks();
//			pFilter->FlushOutPins(true);
//			pFilter->FlushOutPins(false);
			for (int i=1; i<=pFilter->GetNumTracks(); i++) pFilter->EndOfStream(i);
			pFilter->ActivateOutPins(false);
			pLock->Unlock();
			NOTE("CKaxDemuxFilter::CStreamer::ThreadProc break1");
			break;
		}

		if (!pFilter->DeliverBlock(pBlock->TrackNum(), pBlock))
		{			
			NOTE("CKaxDemuxFilter::CStreamer::ThreadProc break2");
			break;
		}
	}

    NOTE("CKaxDemuxFilter::CStreamer::ThreadProc out2.");
	Reply(1);
	running = FALSE;
	return 1;
}

// ----------------------------------------------------------------------------
// IMediaSeeking implementation
// ----------------------------------------------------------------------------

// inspiration taken from CSourceSeeking (in BaseClasses\ctlutil.cpp)

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::GetCapabilities(DWORD *pCapabilities)
{
    CheckPointer(pCapabilities, E_POINTER);
    *pCapabilities = m_dwSeekingCaps;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::CheckCapabilities(DWORD *pCapabilities)
{
    CheckPointer(pCapabilities, E_POINTER);
	
    // make sure all requested capabilities are in our mask
    return (~m_dwSeekingCaps & *pCapabilities) ? S_FALSE : S_OK;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::IsFormatSupported(const GUID *pFormat)
{
    CheckPointer(pFormat, E_POINTER);
    // only seeking in time (REFERENCE_TIME units) is supported
    return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::QueryPreferredFormat(GUID *pFormat)
{
    CheckPointer(pFormat, E_POINTER);
    *pFormat = TIME_FORMAT_MEDIA_TIME;
    return S_OK;
}    
HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::GetTimeFormat(GUID *pFormat) 
{ 
    CheckPointer(pFormat, E_POINTER);
    *pFormat = TIME_FORMAT_MEDIA_TIME;
    return S_OK;
}
   
HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::IsUsingTimeFormat(const GUID *pFormat)
{ 
    CheckPointer(pFormat, E_POINTER);
    return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::SetTimeFormat(const GUID *pFormat)
{ 
    CheckPointer(pFormat, E_POINTER);
	
    // nothing to set; just check that it's TIME_FORMAT_TIME
    return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::GetDuration(LONGLONG *pDuration)
{ 
    CheckPointer(pDuration, E_POINTER);
    CAutoLock lock(m_pLock);
    *pDuration = m_rtDuration;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::GetStopPosition(LONGLONG *pStop)
{ 
    CheckPointer(pStop, E_POINTER);
    CAutoLock lock(m_pLock);
    *pStop = m_rtStop;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::GetCurrentPosition(LONGLONG *pCurrent) 
{
    // GetCurrentPosition is typically supported only in renderers and
    // not in source filters.
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::ConvertTimeFormat(LONGLONG *pTarget,
															 const GUID *pTargetFormat,
															 LONGLONG Source,
															 const GUID *pSourceFormat) 
{
    CheckPointer(pTarget, E_POINTER);
    // format guids can be null to indicate current format
	
    // since we only support TIME_FORMAT_MEDIA_TIME, we don't really
    // offer any conversions.
    if(pTargetFormat == 0 || *pTargetFormat == TIME_FORMAT_MEDIA_TIME)
    {
        if(pSourceFormat == 0 || *pSourceFormat == TIME_FORMAT_MEDIA_TIME)
        {
            *pTarget = Source;
            return S_OK;
        }
    }
	
    return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::SetPositions(LONGLONG *pCurrent,
														DWORD dwCurrentFlags,
														LONGLONG *pStop,
														DWORD dwStopFlags) 
{ 
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::GetPositions(LONGLONG *pCurrent,
														LONGLONG *pStop)
{
    if(pCurrent) {
        *pCurrent = m_rtStart;
    }
    if(pStop) {
        *pStop = m_rtStop;
    }
	
    return S_OK;;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::GetAvailable(LONGLONG *pEarliest,
														LONGLONG *pLatest) 
{ 
    if(pEarliest) {
        *pEarliest = 0;
    }
    if(pLatest) {
        CAutoLock lock(m_pLock);
        *pLatest = m_rtDuration;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::SetRate(double dRate)
{ 
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::GetRate(double *pdRate) 
{ 
    CheckPointer(pdRate, E_POINTER);
    CAutoLock lock(m_pLock);
    *pdRate = m_dRateSeeking;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CKaxDemuxFilter::GetPreroll(LONGLONG *pllPreroll) 
{ 
	return E_NOTIMPL; 
}