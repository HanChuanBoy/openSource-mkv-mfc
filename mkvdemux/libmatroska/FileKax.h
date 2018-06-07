/****************************************************************************
** libmatroska : parse Matroska files, see http://www.matroska.org/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Steve Lhomme.  All rights reserved.
**
** This file is part of libmatroska.
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
	\version \$Id: FileKax.h,v 1.2 2003/04/28 10:06:10 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_FILE_H
#define LIBMATROSKA_FILE_H

//#include <vector>

#include "KaxTypes.h"
#include "IOCallback.h"
//#include "MainHeader.h"
//#include "TrackType.h"
//#include "StreamInfo.h"
//#include "Cluster.h"
//#include "CodecHeader.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

//class Track;
//class Frame;

/*!
    \class FileMatroska
    \brief General container of all the parameters and data of an Matroska file
    \todo Handle the filename and next filename
    \todo Handle the IOCallback selection/type
*/
class FileMatroska {
    public:
		FileMatroska(IOCallback & output);
		~FileMatroska();
#ifdef OLD
	uint32 RenderHead(const std::string & aEncoderApp);
	uint32 ReadHead();
	uint32 ReadTracks();
	uint32 ReadCodec();
	void Close(const uint32 aTimeLength);

	inline void type_SetInfo(const std::string & aStr) {myMainHeader.type_SetInfo(aStr);}
	inline void type_SetAds(const std::string & aStr) {myMainHeader.type_SetAds(aStr);}
	inline void type_SetSize(const std::string & aStr) {myMainHeader.type_SetSize(aStr);}
	inline void type_SetSize(const uint64 aSize) {myMainHeader.type_SetSize(aSize);}

	inline uint8 GetTrackNumber() const { return myTracks.size(); }

	void track_SetName(Track * aTrack, const std::string & aName);
	void track_SetLaced(Track * aTrack, const bool bLaced = true);

	Track * CreateTrack(const track_type aType);
	inline Track * GetTrack(const uint8 aTrackNb) const
	{
	    if (aTrackNb > myTracks.size())
		return NULL;
	    else
		return myTracks[aTrackNb-1];
	}

	void Track_GetInfo(const Track * aTrack, TrackInfo & aTrackInfo) const;
	
	void Track_SetInfo_Audio(Track * aTrack, const TrackInfoAudio & aTrackInfo);
	void Track_GetInfo_Audio(const Track * aTrack, TrackInfoAudio & aTrackInfo) const;

	void Track_SetInfo_Video(Track * aTrack, const TrackInfoVideo & aTrackInfo);
	void Track_GetInfo_Video(const Track * aTrack, TrackInfoVideo & aTrackInfo) const;

	void SelectReadingTrack(Track * aTrack, bool select = true);

	/*!
	    \return wether the frame has been added or not
	*/
	bool AddFrame(Track * aTrack, const uint32 aTimecode, const binary *aFrame, const uint32 aFrameSize,
		     const bool aKeyFrame = true, const bool aBFrame = false);

	/*!
	    \return wether the frame has been read or not
	*/
	bool ReadFrame(Track * & aTrack, uint32 & aTimecode, const binary * & aFrame, uint32 & aFrameSize,
		     bool & aKeyFrame, bool & aBFrame);

	/*
	    Render the pending cluster to file
	*/
	void Flush();

	void SetMaxClusterSize(const uint32 value);
	void SetMinClusterSize(const uint32 value) {myMinClusterSize = value;}

    protected:
	MainHeader myMainHeader;

	std::vector<Track *> myTracks;
	std::vector<uint8> mySelectedTracks;

//	Track *findTrack(Track * aTrack) const;

	Cluster  myCurrWriteCluster; /// \todo merge with the write one ?
	uint32   myReadBlockNumber;
	Cluster  myCurrReadCluster;
	binary * myCurrReadBlock;      ///< The buffer containing the current read block
	uint32   myCurrReadBlockSize;  ///< The size of the buffer containing the current read block
	uint8    myCurrReadBlockTrack; ///< The track number of the current track to read

	uint32 myMaxClusterSize;
	uint32 myMinClusterSize;

	StreamInfo myStreamInfo;

	CodecHeader myCodecHeader;

	inline bool IsMyTrack(const Track * aTrack) const;
	inline bool IsReadingTrack(const uint8 aTrackNum) const;
#endif // OLD
	IOCallback & myFile;

};

END_LIBMATROSKA_NAMESPACE

#endif // FILE_KAX_HPP
