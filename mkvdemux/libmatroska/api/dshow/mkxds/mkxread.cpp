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
	\version \$Id: mkxread.cpp,v 1.29 2003/06/05 14:15:12 toff Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/

#include <cassert>
#include <streams.h>
#include "mkxread.h"

#include "global.h"
#include "mkxds.h"
#include "mkx_opin.h"

const bool bSeekToPreciseBlock = true;

using namespace LIBMATROSKA_NAMESPACE;

MkxReadThread::MkxReadThread(MkxFilter * aFilter, IOCallback * aFileHandle)
	:mFilter(aFilter)
	,TrackPinTable(NULL)
{
	m_MKReader.InitKaxFile(aFileHandle);
}

MkxReadThread::~MkxReadThread()
{
	NOTE("MkxReadThread::~MkxReadThread");
	BlockProc();
	CallWorker(CMD_STOP);
	Close();
	delete [] TrackPinTable;
}

/*!
	\brief read blocks and release frames of cluster when blocks have been read
	\warning Blocks output in queue must be freed by the queue reader
	\todo Clusters when it's finished reading and there is no further frame reference (from the queue reader)
*/
DWORD MkxReadThread::ThreadProc()
{
	NOTE("MkxReadThread::ThreadProc");
	assert(mFilter != NULL);

	while (1) {
		DWORD aCmd;
		bool bStop = false;
		aCmd = GetRequest();
		switch(aCmd) {
		case CMD_STOP:
			bStop = true;
			Reply(NOERROR);
			break;
		case CMD_SLEEP:
			Reply(NOERROR);
			continue; // wait for the next command
			break;
		case CMD_WAKEUP:
			Reply(NOERROR);
			break;
		default:
			NOTE1("UNKNOWN command 0x%08x!!!", aCmd);
			Reply(S_FALSE);
		}
		if (bStop)
			break;

		while (1) {
			KaxBlockGroup * ReadBlockG = m_MKReader.GetNextBlockG();

			if(ReadBlockG == NULL)
			{
				// End of stream
				for (UINT i=0; i<m_MKReader.GetTrackNumber(); i++) {
					TrackPinTable[i].Pin->BlockQueue().EndOfStream();
				}
				break; // wait for new commands
			} else {
				KaxBlock *ReadBlock = static_cast<KaxBlock *>(ReadBlockG->FindFirstElt(KaxBlock::ClassInfos, false));
				if (ReadBlock == NULL) {
					// this block is useless
					delete ReadBlockG;
				} else {
					// find the OutputQueue corresponding to the track
					MkxOutPin * ThePin = PinForTrack(ReadBlock->TrackNum());

					// as it is blocking some call to stop the thread through ReaderEvent may produce a deadlock
					if(ThePin->IsConnected() && ThePin->IsProcessing())
					{
						ThePin->BlockQueue().PushBlockBack(ReadBlockG);
					} else {
						delete ReadBlockG;
					}
				}
			}
			if (CheckRequest(&aCmd))
				break;
		}
	}

	return 0;
}

BOOL MkxReadThread::Create(void)
{
	NOTE("MkxReadThread::Create");

	CreateOutputPin();

	for (UINT i=0; i<m_MKReader.GetTrackNumber(); i++) {
		TrackPinTable[i].Pin->BlockQueue().Reset();		
	}

	if (!CAMThread::Create())
		return FALSE;

	return SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL);
}

void MkxReadThread::Stop()
{
	NOTE("MkxReadThread::Stop");
	BlockProc();

	for (UINT i=0; i<m_MKReader.GetTrackNumber(); i++) {
		TrackPinTable[i].Pin->BlockQueue().EndOfStream();
	}
}

HRESULT MkxReadThread::CreateOutputPin()
{
	HRESULT	hr = S_OK;

	delete [] TrackPinTable;
	TrackPinTable = new TrackPin[m_MKReader.GetTrackNumber()];
	if (TrackPinTable == NULL)
		return S_FALSE;

	for(int i=0; i < m_MKReader.GetTrackNumber(); i++)
	{
		
		TCHAR szPinName[32];		
		KaxTrackInfoStruct & TrackInfo = m_MKReader.GetTrackInfo(i);

		// create a new outpin
		switch(TrackInfo.Type)
		{
		case track_video:
			
			wsprintf(szPinName, _T("Video %02d %s"), TrackInfo.Number, TrackInfo.Name.c_str()); break;
		case track_audio:
			wsprintf(szPinName, _T("Audio %02d %s"), TrackInfo.Number, TrackInfo.Name.c_str()); break;
		case track_subtitle:
			wsprintf(szPinName, _T("Subtitle %02d %s"), TrackInfo.Number, TrackInfo.Name.c_str()); break;
		}			
		
#ifdef UNICODE
		TrackPinTable[i].Pin = new MkxOutPin(_T("Output"), &hr, mFilter, szPinName, TrackInfo);
#else // UNICODE*/
		WCHAR szwPinName[32];
		MultiByteToWideChar(CP_ACP, 0, szPinName, -1, szwPinName, NUMELMS(szwPinName)) ;
		TrackPinTable[i].Pin = new MkxOutPin(_T("Output"), &hr, mFilter, szwPinName, TrackInfo);
#endif // UNICODE

		TrackPinTable[i].TrackNum = TrackInfo.Number;
		
		if(FAILED(hr))
		{
			break;
		}
	}

	return hr;
}

MkxOutPin *MkxReadThread::PinForTrack(UINT aTrackNum) const
{
	if (TrackPinTable == NULL)
		return NULL;

	for (UINT i=0; i<m_MKReader.GetTrackNumber(); i++) {
		if (TrackPinTable[i].TrackNum == aTrackNum)
			return TrackPinTable[i].Pin;
	}
	return NULL;
}

/*!
	\brief return when the thread is successfully blocked in a safe state
*/
void MkxReadThread::BlockProc()
{
	NOTE("MkxReadThread::BlockProc");
	// deblock the thread To be able to use CallWorker
	for (UINT i=0; i<m_MKReader.GetTrackNumber(); i++) {
		TrackPinTable[i].Pin->BlockQueue().DisableWriteBlock();		
	}
	CallWorker(CMD_SLEEP);
	// enable back the Queue blocking
	for (i=0; i<m_MKReader.GetTrackNumber(); i++) {
		TrackPinTable[i].Pin->BlockQueue().EnableWriteBlock();		
	}
}

void MkxReadThread::UnBlockProc(bool bPauseMode)
{
	NOTE1("MkxReadThread::UnBlockProc %s", bPauseMode?TEXT("true"):TEXT("false"));
	// deblock the thread To be able to use CallWorker
	for (UINT i=0; i<m_MKReader.GetTrackNumber(); i++) {
		TrackPinTable[i].Pin->BlockQueue().DisableWriteBlock();
		TrackPinTable[i].Pin->BlockQueue().SetPauseMode(bPauseMode);
		TrackPinTable[i].Pin->SetProcessingFlag();
	}
	CallWorker(CMD_WAKEUP);
	// enable back the Queue blocking
	for (i=0; i<m_MKReader.GetTrackNumber(); i++) {
		TrackPinTable[i].Pin->BlockQueue().EnableWriteBlock();
	}	
}

void MkxReadThread::SeekToTimecode(uint64 aTimecode, bool bPauseMode)
{
	BlockProc(); // stop the reader thread
NOTE1("MkxReadThread::SeekToTimecode A %s", bPauseMode?TEXT("true"):TEXT("false"));

	// flush our output queues
	for (UINT i=0; i<m_MKReader.GetTrackNumber(); i++) {
		TrackPinTable[i].Pin->BlockQueue().Flush();
		TrackPinTable[i].Pin->DeliverBeginFlush();
		TrackPinTable[i].Pin->Stop();
		TrackPinTable[i].Pin->SetNewStartTime(aTimecode);
	}

	// position to the location corresponding to the timecode
	m_MKReader.JumpToTimecode(aTimecode, bSeekToPreciseBlock);

	for (i=0; i<m_MKReader.GetTrackNumber(); i++) {
		TrackPinTable[i].Pin->BlockQueue().Reset();
		TrackPinTable[i].Pin->DeliverEndFlush();
		TrackPinTable[i].Pin->Run();
	}

	UnBlockProc(bPauseMode); // resume the reader thread
}

