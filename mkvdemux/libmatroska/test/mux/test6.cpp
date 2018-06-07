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
    \version \$Id: test6.cpp,v 1.30 2003/06/13 18:54:00 mosu Exp $
    \brief Test muxing two tracks into valid clusters/blocks/frames
    \author Steve Lhomme     <robux4 @ users.sf.net>
*/

#ifdef _MSC_VER
#include <windows.h>      // for min/max
#endif // _MSC_VER

#include <iostream>

#include "StdIOCallback.h"

#include "EbmlHead.h"
#include "EbmlSubHead.h"
#include "EbmlVoid.h"
#include "FileKax.h"
#include "KaxSegment.h"
#include "KaxTracks.h"
#include "KaxTrackEntryData.h"
#include "KaxTrackAudio.h"
#include "KaxTrackVideo.h"
#include "KaxCluster.h"
#include "KaxClusterData.h"
#include "KaxBlock.h"
#include "KaxBlockData.h"
#include "KaxSeekHead.h"
#include "KaxCues.h"
#include "KaxInfo.h"
#include "KaxInfoData.h"

using namespace LIBMATROSKA_NAMESPACE;
using namespace std;

unsigned int BIN_FILE_SIZE = 15000;
unsigned int TXT_FILE_SIZE = 3000;
const unsigned int BIN_FRAME_SIZE = 1500;
const unsigned int TXT_FRAME_SIZE = 200;
const uint64  TIMECODE_SCALE = 1000000; 

const bool bWriteDefaultValues = false;

/*!
    The first file is a "binary" file with data scaling from 0x00 to 0xFF repeatedly
    The second file is a "text" file with data scaling from 'z' to 'a'
*/
int main(int argc, char **argv)
{
#if 0
	char *file1;
	char *file2;
	if (argc == 1)
	{
		file1 = "binr.bin";
		// create the files
		StdIOCallback bin_file(file1, MODE_CREATE);
		char c;
		uint64 i;
		for (i=0; i<BIN_FILE_SIZE; i++)
		{
			c = char(i % 0x100);
			bin_file.write(&c, 1);
		}
		bin_file.close();

		file2 = "text.bin";
		StdIOCallback txt_file(file2, MODE_CREATE);
		for (i=TXT_FILE_SIZE; i>0; i--)
		{
			c = char((i % 26) + 'a');
			txt_file.write(&c, 1);
		}
		txt_file.close();
	}
	else if(argc < 3)
	{
		cout << "Syntax: mkvmux [binfile txtfile]" << endl;
		return 1; // error
	}
	else
	{
		file1 = argv[1];
		file2 = argv[2];
	}
#endif // 0

    cout << "Creating \"muxed.mkv\"" << endl;

    try {
		// write the head of the file (with everything already configured)
		StdIOCallback out_file("muxed.mkv", MODE_CREATE);

		///// Writing EBML test
		EbmlHead FileHead;

		EDocType & MyDocType = GetChild<EDocType>(FileHead);
		*static_cast<EbmlString *>(&MyDocType) = "matroska";

		EDocTypeVersion & MyDocTypeVer = GetChild<EDocTypeVersion>(FileHead);
		*(static_cast<EbmlUInteger *>(&MyDocTypeVer)) = 1;

		EDocTypeReadVersion & MyDocTypeReadVer = GetChild<EDocTypeReadVersion>(FileHead);
		*(static_cast<EbmlUInteger *>(&MyDocTypeReadVer)) = 1;

		FileHead.Render(out_file, bWriteDefaultValues);

		KaxSegment FileSegment;

		// size is unknown and will always be, we can render it right away
		uint64 SegmentSize = FileSegment.WriteHead(out_file, 5, bWriteDefaultValues);
		
		KaxTracks & MyTracks = GetChild<KaxTracks>(FileSegment);

		// reserve some space for the Meta Seek writen at the end		
		EbmlVoid Dummy;
		Dummy.SetSize(300); // 300 octets
		Dummy.Render(out_file, bWriteDefaultValues);

		KaxSeekHead MetaSeek;

		// fill the mandatory Info section
		KaxInfo & MyInfos = GetChild<KaxInfo>(FileSegment);
		KaxTimecodeScale & TimeScale = GetChild<KaxTimecodeScale>(MyInfos);
		*(static_cast<EbmlUInteger *>(&TimeScale)) = TIMECODE_SCALE;

		KaxDuration & SegDuration = GetChild<KaxDuration>(MyInfos);
		*(static_cast<EbmlFloat *>(&SegDuration)) = 0.0;

		*((EbmlUnicodeString *)&GetChild<KaxMuxingApp>(MyInfos))  = L"libmatroska 0.4.3";
		*((EbmlUnicodeString *)&GetChild<KaxWritingApp>(MyInfos)) = L"liba";
		GetChild<KaxWritingApp>(MyInfos).SetDefaultSize(25);

		uint32 InfoSize = MyInfos.Render(out_file);
		MetaSeek.IndexThis(MyInfos, FileSegment);

		// fill track 1 params
		KaxTrackEntry & MyTrack1 = GetChild<KaxTrackEntry>(MyTracks);
		MyTrack1.SetGlobalTimecodeScale(TIMECODE_SCALE);

		KaxTrackNumber & MyTrack1Number = GetChild<KaxTrackNumber>(MyTrack1);
		*(static_cast<EbmlUInteger *>(&MyTrack1Number)) = 1;

		KaxTrackUID & MyTrack1UID = GetChild<KaxTrackUID>(MyTrack1);
		*(static_cast<EbmlUInteger *>(&MyTrack1UID)) = 7;

		*(static_cast<EbmlUInteger *>(&GetChild<KaxTrackType>(MyTrack1))) = track_audio;

		KaxCodecID & MyTrack1CodecID = GetChild<KaxCodecID>(MyTrack1);
		*static_cast<EbmlString *>(&MyTrack1CodecID) = "Dummy Audio Codec";

		MyTrack1.EnableLacing(true);

		// audio specific params
		KaxTrackAudio & MyTrack1Audio = GetChild<KaxTrackAudio>(MyTrack1);
		
		KaxAudioSamplingFreq & MyTrack1Freq = GetChild<KaxAudioSamplingFreq>(MyTrack1Audio);
		*(static_cast<EbmlFloat *>(&MyTrack1Freq)) = 44100.0;
		MyTrack1Freq.ValidateSize();

		KaxAudioChannels & MyTrack1Channels = GetChild<KaxAudioChannels>(MyTrack1Audio);
		*(static_cast<EbmlUInteger *>(&MyTrack1Channels)) = 2;

		// fill track 2 params
		KaxTrackEntry & MyTrack2 = GetNextChild<KaxTrackEntry>(MyTracks, MyTrack1);
		MyTrack2.SetGlobalTimecodeScale(TIMECODE_SCALE);

		KaxTrackNumber & MyTrack2Number = GetChild<KaxTrackNumber>(MyTrack2);
		*(static_cast<EbmlUInteger *>(&MyTrack2Number)) = 200;

		KaxTrackUID & MyTrack2UID = GetChild<KaxTrackUID>(MyTrack2);
		*(static_cast<EbmlUInteger *>(&MyTrack2UID)) = 13;

		*(static_cast<EbmlUInteger *>(&GetChild<KaxTrackType>(MyTrack2))) = track_video;

		KaxCodecID & MyTrack2CodecID = GetChild<KaxCodecID>(MyTrack2);
		*static_cast<EbmlString *>(&MyTrack2CodecID) = "Dummy Video Codec";

		MyTrack2.EnableLacing(false);

		// video specific params
		KaxTrackVideo & MyTrack2Video = GetChild<KaxTrackVideo>(MyTrack2);

		KaxVideoPixelHeight & MyTrack2PHeight = GetChild<KaxVideoPixelHeight>(MyTrack2Video);
		*(static_cast<EbmlUInteger *>(&MyTrack2PHeight)) = 200;

		KaxVideoPixelWidth & MyTrack2PWidth = GetChild<KaxVideoPixelWidth>(MyTrack2Video);
		*(static_cast<EbmlUInteger *>(&MyTrack2PWidth)) = 320;

		uint64 TrackSize = MyTracks.Render(out_file, bWriteDefaultValues);

		MetaSeek.IndexThis(MyTracks, FileSegment);


		// creation of the original binary/raw files
#if 0
		StdIOCallback bin_file(file1, MODE_READ);
		StdIOCallback txt_file(file2, MODE_READ);
		
		bin_file.setFilePointer(0L, seek_end);
		txt_file.setFilePointer(0L, seek_end);
		
		BIN_FILE_SIZE = (unsigned int) bin_file.getFilePointer();
		TXT_FILE_SIZE = (unsigned int) txt_file.getFilePointer();
		
		binary *buf_bin = new binary[BIN_FILE_SIZE];
		binary *buf_txt = new binary[TXT_FILE_SIZE];

		// start muxing (2 binary frames for each text frame)
		bin_file.setFilePointer(0L, seek_beginning);
		txt_file.setFilePointer(0L, seek_beginning);

		bin_file.read(buf_bin, BIN_FILE_SIZE);
		txt_file.read(buf_txt, TXT_FILE_SIZE);

		binary *curs_bin = buf_bin;
		binary *curs_txt = buf_txt;
		unsigned int remain_bin = BIN_FILE_SIZE;
		unsigned int remain_txt = TXT_FILE_SIZE;
#endif // 0

	// "manual" filling of a cluster"
	/// \todo whenever a BlockGroup is created, we should memorize it's position
	KaxCues AllCues;
	AllCues.SetGlobalTimecodeScale(TIMECODE_SCALE);

	KaxCluster Clust1;
	Clust1.SetParent(FileSegment); // mandatory to store references in this Cluster
	Clust1.SetPreviousTimecode(0, TIMECODE_SCALE); // the first timecode here

	// automatic filling of a Cluster
	// simple frame
	KaxBlockGroup *MyNewBlock, *MyLastBlockTrk1 = NULL, *MyLastBlockTrk2 = NULL, *MyNewBlock2;
	DataBuffer *data7 = new DataBuffer((binary *)"tototototo", countof("tototototo"));
	Clust1.AddFrame(MyTrack1, 250 * TIMECODE_SCALE, *data7, MyNewBlock);
	if (MyNewBlock != NULL)
		MyLastBlockTrk1 = MyNewBlock;
	DataBuffer *data6 = new DataBuffer((binary *)"tototototo", countof("tototototo"));
	Clust1.AddFrame(MyTrack1, 270 * TIMECODE_SCALE, *data6, MyNewBlock); // to test lacing
	if (MyNewBlock != NULL) {
		MyLastBlockTrk1 = MyNewBlock;
	} else {
		MyLastBlockTrk1->SetBlockDuration(50 * TIMECODE_SCALE);
	}
	DataBuffer *data5 = new DataBuffer((binary *)"tototototo", countof("tototototo"));
	Clust1.AddFrame(MyTrack2, 23 * TIMECODE_SCALE, *data5, MyNewBlock); // to test with another track

	// add the "real" block to the cue entries
	AllCues.AddBlockGroup(*MyLastBlockTrk1);

	// frame for Track 2
	DataBuffer *data8 = new DataBuffer((binary *)"tttyyy", countof("tttyyy"));
	Clust1.AddFrame(MyTrack2, 107 * TIMECODE_SCALE, *data8, MyNewBlock, *MyLastBlockTrk2);

	AllCues.AddBlockGroup(*MyNewBlock);

	// frame with a past reference
	DataBuffer *data4 = new DataBuffer((binary *)"tttyyy", countof("tttyyy"));
	Clust1.AddFrame(MyTrack1, 300 * TIMECODE_SCALE, *data4, MyNewBlock, *MyLastBlockTrk1);

	// frame with a past & future reference
	if (MyNewBlock != NULL) {
		DataBuffer *data3 = new DataBuffer((binary *)"tttyyy", countof("tttyyy"));
		if (Clust1.AddFrame(MyTrack1, 280 * TIMECODE_SCALE, *data3, MyNewBlock2, *MyLastBlockTrk1, *MyNewBlock)) {
			MyNewBlock2->SetBlockDuration(20 * TIMECODE_SCALE);
			MyLastBlockTrk1 = MyNewBlock2;
		} else {
			printf("Error adding a frame !!!");
		}
	}

	AllCues.AddBlockGroup(*MyLastBlockTrk1);
//AllCues.UpdateSize();

		// simulate the writing of the stream :
		// - write an empty element with enough size for the cue entry
		// - write the cluster(s)
		// - seek back in the file and write the cue entry over the empty element

		uint64 ClusterSize = Clust1.Render(out_file, AllCues, bWriteDefaultValues);
		Clust1.ReleaseFrames();
		MetaSeek.IndexThis(Clust1, FileSegment);

		KaxCluster Clust2;
		Clust2.SetParent(FileSegment); // mandatory to store references in this Cluster
		Clust2.SetPreviousTimecode(300 * TIMECODE_SCALE, TIMECODE_SCALE); // the first timecode here
	
		DataBuffer *data2 = new DataBuffer((binary *)"tttyyy", countof("tttyyy"));
		Clust2.AddFrame(MyTrack1, 350 * TIMECODE_SCALE, *data2, MyNewBlock, *MyLastBlockTrk1);
		
		AllCues.AddBlockGroup(*MyNewBlock);

		ClusterSize += Clust2.Render(out_file, AllCues, bWriteDefaultValues);
		Clust2.ReleaseFrames();

// older version, write at the end		AllCues.Render(out_file);
		uint32 CueSize = AllCues.Render(out_file, bWriteDefaultValues);
		MetaSeek.IndexThis(AllCues, FileSegment);

		// \todo put it just before the Cue Entries
		uint32 MetaSeekSize = Dummy.ReplaceWith(MetaSeek, out_file, bWriteDefaultValues);

#ifdef VOID_TEST
		MyInfos.VoidMe(out_file);
#endif // VOID_TEST

		// let's assume we know the size of the Segment element
		// the size of the FileSegment is also computed because mandatory elements we don't write ourself exist
		if (FileSegment.ForceSize(SegmentSize - FileSegment.HeadSize() + MetaSeekSize + TrackSize + ClusterSize + CueSize + InfoSize)) {
			FileSegment.OverwriteHead(out_file);
		}

#if 0
		delete[] buf_bin;
		delete[] buf_txt;
#endif // 0

#ifdef OLD
		MuxedFile.Close(1000); // 1000 ms
#endif // OLD
		out_file.close();
    }
    catch (exception & Ex)
    {
		cout << Ex.what() << endl;
    }

    return 0;
}
