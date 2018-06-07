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
        \version \$Id: mkxqueue.h,v 1.20 2003/06/01 16:05:09 robux4 Exp $
        \author Steve Lhomme     <robux4 @ users.sf.net>
*/

#ifndef MKXDS_MKXQUEUE_H
#define MKXDS_MKXQUEUE_H

#include <streams.h>
#include <deque>

const UINT DisplayStatsEvery = 30; // blocks

/*!
	\class
	\todo add some statistics (min, max, average queue size)
*/
template<typename _Type>
class MkxQueue : public CCritSec {
public:
	MkxQueue(UINT aMaxWriteThreshold,     UINT aMaxReadThreshold, 
	         UINT aMinWriteThreshold = 1, UINT aMinReadThreshold = 1);

	virtual ~MkxQueue();

	/*!
		\brief Retrieve a Block from the queue
		\note Block until data is coming, unless it's being flushed or at the end of stream
		\note the reader becomes the "owner" of the Block (memory freeing)
		\return a Block or NULL if the queue is being flushed or ending
	*/
	_Type * GetFrontBlock();

	/*!
		\brief Put a Block in the queue
		\param bForce pushing an element will not block the caller in this case
		\note the queue becomes the "owner" of the Block (memory freeing)
	*/
	void PushBlockBack(_Type * aBlock);

	void DisableWriteBlock();
	void EnableWriteBlock();

	/*!
		\brief Remove all Blocks from the queue and disable writing
	*/
	void Flush();

	/*!
		\brief Signal that the queue will not have any further blocks
	*/
	void EndOfStream();

	void SetMinWriteThreshold(UINT aThreshold);

	/*!
		\warning this is non blocking
	*/
	void SetMaxWriteThreshold(UINT aThreshold);
	
	/*!
		\warning this is non blocking
	*/
	void SetMinReadThreshold(UINT aThreshold);

	void SetMaxReadThreshold(UINT aThreshold);

	/*!
		\warning You have to Lock the queue when using this operator
	*/
	_Type *operator[](size_t pos);

	/*!
		\brief trigger the Pause mode of the queue (reduced queue size)
	*/
	void SetPauseMode(bool bPauseMode);

	void Reset();

#ifdef _UNICODE
	std::wstring        mName;
#else // _UNICODE
	std::string         mName;
#endif // _UNICODE

private:
	std::deque<_Type *> Blocks;
	UINT                OrigMinWriteThreshold;
	UINT                OrigMaxWriteThreshold;
	UINT                MinWriteThreshold;
	UINT                MaxWriteThreshold;
	UINT                MinReadThreshold;
	UINT                MaxReadThreshold;
	HANDLE              ReadLock;  // set or unset
	HANDLE              WriteLock; // set or unset
	HANDLE              mCanClose; // set or unset
	BOOL                Flushing;
	BOOL                Ending;
	BOOL                WriteBlockEnabled;

#ifdef _QUEUE_STATS_
	UINT MinReadSize;
	UINT AvgReadSize;
	UINT MaxWriteSize;
	UINT AvgWriteSize;
	UINT ReadBlocked;
	UINT WriteBlocked;
	UINT DisplayCounter;

	void DisplayStats();
#endif // _QUEUE_STATS_
};


// --------------------------------------------------------------------------
//            Actual code for the template

template<typename _Type>
MkxQueue<_Type>::MkxQueue(UINT aMaxWriteThreshold, UINT aMaxReadThreshold, 
	         UINT aMinWriteThreshold, UINT aMinReadThreshold)
 :OrigMinWriteThreshold(aMinWriteThreshold)
 ,OrigMaxWriteThreshold(aMaxWriteThreshold)
 ,MinWriteThreshold(aMinWriteThreshold)
 ,MaxWriteThreshold(aMaxWriteThreshold)
 ,MinReadThreshold(aMinReadThreshold)
 ,MaxReadThreshold(aMaxReadThreshold)
 ,Flushing(false)
 ,Ending(false)
 ,WriteBlockEnabled(true)
#ifdef _QUEUE_STATS_
 ,MinReadSize(0xFFFF)
 ,AvgReadSize(0)
 ,MaxWriteSize(0)
 ,AvgWriteSize(0)
 ,ReadBlocked(0)
 ,WriteBlocked(0)
 ,DisplayCounter(0)
#endif // _QUEUE_STATS_
{
	WriteLock = CreateEvent(NULL, // security
		TRUE, // manual-reset
		FALSE, // set on init
		NULL);
	ReadLock = CreateEvent(NULL, // security
		TRUE, // manual-reset
		FALSE, // set on init
		NULL);
	mCanClose = CreateEvent(NULL, // security
		TRUE, // manual-reset
		TRUE, // set on init
		NULL);
	NOTE1("New MkxQueue 0x%08X", this);
}

template<typename _Type>
MkxQueue<_Type>::~MkxQueue()
{
	NOTE("MkxQueue::~MkxQueue A");
	EndOfStream();
	Flush();
	WaitForSingleObject(mCanClose, INFINITE);
	if (WriteLock != NULL)
		CloseHandle(WriteLock);
	if (ReadLock != NULL)
		CloseHandle(ReadLock);
	if (mCanClose != NULL)
		CloseHandle(mCanClose);

#ifdef _QUEUE_STATS_
	DisplayStats();
#endif // _QUEUE_STATS_
	NOTE("MkxQueue::~MkxQueue B");
}

template<typename _Type>
_Type * MkxQueue<_Type>::GetFrontBlock()
{
	_Type *result;
	Lock(); // Lock the resource (to prevent other threads to access it)
	
	//NOTE2("MkxQueue::GetBlockFront %d Blocks.size() == %d", ID, Blocks.size());

	if (!Ending && !Flushing) {
		// We should block ourself if the MinThreshold is not reached
		while (Blocks.size() < MinReadThreshold && !Ending && !Flushing) {
			//NOTE2("Locking Reading of queue 0x%08X on thread %d", this, GetCurrentThreadId());
#ifdef _QUEUE_STATS_
			ReadBlocked++;
#endif // _QUEUE_STATS_
			Unlock();
			// possible deadlock here if PopFrontBlock is called
			WaitForSingleObject(ReadLock, INFINITE);
			ResetEvent(ReadLock);
			Lock();
		}
	}  
	
	if (Blocks.size()) {
#ifdef _QUEUE_STATS_
		AvgReadSize = (AvgReadSize + Blocks.size()) >> 1;
		if (MinReadSize > Blocks.size())
			MinReadSize = Blocks.size();
		DisplayCounter++;
		if (DisplayCounter > DisplayStatsEvery) {
			DisplayStats();
			DisplayCounter = 0;
		}
#endif // _QUEUE_STATS_
		result = Blocks.front();
		Blocks.pop_front();
		if ((WaitForSingleObject(WriteLock, 0) != WAIT_OBJECT_0) &&
				(Blocks.size() <= MinWriteThreshold)) {
			SetEvent(WriteLock);
		}
	} else {
		result = NULL;
	}
//		NOTE2("MkxQueue::GetFrontBlock 0x%08X in queue 0x%08x", result, this);
	
	Unlock();
	return result;
}

template<typename _Type>
void MkxQueue<_Type>::PushBlockBack(_Type * aBlock)
{
	Lock();
	
	if (!Ending && !Flushing) {
		while (Blocks.size() > MaxWriteThreshold && WriteBlockEnabled && !Flushing && !Ending) {
#ifdef _QUEUE_STATS_
			WriteBlocked++;
#endif // _QUEUE_STATS_
			Unlock();
			ResetEvent(mCanClose);
			WaitForSingleObject(WriteLock, INFINITE);
			if (Ending) {
				return;
			}
			ResetEvent(WriteLock);
			SetEvent(mCanClose);
			Lock();
		}
		if (!Ending && !Flushing) {
#ifdef _QUEUE_STATS_
			AvgWriteSize = (AvgWriteSize + Blocks.size()) >> 1;
			if (MaxWriteSize < Blocks.size())
				MaxWriteSize = Blocks.size();
			DisplayCounter++;
			if (DisplayCounter > DisplayStatsEvery) {
				DisplayStats();
				DisplayCounter = 0;
			}
#endif // _QUEUE_STATS_
			//NOTE2("MkxQueue::PushBlockBack 0x%08X in queue 0x%08x", aBlock, this);
			Blocks.push_back(aBlock);
		}
	}

	// Unblock reading
	if ((Ending || (Blocks.size() >= MaxReadThreshold)) &&
		(WaitForSingleObject(ReadLock, 0) != WAIT_OBJECT_0)) {
		SetEvent(ReadLock);
	}

	Unlock();
}

template<typename _Type>
void MkxQueue<_Type>::DisableWriteBlock()
{
	Lock();
	WriteBlockEnabled = false;
	SetEvent(WriteLock);
	Unlock();
}

template<typename _Type>
void MkxQueue<_Type>::EnableWriteBlock()
{
	Lock();
	WriteBlockEnabled = true;
	Unlock();
}

template<typename _Type>
void MkxQueue<_Type>::Flush()
{
	Lock();
NOTE("MkxQueue::Flush");
	Flushing = true;
	SetEvent(ReadLock); // deblock all threads
	SetEvent(WriteLock); // deblock all threads
	while(Blocks.size())
	{        
		_Type *b = Blocks.front();
		Blocks.pop_front();
		delete b;
	}
	Unlock();
}

/*!
	\brief Signal that the queue will not have any further blocks
*/
template<typename _Type>
void MkxQueue<_Type>::EndOfStream()
{
	Lock();
NOTE("MkxQueue::EndOfStream");
	Ending = true;
	SetEvent(ReadLock); // deblock all threads
	SetEvent(WriteLock); // deblock all threads
	Unlock();
}

template<typename _Type>
void MkxQueue<_Type>::SetMinWriteThreshold(UINT aThreshold)
{
	Lock();
	OrigMinWriteThreshold = aThreshold;
	// deblock if it was blocked
	if ((WaitForSingleObject(WriteLock, 0) != WAIT_OBJECT_0) &&
		(Blocks.size() <= MinWriteThreshold)) {
		SetEvent(WriteLock);
	}
	Unlock();
}

/*!
	\warning this is non blocking
*/
template<typename _Type>
void MkxQueue<_Type>::SetMaxWriteThreshold(UINT aThreshold)
{
	Lock();
	OrigMaxWriteThreshold = aThreshold;
	Unlock();
}

/*!
	\warning this is non blocking
*/
template<typename _Type>
void MkxQueue<_Type>::SetMinReadThreshold(UINT aThreshold)
{
	Lock();
	MinReadThreshold = aThreshold;
	Unlock();
}

template<typename _Type>
void MkxQueue<_Type>::SetMaxReadThreshold(UINT aThreshold)
{
	Lock();
	MaxReadThreshold = aThreshold;
	if ((WaitForSingleObject(ReadLock, 0) != WAIT_OBJECT_0) &&
		(Ending || (Blocks.size() >= MaxReadThreshold))) {
		SetEvent(ReadLock);
	}
	Unlock();
}

/*!
	\warning You have to Lock the queue when using this operator
*/
template<typename _Type>
_Type * MkxQueue<_Type>::operator[](size_t pos)
{
	_Type *b;
	if (Ending || Flushing || pos >= Blocks.size()) {
		b = NULL;
	} else {
		b = Blocks[pos];
	}
	return b;
}

template<typename _Type>
void MkxQueue<_Type>::Reset()
{
	Lock();
NOTE("MkxQueue::Reset");
	Ending = Flushing = false;
	ResetEvent(ReadLock);
	ResetEvent(WriteLock);
	Unlock();
}

template<typename _Type>
void MkxQueue<_Type>::SetPauseMode(bool bPauseMode)
{
	Lock();
NOTE1("MkxQueue::SetPauseMode %s", bPauseMode?TEXT("true"):TEXT("false"));
	if (bPauseMode) {
		MaxWriteThreshold = OrigMaxWriteThreshold / 3 + 1; // +1 just in case
		MinWriteThreshold = OrigMinWriteThreshold / 3 + 1;
	} else {
		MaxWriteThreshold = OrigMaxWriteThreshold;
		MinWriteThreshold = OrigMinWriteThreshold;
	}
	Unlock();
}

#ifdef _QUEUE_STATS_
template<typename _Type>
void MkxQueue<_Type>::DisplayStats()
{
	NOTE4("MkxQueue %s - 0x%08x : Read Blocked : %d / Write Blocked %d", mName.c_str(), this, ReadBlocked, WriteBlocked);
	NOTE4("Average Read queue size %d (%d) / Average Write queue size %d (%d)", AvgReadSize, MinReadThreshold, AvgWriteSize, MaxWriteThreshold);
	NOTE2("Min Read queue size %d / Max Write queue size %d", MinReadSize, MaxWriteSize);
}
#endif // _QUEUE_STATS_

#endif // MKXDS_MKXQUEUE_H

