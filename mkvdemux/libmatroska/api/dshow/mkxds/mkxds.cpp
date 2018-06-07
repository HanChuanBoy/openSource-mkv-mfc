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
	\version \$Id: mkxds.cpp,v 1.30 2003/06/04 15:19:45 toff Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Christophe Paris <toffparis @ users.sf.net>
*/

#include <streams.h>
#include <initguid.h>    // declares DEFINE_GUID to declare an EXTERN_C const.
                         // Use once per project.
#include <tchar.h>
#include "mkxds.h"

#include "WinIOCallback.h"
#include "MatroskaReader.h"
#include "mkx_opin.h"
#include "mkxqueue.h"
#include "mkxread.h"
#include "mkxdsProperty.h"

// ----------------------------------------------------------------------------
// Data used to register the filter
// ----------------------------------------------------------------------------

// Object table - all com objects in this DLL
CFactoryTemplate g_Templates[2]=
{
	{ L"Matroska Filter"
		, &CLSID_MKXDEMUX
		, MkxFilter::CreateInstance
		, NULL
		, &MkxFilter::sudFilter 
	},
	{ L"Matroska Filter Property Page"
		, &CLSID_MKXFILTERPROP
		, MkxFilterProperty::CreateInstance
	}
};
int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

const AMOVIESETUP_MEDIATYPE sudOutPinTypes = {
	&MEDIATYPE_NULL,
	&MEDIASUBTYPE_NULL
};

const AMOVIESETUP_PIN sudPins =
{
		L"Output",              // Obsolete
		FALSE,                  // Is this pin rendered ?
		TRUE,                   // Is it an output pin ?
		FALSE,                  // Can the filter create zero instance ?
		FALSE,                  // Does the filter create multiple instances ?
		&CLSID_NULL,            // Obsolete
		NULL,                   // Obsolete
		1,                      // Number of types
    &sudOutPinTypes };      // Pin details

// setup data - allows the self-registration to work.
const AMOVIESETUP_FILTER MkxFilter::sudFilter = {
    g_Templates[0].m_ClsID		// clsID
  , g_Templates[0].m_Name		// strName
  , MERIT_NORMAL				// dwMerit
  , 1							// nPins
  , &sudPins					// lpPin
};

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

	// Identification by extension
	// HKEY_CLASSES_ROOT\Media Type\Extensions\.ext
	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CLASSES_ROOT,
			_T("Media Type\\Extensions\\.mkv"), 0, _T("REG_SZ"),
			REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &disp))
	{
		// We are the source filter
		const BYTE guidSource[] = "{34293064-02F2-41d5-9D75-CC5967ACA1AB}";
		RegSetValueExA(key, "Source Filter", 0, REG_SZ, guidSource, countof(guidSource));
		RegCloseKey(key);
	}

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CLASSES_ROOT,
		_T("Media Type\\Extensions\\.mka"), 0, _T("REG_SZ"),
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &disp))
	{
		// We are the source filter
		const BYTE guidSource[] = "{34293064-02F2-41d5-9D75-CC5967ACA1AB}";
		RegSetValueExA(key, "Source Filter", 0, REG_SZ, guidSource, countof(guidSource));
		RegCloseKey(key);
	}

	return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
	RegDeleteKey(HKEY_CLASSES_ROOT, _T("Media Type\\Extensions\\.mkv"));
	RegDeleteKey(HKEY_CLASSES_ROOT, _T("Media Type\\Extensions\\.mka"));

    return AMovieDllRegisterServer2(FALSE);
}

// ***********************************************************
// 
//  The MkxDs methods
//

// Provide the way for the COM support routines in <streams.h>
// to create a CKaxDemuxFilter object
CUnknown * WINAPI MkxFilter::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
	DbgSetModuleLevel(LOG_ERROR,5);
    DbgSetModuleLevel(LOG_TRACE,5); // comment this to remove trace
	//DbgSetModuleLevel(LOG_MEMORY,2);
	DbgSetModuleLevel(LOG_LOCKING,5);
	//DbgSetModuleLevel(LOG_TIMING,5);
	
    NOTE("MkxFilter::CreateInstance");

    CUnknown *pNewObject = new MkxFilter(NAME("MkxDs Object"), punk, phr );
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}

MkxFilter::MkxFilter(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
 :CSource(tszName, punk, *sudFilter.clsID, phr)
// ,m_pMKReader(NULL)
 ,m_FileHandle(NULL)
 ,m_rtStart((long)0)
 ,m_pFileName(NULL)
{
    m_rtStop = _I64_MAX / 2;
    m_rtDuration = m_rtStop;
    m_dRateSeeking = 1.0;
	
    m_dwSeekingCaps = AM_SEEKING_CanGetDuration		
		| AM_SEEKING_CanGetStopPos
		| AM_SEEKING_CanSeekForwards
        | AM_SEEKING_CanSeekBackwards
        | AM_SEEKING_CanSeekAbsolute;
}

MkxFilter::~MkxFilter()
{
	NOTE("MkxFilter::~MkxFilter");
	delete m_pRunningThread;
	delete m_FileHandle;
	delete [] m_pFileName;
}

// ============================================================================
// MkxFilter IFileSourceFilter interface
// ============================================================================

STDMETHODIMP MkxFilter::Load(LPCOLESTR lpwszFileName, const AM_MEDIA_TYPE *pmt)
{
	NOTE("MkxFilter::Load in...");
	CheckPointer(lpwszFileName, E_POINTER);	
	
	// lstrlenW is one of the few Unicode functions that works on win95
	int cch = lstrlenW(lpwszFileName) + 1;
	
	delete [] m_pFileName;
	m_pFileName = new WCHAR[cch];
	
	if (m_pFileName!=NULL)
	{
		CopyMemory(m_pFileName, lpwszFileName, cch*sizeof(WCHAR));	
	} else {
		NOTE("CCAudioSource::Load filename is empty !");
		return VFW_E_CANNOT_RENDER;
	}

#ifndef _UNICODE
	TCHAR *lpszFileName=0;
	lpszFileName = new char[cch * 2];
	if (!lpszFileName) {
		NOTE("MkxFilter::Load new lpszFileName failed E_OUTOFMEMORY");
		return E_OUTOFMEMORY;
	}
	WideCharToMultiByte(GetACP(), 0, lpwszFileName, -1,
		lpszFileName, cch, NULL, NULL);
	NOTE1("MkxFilter::Load Loading %s", lpszFileName);
#else
	TCHAR lpszFileName[MAX_PATH]={0};
	lstrcpy(lpszFileName, lpwszFileName);
#endif
	
	// Open the file & create pin
	m_FileHandle = new WinIOCallback(lpszFileName, MODE_READ);

#ifndef _UNICODE
	delete[] lpszFileName;
#endif

	if (m_FileHandle == NULL)
		return S_FALSE;

	// create the thread that will output data
	m_pRunningThread = new MkxReadThread(this, m_FileHandle);
	if (m_pRunningThread == NULL)
		return S_FALSE;
	m_pRunningThread->Create();
	m_rtDuration = m_rtStop = m_pRunningThread->GetDuration();
	
	return S_OK;
}

// ----------------------------------------------------------------------------

STDMETHODIMP MkxFilter::GetCurFile(LPOLESTR * ppszFileName, AM_MEDIA_TYPE *pmt)
{
	NOTE("MkxFilter::GetCurFile");
	
	CheckPointer(ppszFileName, E_POINTER);
	*ppszFileName = NULL;
	
	if (m_pFileName!=NULL)
	{
		DWORD n = sizeof(WCHAR)*(1+lstrlenW(m_pFileName));
		
		*ppszFileName = (LPOLESTR) CoTaskMemAlloc( n );
		if (*ppszFileName!=NULL)
		{
			CopyMemory(*ppszFileName, m_pFileName, n);
		}
	}
	
	if (pmt!=NULL)
	{
		// TODO
		NOTE("MkxFilter::GetCurFile TODO");
	}
	
	return NOERROR;
}

// ============================================================================

STDMETHODIMP MkxFilter::Pause()
{
	NOTE("MkxFilter::Pause");
	
	if (m_pRunningThread == NULL)
		return S_FALSE;
	
	if(m_State == State_Stopped)
	{
		m_pRunningThread->UnBlockProc(false);
	}
	else if(m_State == State_Running)
	{
		m_pRunningThread->BlockProc();
	}

	return CSource::Pause();
}

STDMETHODIMP MkxFilter::Run(REFERENCE_TIME tStart)
{
	NOTE("MkxFilter::Run");
	m_pRunningThread->UnBlockProc(false);
	
	return CSource::Run(tStart);
}

STDMETHODIMP MkxFilter::Stop()
{
	NOTE("MkxFilter::Stop");

	if(m_State != State_Stopped)
	{
		m_pRunningThread->Stop();
	}	

	return CSource::Stop();
}

// ============================================================================
// MkxFilter IMediaSeeking (code from CSourceSeeking in BaseClasses\ctlutil.cpp)
// ============================================================================

HRESULT MkxFilter::ChangeStart()
{
	m_pRunningThread->SeekToTimecode(m_rtStart, m_State == State_Stopped);
	return S_OK;
}

HRESULT MkxFilter::ChangeStop()
{
	return S_OK;
}

HRESULT MkxFilter::ChangeRate()
{
	return S_OK;
}

HRESULT MkxFilter::IsFormatSupported(const GUID * pFormat)
{
    CheckPointer(pFormat, E_POINTER);
    // only seeking in time (REFERENCE_TIME units) is supported
    return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : S_FALSE;
}

HRESULT MkxFilter::QueryPreferredFormat(GUID *pFormat)
{
    CheckPointer(pFormat, E_POINTER);
    *pFormat = TIME_FORMAT_MEDIA_TIME;
    return S_OK;
}

HRESULT MkxFilter::SetTimeFormat(const GUID * pFormat)
{
    CheckPointer(pFormat, E_POINTER);

    // nothing to set; just check that it's TIME_FORMAT_TIME
    return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : E_INVALIDARG;
}

HRESULT MkxFilter::IsUsingTimeFormat(const GUID * pFormat)
{
    CheckPointer(pFormat, E_POINTER);
    return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : S_FALSE;
}

HRESULT MkxFilter::GetTimeFormat(GUID *pFormat)
{
    CheckPointer(pFormat, E_POINTER);
    *pFormat = TIME_FORMAT_MEDIA_TIME;
    return S_OK;
}

HRESULT MkxFilter::GetDuration(LONGLONG *pDuration)
{
    CheckPointer(pDuration, E_POINTER);
    CAutoLock lock(&m_SeekLock);
    *pDuration = m_rtDuration;
    return S_OK;
}

HRESULT MkxFilter::GetStopPosition(LONGLONG *pStop)
{
    CheckPointer(pStop, E_POINTER);
    CAutoLock lock(&m_SeekLock);
    *pStop = m_rtStop;
    return S_OK;
}

HRESULT MkxFilter::GetCurrentPosition(LONGLONG *pCurrent)
{
    // GetCurrentPosition is typically supported only in renderers and
    // not in source filters.
    return E_NOTIMPL;
}

HRESULT MkxFilter::GetCapabilities( DWORD * pCapabilities )
{
    CheckPointer(pCapabilities, E_POINTER);
    *pCapabilities = m_dwSeekingCaps;
    return S_OK;
}

HRESULT MkxFilter::CheckCapabilities( DWORD * pCapabilities )
{
    CheckPointer(pCapabilities, E_POINTER);

    // make sure all requested capabilities are in our mask
    return (~m_dwSeekingCaps & *pCapabilities) ? S_FALSE : S_OK;
}

HRESULT MkxFilter::ConvertTimeFormat( LONGLONG * pTarget, const GUID * pTargetFormat,
                           LONGLONG    Source, const GUID * pSourceFormat )
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


HRESULT MkxFilter::SetPositions( LONGLONG * pCurrent,  DWORD CurrentFlags
                      , LONGLONG * pStop,  DWORD StopFlags )
{
    DWORD StopPosBits = StopFlags & AM_SEEKING_PositioningBitsMask;
    DWORD StartPosBits = CurrentFlags & AM_SEEKING_PositioningBitsMask;

    if(StopFlags) {
        CheckPointer(pStop, E_POINTER);

        // accept only relative, incremental, or absolute positioning
        if(StopPosBits != StopFlags) {
            return E_INVALIDARG;
        }
    }

    if(CurrentFlags) {
        CheckPointer(pCurrent, E_POINTER);
        if(StartPosBits != AM_SEEKING_AbsolutePositioning &&
           StartPosBits != AM_SEEKING_RelativePositioning) {
            return E_INVALIDARG;
        }
    }


    // scope for autolock
    {
        CAutoLock lock(&m_SeekLock);

        // set start position
        if(StartPosBits == AM_SEEKING_AbsolutePositioning)
        {
            m_rtStart = *pCurrent;
        }
        else if(StartPosBits == AM_SEEKING_RelativePositioning)
        {
            m_rtStart += *pCurrent;
        }

        // set stop position
        if(StopPosBits == AM_SEEKING_AbsolutePositioning)
        {
            m_rtStop = *pStop;
        }
        else if(StopPosBits == AM_SEEKING_IncrementalPositioning)
        {
            m_rtStop = m_rtStart + *pStop;
        }
        else if(StopPosBits == AM_SEEKING_RelativePositioning)
        {
            m_rtStop = m_rtStop + *pStop;
        }
    }


    HRESULT hr = S_OK;
    if(SUCCEEDED(hr) && StopPosBits) {
        hr = ChangeStop();
    }
    if(StartPosBits) {
        hr = ChangeStart();
    }

    return hr;
}


HRESULT MkxFilter::GetPositions( LONGLONG * pCurrent, LONGLONG * pStop )
{
    if(pCurrent) {
        *pCurrent = m_rtStart;
    }
    if(pStop) {
        *pStop = m_rtStop;
    }

    return S_OK;;
}


HRESULT MkxFilter::GetAvailable( LONGLONG * pEarliest, LONGLONG * pLatest )
{
    if(pEarliest) {
        *pEarliest = 0;
    }
    if(pLatest) {
        CAutoLock lock(&m_SeekLock);
        *pLatest = m_rtDuration;
    }
    return S_OK;
}

HRESULT MkxFilter::SetRate( double dRate)
{
    {
        CAutoLock lock(&m_SeekLock);
        m_dRateSeeking = dRate;
    }
    return ChangeRate();
}

HRESULT MkxFilter::GetRate( double * pdRate)
{
    CheckPointer(pdRate, E_POINTER);
    CAutoLock lock(&m_SeekLock);
    *pdRate = m_dRateSeeking;
    return S_OK;
}

HRESULT MkxFilter::GetPreroll(LONGLONG *pPreroll)
{
    CheckPointer(pPreroll, E_POINTER);
    *pPreroll = 0;
    return S_OK;
}

// ============================================================================
// IMkxFilter
// ============================================================================

STDMETHODIMP MkxFilter::GetPages(CAUUID *pPages)
{
    if (!pPages) return E_POINTER;

    pPages->cElems = 1;
    pPages->pElems = reinterpret_cast<GUID*>(CoTaskMemAlloc(sizeof(GUID)));
    if (pPages->pElems == NULL) 
    {
        return E_OUTOFMEMORY;
    }
    *(pPages->pElems) = CLSID_MKXFILTERPROP;
    return NOERROR;
}

// ============================================================================