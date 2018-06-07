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
    \version \$Id: test8.cpp,v 1.26 2003/06/13 17:49:15 mosu Exp $
    \brief Test reading of the Header
    \author Steve Lhomme     <robux4 @ users.sf.net>
*/

#include <iostream>
#include <cassert>

#if __GNUC__ == 2
#include <wchar.h>
#endif

#include "EbmlHead.h"
#include "EbmlSubHead.h"
#include "EbmlStream.h"
#include "EbmlContexts.h"
#include "FileKax.h"
#include "KaxSegment.h"
#include "KaxContexts.h"
#include "KaxTracks.h"
#include "KaxInfo.h"
#include "KaxInfoData.h"
#include "KaxCluster.h"
#include "KaxClusterData.h"
#include "KaxBlockData.h"
#include "KaxSeekHead.h"
#include "KaxCuesData.h"
#include "StdIOCallback.h"

using namespace LIBMATROSKA_NAMESPACE;
using namespace std;

#define NO_DISPLAY_DATA
//#define JUMP_TEST

#ifdef OLD
void displayTrackInfo(const TrackInfo & aTrackInfo)
{
    cout << "Track name  : " << aTrackInfo.Name << endl;
    cout << "enabled     : " << (aTrackInfo.Enabled?"yes":"no") << endl;
    cout << "preferred   : " << (aTrackInfo.Preferred?"yes":"no") << endl << endl;
}

void displayAudioInfo(const TrackInfoAudio & aAudioTrack)
{
    cout << "sample rate : " << (int)aAudioTrack.SampleRate << endl;
    cout << "bit depth   : " << (int)aAudioTrack.BitDepth << endl;
    cout << "channel/tra : " << (int)aAudioTrack.ChannelPerSubtrack << endl;
    cout << "channel pos : " << (int)aAudioTrack.ChannelPosition << endl;
    cout << "DTS         : " << (aAudioTrack.DTS?"yes":"no") << endl;
    cout << "secondary   : " << (aAudioTrack.SecondaryTrack?"yes":"no") << endl;
    cout << "subtrack #  : " << (int)aAudioTrack.SubtrackNumber << endl;
    cout << "AVI format  : " << (int)aAudioTrack.AVIFormatTag << endl << endl;
}

void displayVideoInfo(const TrackInfoVideo & aVideoTrack)
{
    cout << "interlaced  : " << (aVideoTrack.Interlaced?"yes":"no") << endl;
    cout << "stereo/3D   : " << (aVideoTrack.Stereo3D?"yes":"no");
    if (aVideoTrack.Stereo3D)
	switch (aVideoTrack.Eyes) {
	case eye_right:
	    cout << " (right eye)";
	    break;
	case eye_left:
	    cout << " (left eye)";
	    break;
	case eye_both:
	    cout << " (both eyes)";
	    break;
	default:
	    cout << " (unknown eyes)";
	    break;
	}
    cout << endl;
    cout << "resolution  : " << int(aVideoTrack.xSize) << "x" << int(aVideoTrack.ySize);
    cout << " (aspect ratio : " << (float(aVideoTrack.AspectRatio) / 1000.0) << ")" << endl;
    cout << "FourCC      : " << char(aVideoTrack.FourCC[0]) << char(aVideoTrack.FourCC[1]) << char(aVideoTrack.FourCC[2]) << char(aVideoTrack.FourCC[3]) << endl;
}
#endif // OLD

/*!
    \note you can generate the file used in this example using test6.cpp
	\todo the blocks with "titi" and "tetetete" don't seem to be read !
*/
int main(int argc, char **argv)
{
    try {
	StdIOCallback Cluster_file("Float.mkv", MODE_READ);

	// create the Matroska file
///	FileKax MuxedFile(Cluster_file);
	bool bAllowDummy = false; // don't read elements we don't know

	// read the EBML head
	EbmlStream aStream(Cluster_file);
	EbmlElement * ElementLevel0;
	EbmlElement * ElementLevel1;
	EbmlElement * ElementLevel2;
	EbmlElement * ElementLevel3;
	EbmlElement * ElementLevel4;

///	MuxedFile.ReadHead();
	// find the EBML head in the file
	ElementLevel0 = aStream.FindNextID(EbmlHead::ClassInfos, 0xFFFFFFFFL);
	if (ElementLevel0 != NULL)
	{
		printf("EBML : ");
		for (unsigned int i=0; i<EbmlId(*ElementLevel0).Length; i++)
		{
			printf("[%02X]", (EbmlId(*ElementLevel0).Value >> (8*(3-i))) & 0xFF);
		}
		printf("\n");

		ElementLevel0->SkipData(aStream, EbmlHead_Context);
		if (ElementLevel0 != NULL)
			delete ElementLevel0;
	}

	int UpperElementLevel = 0;
	KaxSegment * Segment;
	KaxInfo * SegmentInfo;
	KaxTrackEntry * TrackAudio;
	KaxTrackEntry * TrackVideo;
	KaxCluster *SegmentCluster;
	KaxCues *CuesEntry;
	KaxSeekHead *MetaSeek;
	uint64 TimecodeScale = 1000000;

	// find the segment to read
	ElementLevel0 = aStream.FindNextID(KaxSegment::ClassInfos, 0xFFFFFFFFL);
	if (ElementLevel0 != NULL)
	{
		if (EbmlId(*ElementLevel0) == KaxSegment::ClassInfos.GlobalId) {
			Segment = static_cast<KaxSegment*>(ElementLevel0);
//			MuxedFile.ReadTracks();
//			MuxedFile.ReadCodec();
			// scan the file for a Tracks element (all previous Level1 elements are discarded)
			ElementLevel1 = aStream.FindNextElement(ElementLevel0->Generic().Context, UpperElementLevel, ElementLevel0->ElementSize(), bAllowDummy);

			while (ElementLevel1 != NULL) {
				if (UpperElementLevel != 0) {
					break;
				}

				/// \todo switch the type of the element to check if it's one we want to handle, like attachements
				if (EbmlId(*ElementLevel1) == KaxTracks::ClassInfos.GlobalId) {
					// found the Tracks element
					printf("\n- Segment Tracks found\n");
					// handle the data in Tracks here.
					// poll for new tracks and handle them
					ElementLevel2 = aStream.FindNextElement(ElementLevel1->Generic().Context, UpperElementLevel, ElementLevel1->ElementSize(), bAllowDummy);

					while (ElementLevel2 != NULL) {
						if (UpperElementLevel != 0) {
							break;
						}
						/// \todo switch the type of the element to check if it's one we want to handle, like attachements
						if (EbmlId(*ElementLevel2) == KaxTrackEntry::ClassInfos.GlobalId) {
							printf("* Found a track\n");

							ElementLevel3 = aStream.FindNextElement(ElementLevel2->Generic().Context, UpperElementLevel, ElementLevel2->ElementSize(), bAllowDummy);
							while (ElementLevel3 != NULL) {
								if (UpperElementLevel != 0) {
									break;
								}
								// read the data we care about in a track
								// Track number
								if (EbmlId(*ElementLevel3) == KaxTrackNumber::ClassInfos.GlobalId) {
									KaxTrackNumber & TrackNum = *static_cast<KaxTrackNumber*>(ElementLevel3);
									TrackNum.ReadData(aStream.I_O());
									printf("Track # %d\n", uint8(TrackNum));
								}

								// Track type
								else if (EbmlId(*ElementLevel3) == KaxTrackType::ClassInfos.GlobalId) {
									KaxTrackType & TrackType = *static_cast<KaxTrackType*>(ElementLevel3);
									TrackType.ReadData(aStream.I_O());
									printf("Track type : ");
									switch(uint8(TrackType))
									{
									case track_audio:
										printf("Audio");
										TrackAudio = static_cast<KaxTrackEntry *>(ElementLevel2);
										TrackAudio->SetGlobalTimecodeScale(TimecodeScale);
										break;
									case track_video:
										printf("Video");
										TrackVideo = static_cast<KaxTrackEntry *>(ElementLevel2);
										TrackVideo->SetGlobalTimecodeScale(TimecodeScale);
										break;
									default:
										printf("unknown");
									}
									printf("\n");
								}

								else if (EbmlId(*ElementLevel3) == KaxCodecID::ClassInfos.GlobalId) {
									KaxCodecID & CodecID = *static_cast<KaxCodecID*>(ElementLevel3);
									CodecID.ReadData(aStream.I_O());
									printf("Codec ID   : %s\n", string(CodecID).c_str());
								}

								if (UpperElementLevel > 0) {
									assert(0 == 1); // impossible to be here ?
									UpperElementLevel--;
									delete ElementLevel2;
									ElementLevel2 = ElementLevel3;
									if (UpperElementLevel > 0)
										break;
								} else {
									ElementLevel3->SkipData(aStream, ElementLevel3->Generic().Context);
									delete ElementLevel3;

									ElementLevel3 = aStream.FindNextElement(ElementLevel2->Generic().Context, UpperElementLevel, ElementLevel2->ElementSize(), bAllowDummy);
								}
							}
						}
						if (UpperElementLevel > 0) {
							UpperElementLevel--;
							delete ElementLevel2;
							ElementLevel2 = ElementLevel3;
							if (UpperElementLevel > 0)
								break;
						} else {
							ElementLevel2->SkipData(aStream, ElementLevel2->Generic().Context);
							delete ElementLevel2;
					
							ElementLevel2 = aStream.FindNextElement(ElementLevel1->Generic().Context, UpperElementLevel, ElementLevel1->ElementSize(), bAllowDummy);
						}
					}
				}

				else if (EbmlId(*ElementLevel1) == KaxInfo::ClassInfos.GlobalId) {
					printf("\n- Segment Informations found\n");
					SegmentInfo = static_cast<KaxInfo *>(ElementLevel1);

#ifdef JUMP_TEST
					// test jumping to a random location and find the next Level 0 / Level 1 element
					aStream.I_O().setFilePointer(79, seek_current);
					printf("Seeked at position 0x%X in the file", aStream.I_O().getFilePointer());
					int LowLevel = 1;
					ElementLevel2 = aStream.FindNextElement(KaxSegment::ClassInfos.Context, LowLevel, ElementLevel0->ElementSize(), false); // search up to one level lower
					UpperElementLevel = LowLevel;
#else // JUMP_TEST
					// read the data we care about in matroska
					/// \todo There should be a way to get the default values of the elements not defined
					ElementLevel2 = aStream.FindNextElement(ElementLevel1->Generic().Context, UpperElementLevel, ElementLevel1->ElementSize(), bAllowDummy);
					while (ElementLevel2 != NULL) {
						if (UpperElementLevel != 0) {
							break;
						}
						if (EbmlId(*ElementLevel2) == KaxTimecodeScale::ClassInfos.GlobalId) {
							KaxTimecodeScale *TimeScale = static_cast<KaxTimecodeScale*>(ElementLevel2);
							TimeScale->ReadData(aStream.I_O());
							printf("Timecode Scale %d\n", uint32(*TimeScale));
							TimecodeScale = uint64(*TimeScale);
						} else if (EbmlId(*ElementLevel2) == KaxDuration::ClassInfos.GlobalId) {
							printf("Segment duration\n");
						} else if (EbmlId(*ElementLevel2) == KaxDateUTC::ClassInfos.GlobalId) {
							printf("Date UTC\n");
						} else if (EbmlId(*ElementLevel2) == KaxTitle::ClassInfos.GlobalId) {
							printf("Title\n");
						} else if (EbmlId(*ElementLevel2) == KaxMuxingApp::ClassInfos.GlobalId) {
							KaxMuxingApp *pApp = static_cast<KaxMuxingApp*>(ElementLevel2);
							pApp->ReadData(aStream.I_O());
#if !defined(__CYGWIN__)
							wprintf(L"Muxing App : %ls\n", UTFstring(*pApp).c_str());
#else
							printf("Muxing App : %s\n", UTFstring(*pApp).c_str());
#endif
						} else if (EbmlId(*ElementLevel2) == KaxWritingApp::ClassInfos.GlobalId) {
							KaxWritingApp *pApp = static_cast<KaxWritingApp*>(ElementLevel2);
							pApp->ReadData(aStream.I_O());
#if !defined(__CYGWIN__)
							wprintf(L"Writing App : %ls \n", UTFstring(*pApp).c_str());
#else
							printf("Writing App : %s \n", UTFstring(*pApp).c_str());
#endif
						}

						if (UpperElementLevel > 0) {
							UpperElementLevel--;
							delete ElementLevel2;
							ElementLevel2 = ElementLevel3;
							if (UpperElementLevel > 0)
								break;
						} else {
							ElementLevel2->SkipData(aStream, ElementLevel2->Generic().Context);
							delete ElementLevel2;

							ElementLevel2 = aStream.FindNextElement(ElementLevel1->Generic().Context, UpperElementLevel, ElementLevel1->ElementSize(), bAllowDummy);
						}
					}
#endif // JUMP_TEST
				}

				else if (EbmlId(*ElementLevel1) == KaxCluster::ClassInfos.GlobalId) {
					printf("\n- Segment Clusters found\n");
					SegmentCluster = static_cast<KaxCluster *>(ElementLevel1);
					uint32 ClusterTimecode;

#ifdef MEMORY_READ // read the Cluster in memory and then extract elements from memory
					SegmentCluster->Read(aStream, KaxCluster::ClassInfos.Context, UpperElementLevel, ElementLevel2, bAllowDummy);
					if (SegmentCluster->CheckMandatory()) {
						printf("  All mandatory elements found\n");
					} else {
						printf("  Some mandatory elements ar missing !!!\n");
					}
					
					// display the elements read
					unsigned int Index0;
					for (Index0 = 0; Index0<SegmentCluster->ListSize() ;Index0++) {
						printf(" - found %s\n", (*SegmentCluster)[Index0]->Generic().DebugName);
					}
#else // not MEMORY_READ
					// read blocks and discard the ones we don't care about
					ElementLevel2 = aStream.FindNextElement(ElementLevel1->Generic().Context, UpperElementLevel, ElementLevel1->ElementSize(), bAllowDummy);
					while (ElementLevel2 != NULL) {
						if (UpperElementLevel != 0) {
							break;
						}
						if (EbmlId(*ElementLevel2) == KaxClusterTimecode::ClassInfos.GlobalId) {
							printf("Cluster timecode found\n");
							KaxClusterTimecode & ClusterTime = *static_cast<KaxClusterTimecode*>(ElementLevel2);
							ClusterTime.ReadData(aStream.I_O());
							ClusterTimecode = uint32(ClusterTime);
							SegmentCluster->InitTimecode(ClusterTimecode, TimecodeScale);
						} else  if (EbmlId(*ElementLevel2) == KaxBlockGroup::ClassInfos.GlobalId) {
							printf("Block Group found\n");
							KaxBlockGroup & aBlockGroup = *static_cast<KaxBlockGroup*>(ElementLevel2);
							// Extract the valuable data from the Block

#ifdef TEST_BLOCKGROUP_READ
							aBlockGroup.Read(aStream, KaxBlockGroup::ClassInfos.Context, UpperElementLevel, ElementLevel3, bAllowDummy);
							KaxBlock * DataBlock = static_cast<KaxBlock *>(aBlockGroup.FindElt(KaxBlock::ClassInfos));
							if (DataBlock != NULL) {
//								DataBlock->ReadData(aStream.I_O());
								DataBlock->SetParent(*SegmentCluster);
								printf("   Track # %d / %d frame%s / Timecode %I64d\n",DataBlock->TrackNum(), DataBlock->NumberFrames(), (DataBlock->NumberFrames() > 1)?"s":"", DataBlock->GlobalTimecode());
							} else {
								printf("   A BlockGroup without a Block !!!");
							}
							KaxBlockDuration * BlockDuration = static_cast<KaxBlockDuration *>(aBlockGroup.FindElt(KaxBlockDuration::ClassInfos));
							if (BlockDuration != NULL) {
								printf("  Block Duration %d scaled ticks : %ld ns\n", uint32(*BlockDuration), uint32(*BlockDuration) * TimecodeScale);
							}
							KaxReferenceBlock * RefTime = static_cast<KaxReferenceBlock *>(aBlockGroup.FindElt(KaxReferenceBlock::ClassInfos));
							if (RefTime != NULL) {
								printf("  Reference frame at scaled (%d) timecode %ld\n", int32(*RefTime), int32(int64(*RefTime) * TimecodeScale));
							}
#else // TEST_BLOCKGROUP_READ
							// read the data we care about in matroska
							/// \todo There should be a way to get the default values of the elements not defined
							ElementLevel3 = aStream.FindNextElement(ElementLevel2->Generic().Context, UpperElementLevel, ElementLevel2->ElementSize(), bAllowDummy);
							while (ElementLevel3 != NULL) {
								if (UpperElementLevel != 0) {
									break;
								}
								if (EbmlId(*ElementLevel3) == KaxBlock::ClassInfos.GlobalId) {
									printf(" Block Data\n");
									KaxBlock & DataBlock = *static_cast<KaxBlock*>(ElementLevel3);
									DataBlock.ReadData(aStream.I_O());
									DataBlock.SetParent(*SegmentCluster);
									printf("   Track # %d / %d frame%s / Timecode %I64d\n",DataBlock.TrackNum(), DataBlock.NumberFrames(), (DataBlock.NumberFrames() > 1)?"s":"", DataBlock.GlobalTimecode());
#ifndef NO_DISPLAY_DATA
									for (unsigned int i=0; i< DataBlock.NumberFrames(); i++) {
										printf("   [%s]\n",DataBlock.GetBuffer(i).Buffer()); // STRING ONLY POSSIBLE WITH THIS PARTICULAR EXAMPLE (the binary data is a string)
									}
#endif // NO_DISPLAY_DATA
//									printf("Codec ID   : %s\n", &binary(CodecID)); // strings for the moment (example)
								} else if (EbmlId(*ElementLevel3) == KaxBlockVirtual::ClassInfos.GlobalId) {
									printf(" Virtual Block\n");
								} else if (EbmlId(*ElementLevel3) == KaxReferencePriority::ClassInfos.GlobalId) {
									printf("  Reference priority\n");
								} else if (EbmlId(*ElementLevel3) == KaxReferenceBlock::ClassInfos.GlobalId) {
									KaxReferenceBlock & RefTime = *static_cast<KaxReferenceBlock*>(ElementLevel3);
									RefTime.ReadData(aStream.I_O());
									printf("  Reference frame at scaled (%d) timecode %ld\n", int32(RefTime), int32(int64(RefTime) * TimecodeScale));
								} else if (EbmlId(*ElementLevel3) == KaxReferenceVirtual::ClassInfos.GlobalId) {
									printf("  virtual Reference\n");
								} else if (EbmlId(*ElementLevel3) == KaxBlockDuration::ClassInfos.GlobalId) {
									KaxBlockDuration & BlockDuration = *static_cast<KaxBlockDuration*>(ElementLevel3);
									BlockDuration.ReadData(aStream.I_O());
									printf("  Block Duration %d scaled ticks : %ld ns\n", uint32(BlockDuration), uint32(BlockDuration) * TimecodeScale);
								}
								if (UpperElementLevel > 0) {
									UpperElementLevel--;
									delete ElementLevel3;
									ElementLevel3 = ElementLevel4;
									if (UpperElementLevel > 0)
										break;
								} else {
									ElementLevel3->SkipData(aStream, ElementLevel3->Generic().Context);
									delete ElementLevel3;

									ElementLevel3 = aStream.FindNextElement(ElementLevel2->Generic().Context, UpperElementLevel, ElementLevel2->ElementSize(), bAllowDummy);
								}
							}
#endif // TEST_BLOCKGROUP_READ
						}
						if (UpperElementLevel > 0) {
							UpperElementLevel--;
							delete ElementLevel2;
							ElementLevel2 = ElementLevel3;
							if (UpperElementLevel > 0)
								break;
						} else {
							ElementLevel2->SkipData(aStream, ElementLevel2->Generic().Context);
							delete ElementLevel2;

							ElementLevel2 = aStream.FindNextElement(ElementLevel1->Generic().Context, UpperElementLevel, ElementLevel1->ElementSize(), bAllowDummy);
						}
					}
#endif // not MEMORY_READ
				}
				else if (EbmlId(*ElementLevel1) == KaxCues::ClassInfos.GlobalId) {
					printf("\n- Cue entries found\n");
					CuesEntry = static_cast<KaxCues *>(ElementLevel1);
					CuesEntry->SetGlobalTimecodeScale(TimecodeScale);
					// read everything in memory
					CuesEntry->Read(aStream, KaxCues::ClassInfos.Context, UpperElementLevel, ElementLevel2, bAllowDummy); // build the entries in memory
					if (CuesEntry->CheckMandatory()) {
						printf("  All mandatory elements found\n");
					} else {
						printf("  Some mandatory elements ar missing !!!\n");
					}
					CuesEntry->Sort();
					// display the elements read
					unsigned int Index0;
					for (Index0 = 0; Index0<CuesEntry->ListSize() ;Index0++) {
						if ((*CuesEntry)[Index0]->Generic().GlobalId == KaxCuePoint::ClassInfos.GlobalId) {
							printf(" Cue Point\n");

							KaxCuePoint & CuePoint = *static_cast<KaxCuePoint *>((*CuesEntry)[Index0]);
							unsigned int Index1;
							for (Index1 = 0; Index1<CuePoint.ListSize() ;Index1++) {
								if (CuePoint[Index1]->Generic().GlobalId == KaxCueTime::ClassInfos.GlobalId) {
									KaxCueTime & CueTime = *static_cast<KaxCueTime *>(CuePoint[Index1]);
									printf("  Time %ld\n", uint64(CueTime) * TimecodeScale);
								} else if (CuePoint[Index1]->Generic().GlobalId == KaxCueTrackPositions::ClassInfos.GlobalId) {
									KaxCueTrackPositions & CuePos = *static_cast<KaxCueTrackPositions *>(CuePoint[Index1]);
									printf("  Positions\n");

									unsigned int Index2;
									for (Index2 = 0; Index2<CuePos.ListSize() ;Index2++) {
										if (CuePos[Index2]->Generic().GlobalId == KaxCueTrack::ClassInfos.GlobalId) {
											KaxCueTrack & CueTrack = *static_cast<KaxCueTrack *>(CuePos[Index2]);
											printf("   Track %d\n", uint16(CueTrack));
										} else if (CuePos[Index2]->Generic().GlobalId == KaxCueClusterPosition::ClassInfos.GlobalId) {
											KaxCueClusterPosition & CuePoss = *static_cast<KaxCueClusterPosition *>(CuePos[Index2]);
											printf("   Cluster position %d\n", uint64(CuePoss));
										} else if (CuePos[Index2]->Generic().GlobalId == KaxCueReference::ClassInfos.GlobalId) {
											KaxCueReference & CueRefs = *static_cast<KaxCueReference *>(CuePos[Index2]);
											printf("   Reference\n");

											unsigned int Index3;
											for (Index3 = 0; Index3<CueRefs.ListSize() ;Index3++) {
												if (CueRefs[Index3]->Generic().GlobalId == KaxCueRefTime::ClassInfos.GlobalId) {
													KaxCueRefTime & CueTime = *static_cast<KaxCueRefTime *>(CueRefs[Index3]);
													printf("    Time %d\n", uint32(CueTime));
												} else if (CueRefs[Index3]->Generic().GlobalId == KaxCueRefCluster::ClassInfos.GlobalId) {
													KaxCueRefCluster & CueClust = *static_cast<KaxCueRefCluster *>(CueRefs[Index3]);
													printf("    Cluster position %d\n", uint64(CueClust));
												} else {
													printf("    - found %s\n", CueRefs[Index3]->Generic().DebugName);
												}
											}
										} else {
											printf("   - found %s\n", CuePos[Index2]->Generic().DebugName);
										}
									}
								} else {
									printf("  - found %s\n", CuePoint[Index1]->Generic().DebugName);
								}
							}
						} else {
							printf(" - found %s\n", (*CuesEntry)[Index0]->Generic().DebugName);
						}
					}
				}
				else if (EbmlId(*ElementLevel1) == KaxSeekHead::ClassInfos.GlobalId) {
					printf("\n- Meta Seek found\n");
					MetaSeek = static_cast<KaxSeekHead *>(ElementLevel1);
					// read it in memory
					MetaSeek->Read(aStream, KaxSeekHead::ClassInfos.Context, UpperElementLevel, ElementLevel2, bAllowDummy);
					if (MetaSeek->CheckMandatory()) {
						printf("  All mandatory elements found\n");
					} else {
						printf("  Some mandatory elements ar missing !!!\n");
					}
					unsigned int Index0;
					for (Index0 = 0; Index0<MetaSeek->ListSize() ;Index0++) {
						if ((*MetaSeek)[Index0]->Generic().GlobalId == KaxSeek::ClassInfos.GlobalId) {
							printf("   Seek Point\n");
							KaxSeek & SeekPoint = *static_cast<KaxSeek *>((*MetaSeek)[Index0]);
							unsigned int Index1;
							for (Index1 = 0; Index1<SeekPoint.ListSize() ;Index1++) {
								if (SeekPoint[Index1]->Generic().GlobalId == KaxSeekID::ClassInfos.GlobalId) {
									KaxSeekID * SeekID = static_cast<KaxSeekID *>(SeekPoint[Index1]);
									printf("    Seek ID ", SeekID->GetBuffer());
									for (int i=0; i<SeekID->GetSize(); i++) {
										printf("%02X", SeekID->GetBuffer()[i]);
									}
									printf("\n");
								} else if (SeekPoint[Index1]->Generic().GlobalId == KaxSeekPosition::ClassInfos.GlobalId) {
									KaxSeekPosition * SeekPos = static_cast<KaxSeekPosition *>(SeekPoint[Index1]);
									printf("    Seek position %d\n", uint32(*SeekPos));
								}
							}
						}
					}
				}

				if (UpperElementLevel > 0) {
					UpperElementLevel--;
					delete ElementLevel1;
					ElementLevel1 = ElementLevel2;
					if (UpperElementLevel > 0)
						break;
				} else {
					ElementLevel1->SkipData(aStream, ElementLevel1->Generic().Context);
					delete ElementLevel1;
			
					ElementLevel1 = aStream.FindNextElement(ElementLevel0->Generic().Context, UpperElementLevel, ElementLevel0->ElementSize(), bAllowDummy);
				}
			}
		}
	}

#ifdef OLD
	uint8 TrackNumber = MuxedFile.GetTrackNumber();

	TrackInfo *Tracks = new TrackInfo[TrackNumber];
	TrackInfoAudio aAudioTrack;
	TrackInfoVideo aVideoTrack;

	Track * track1 = MuxedFile.GetTrack(1);
	Track * track2 = MuxedFile.GetTrack(2);
	Track * track3 = MuxedFile.GetTrack(3); // should be 0
	// get information about the 1st track

	MuxedFile.Track_GetInfo(track1, Tracks[0]);
	displayTrackInfo(Tracks[0]);

	if (Tracks[0].TrackType == track_audio) {
	    MuxedFile.Track_GetInfo_Audio(track1, aAudioTrack);
	    displayAudioInfo(aAudioTrack);
	}
	else
	    cout << "the expected audio track is not an audio one :(" << endl;

	MuxedFile.Track_GetInfo(track3, Tracks[2]);
	displayTrackInfo(Tracks[2]);

	if (Tracks[2].TrackType == track_video) {
	    MuxedFile.Track_GetInfo_Video(track3, aVideoTrack);
	    displayVideoInfo(aVideoTrack);
	}
	else
	    cout << "the expected video track is not an video one :(" << endl;

	// let's read only track1 (audio)
	MuxedFile.SelectReadingTrack(track1);
	MuxedFile.SelectReadingTrack(track2);
	MuxedFile.SelectReadingTrack(track1,false);
	MuxedFile.SelectReadingTrack(track1);
	MuxedFile.SelectReadingTrack(track2,false);
	MuxedFile.SelectReadingTrack(track2);
	MuxedFile.SelectReadingTrack(track3);

	// read the frames from the selected tracks and put them in an output file
	StdIOCallback Output_file1("out-binr.bin", MODE_CREATE);
	StdIOCallback Output_file2("out-text.bin", MODE_CREATE);
	StdIOCallback Output_file3("out-vide.bin", MODE_CREATE);

	Track * TrackRead;
	uint32 timecode; // not used yet
	binary *aFrame;
	uint32 aFrameSize;
	while (MuxedFile.ReadFrame(TrackRead, timecode, aFrame, aFrameSize))
	{
	    if (TrackRead == track1)
	    {
		Output_file1.write(aFrame, aFrameSize);
	    }
	    else if (TrackRead == track2)
	    {
		Output_file2.write(aFrame, aFrameSize);
	    }
	    else if (TrackRead == track3)
	    {
		Output_file3.write(aFrame, aFrameSize);
	    }
	    else cout << "received a frame from an unwanted track" << endl;
	}
#endif // OLD
    }
    catch (exception & Ex)
    {
		cout << Ex.what() << endl;
		return -1;
    }

    return 0;
}
