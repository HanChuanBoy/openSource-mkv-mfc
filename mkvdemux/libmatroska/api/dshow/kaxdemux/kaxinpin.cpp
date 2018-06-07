#include "asyncio.h"
#include "kaxinpin.h"
#include "kaxdemux.h"
#include "MatroskaReader.h"


CKaxInPin::CKaxInPin(CBaseFilter *pFilter, CCritSec *pLock, HRESULT *phr) :
 CBaseInputPin(NAME("Input Pin"), pFilter, pLock, phr, L"Input")
,m_pFilter((CKaxDemuxFilter*)pFilter)
{
}

CKaxInPin::~CKaxInPin() {
}

HRESULT CKaxInPin::Connect(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt) {
	return CBaseInputPin::Connect(pReceivePin, pmt);
}

HRESULT CKaxInPin::ReceiveConnection(IPin *pConnectPin, AM_MEDIA_TYPE *pmt) {
	return CBaseInputPin::ReceiveConnection(pConnectPin, pmt);
}

HRESULT CKaxInPin::CompleteConnect(IPin *pReceivePin) {
	HRESULT			hr = S_OK;
	IAsyncReader	*pReader = NULL;
	CAsyncIOCb		*pIOCb = NULL;

	if (pReceivePin == NULL) 
		return E_POINTER;

	if (FAILED(hr = CBaseInputPin::CompleteConnect(pReceivePin))) 
		return hr;

	if (FAILED(hr = pReceivePin->QueryInterface(IID_IAsyncReader, (void**)&pReader))) 
		return hr;

	if ((pIOCb = new CAsyncIOCb(pReader)) == NULL)
		return E_FAIL;

	return m_pFilter->CreateOutputPin((IOCallback*)pIOCb);
}

HRESULT CKaxInPin::BreakConnect() {
	return CBaseInputPin::BreakConnect();
}

HRESULT CKaxInPin::CheckMediaType(const CMediaType *pmt) {
	if (pmt == NULL) return E_POINTER;

	if ((*pmt->Type() == MEDIATYPE_Stream) && (*pmt->Subtype() == MEDIASUBTYPE_Matroska))
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CKaxInPin::GetMediaType(int iPosition, CMediaType *pMediaType) {
	if (pMediaType == NULL) return E_INVALIDARG;
	if (iPosition != 0) return VFW_S_NO_MORE_ITEMS;

	pMediaType->InitMediaType();
	pMediaType->SetType(&MEDIATYPE_Stream);
	pMediaType->SetSubtype(&MEDIASUBTYPE_Matroska);

	return S_OK;
}

HRESULT CKaxInPin::Receive(IMediaSample *pSample) {
	return CBaseInputPin::Receive(pSample);
}

HRESULT CKaxInPin::BeginFlush() {
	return CBaseInputPin::BeginFlush();
}

HRESULT CKaxInPin::EndFlush() {
	return CBaseInputPin::EndFlush();
}
