
#ifndef __KAXINPIN_H__
#define __KAXINPIN_H__

#include "global.h"

using namespace LIBMATROSKA_NAMESPACE;


class CKaxDemuxFilter;

class CKaxInPin : public CBaseInputPin
{
private:
	CKaxDemuxFilter	*m_pFilter;

public:
	DECLARE_IUNKNOWN

	CKaxInPin(CBaseFilter *pFilter, CCritSec *pLock, HRESULT *phr);
	~CKaxInPin();

	STDMETHODIMP Connect(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt);
	HRESULT ReceiveConnection(IPin *pConnectPin, AM_MEDIA_TYPE *pmt);
	HRESULT CompleteConnect(IPin *pReceivePin);
	HRESULT BreakConnect(void);

	HRESULT CheckMediaType(const CMediaType *pmt);
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);

	STDMETHODIMP Receive(IMediaSample *pSample);
	STDMETHODIMP ReceiveCanBlock() { return S_OK; }

	STDMETHODIMP BeginFlush(void);
	STDMETHODIMP EndFlush(void);

	// custom stuff
private:
	CKaxDemuxFilter *GetFilter() { return m_pFilter; }
};

#endif __KAXINPIN_H__
