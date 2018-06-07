// MatroskaReader.cpp: implementation of the MatroskaReader class.
//
//////////////////////////////////////////////////////////////////////

#include "MatroskaReader.h"
#include <dshow.h>
#include <wxdebug.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MatroskaReader::MatroskaReader() : 
bReadDummyElements(true),
m_pStream(NULL),
m_pKaxIO(NULL),
m_TrackCount(0),
m_qwDuration(0),
m_TimeScale(10000.0)
{
}

// ----------------------------------------------------------------------------

MatroskaReader::~MatroskaReader()
{
	KaxTrackInfoStruct* pTrackInfo;
	for(int i=0; i<m_TrackCount; i++)
	{
		pTrackInfo = m_Tracks[i];
		if (pTrackInfo->CodecID != NULL) 
			free(pTrackInfo->CodecID);
		if (pTrackInfo->CodecPrivate != NULL) 
			free(pTrackInfo->CodecPrivate);
		if (pTrackInfo->Type == track_video)
			delete pTrackInfo->pVideoInfo;
		if (pTrackInfo->Type == track_audio) 
			delete pTrackInfo->pAudioInfo;
	}
	
	if (m_pKaxIO != NULL)
		m_pKaxIO->close();
}

// ----------------------------------------------------------------------------

int MatroskaReader::InitKaxFile(IOCallback *pIOCb) 
{
	bool	bTracksLoaded = false;
	int		UpperElementLevel = 0;
	
    NOTE("CKaxDemuxFilter::InitKaxFile");
	
	// initialize Element array
	for (int i=0; i<6; i++)
		m_pElems[i] = NULL;
	
	// initialize EBML Stream
	if (pIOCb == NULL)
		return -1;
	
	if (m_pStream != NULL)
		return -2;	// for now we dont support this
	
	if ((m_pStream = new EbmlStream(*pIOCb)) == NULL)
		return -3;
	
	m_pKaxIO = pIOCb;
	
	// is this a valid EBML file ?
	if ((m_pElems[0] = m_pStream->FindNextID(EbmlHead::ClassInfos, 0xFFFFFFFFL)) != NULL)
	{
		// Make sure we have the expected element		
		if (!(EbmlId(*m_pElems[0]) == EbmlHead::ClassInfos.GlobalId))
		{
			return -1;
		}
		
		m_pElems[1] = m_pStream->FindNextElement(m_pElems[0]->Generic().Context, UpperElementLevel, 0xFFFFFFFFL, bReadDummyElements);
		while (m_pElems[1] != NULL) 
		{
			if (UpperElementLevel != 0)
				break;
			
			if (EbmlId(*m_pElems[1]) == EDocType::ClassInfos.GlobalId)
			{
				EDocType & DocType = *static_cast<EDocType*>(m_pElems[1]);
				DocType.ReadData(m_pStream->I_O());
				if (std::string(DocType) != "matroska")
					return -5;
				break; // we are finished with the EbmlHead for now
			}

			if (UpperElementLevel > 0) 
			{
				UpperElementLevel--;
				delete m_pElems[1];
				m_pElems[1] = m_pElems[2];
				if (UpperElementLevel > 0)
					break;
			}
			else
			{
				m_pElems[1]->SkipData(*m_pStream, m_pElems[1]->Generic().Context);
				delete m_pElems[1];
				
				m_pElems[1] = m_pStream->FindNextElement(m_pElems[0]->Generic().Context, UpperElementLevel, 0xFFFFFFFFL, bReadDummyElements);
			}
		}
		
		m_pElems[0]->SkipData(*m_pStream, EbmlHead_Context);
		if (m_pElems[0] != NULL)
		{
			delete m_pElems[0];
			m_pElems[0] = NULL;
		}
	}
	else
	{
		return -4;
	}
	
	// find a matroska segment
	if ((m_pElems[0] = m_pStream->FindNextID(KaxSegment::ClassInfos, 0xFFFFFFFFL)) != NULL)
		if (!(EbmlId(*m_pElems[0]) == KaxSegment::ClassInfos.GlobalId))
			return -5;
		
		// parse all sub-elements
		m_pElems[1] = m_pStream->FindNextElement(m_pElems[0]->Generic().Context, UpperElementLevel, 0xFFFFFFFFL, bReadDummyElements);
		while (m_pElems[1] != NULL) 
		{
			if (UpperElementLevel != 0)
				break;
			
			if (EbmlId(*m_pElems[1]) == KaxTracks::ClassInfos.GlobalId)
			{
				// another funky level ;-)
				m_pElems[2] = m_pStream->FindNextElement(m_pElems[1]->Generic().Context, UpperElementLevel, 0xFFFFFFFFL, bReadDummyElements);
				while (m_pElems[2] != NULL) 
				{
					if (UpperElementLevel != 0)
						break;
					if (EbmlId(*m_pElems[2]) == KaxTrackEntry::ClassInfos.GlobalId)
					{					
						// and fill it with those values					
						KaxTrackInfoStruct *TrackInfo = new KaxTrackInfoStruct;
						memset(TrackInfo, 0, sizeof(KaxTrackInfoStruct));						
						InitTrack(m_pElems, UpperElementLevel, TrackInfo);
						m_Tracks[m_TrackCount] = TrackInfo;
						m_TrackCount++;
					}
					
					if (UpperElementLevel > 0) 
					{
						UpperElementLevel--;
						delete m_pElems[2];
						m_pElems[2] = m_pElems[3];
						if (UpperElementLevel > 0)
							break;
					}
					else
					{
						m_pElems[2]->SkipData(*m_pStream, m_pElems[2]->Generic().Context);
						delete m_pElems[2];
						
						m_pElems[2] = m_pStream->FindNextElement(m_pElems[1]->Generic().Context, UpperElementLevel, 0xFFFFFFFFL, bReadDummyElements);
					}
				}
				
				bTracksLoaded = true;
			}
			else if (EbmlId(*m_pElems[1]) == KaxCluster::ClassInfos.GlobalId) 
			{
				// store position
				m_qwKaxSegStart = m_pKaxIO->getFilePointer();
				
				m_pTmpElems[0] = m_pElems[0];
				m_pTmpElems[1] = m_pElems[1];
				
				// and exit if we already parsed the TrackEntry
				m_iLevel = 1;
				m_iTmpUpElLev = m_iUpElLev = UpperElementLevel;
				
				if (bTracksLoaded)
					return 1;
			}
			else if (EbmlId(*m_pElems[1]) == KaxInfo::ClassInfos.GlobalId) 
			{
				m_pElems[2] = m_pStream->FindNextElement(m_pElems[1]->Generic().Context, UpperElementLevel, 0xFFFFFFFFL, bReadDummyElements);
				while (m_pElems[2] != NULL)
				{
					if (UpperElementLevel != 0)
						break;
					if (EbmlId(*m_pElems[2]) == KaxTimecodeScale::ClassInfos.GlobalId) 
					{
						KaxTimecodeScale & TimecodeScale = *static_cast<KaxTimecodeScale*>(m_pElems[2]);
						TimecodeScale.ReadData(*m_pKaxIO);
						uint32 timescale = TimecodeScale;
						m_TimeScale = (double)timescale;
						m_TimeScale /= 100.0f;	// multiplicator (convert 1ns -> 100ns)
					}
					else if (EbmlId(*m_pElems[2]) == KaxDuration::ClassInfos.GlobalId) 
					{
						KaxDuration & Duration = *static_cast<KaxDuration*>(m_pElems[2]);
						Duration.ReadData(*m_pKaxIO);
						m_qwDuration = ((float)Duration) * m_TimeScale;
					}
					
					if (UpperElementLevel > 0) 
					{
						UpperElementLevel--;
						delete m_pElems[2];
						m_pElems[2] = m_pElems[3];
						if (UpperElementLevel > 0)
							break;
					}
					else 
					{
						m_pElems[2]->SkipData(*m_pStream, m_pElems[2]->Generic().Context);
						delete m_pElems[2];
						
						m_pElems[2] = m_pStream->FindNextElement(m_pElems[1]->Generic().Context, UpperElementLevel, 0xFFFFFFFFL, bReadDummyElements);
					}
				}
			}
			
			if (UpperElementLevel > 0) 
			{
				UpperElementLevel--;
				delete m_pElems[1];
				m_pElems[1] = m_pElems[2];
				if (UpperElementLevel > 0)
					break;
			}
			else
			{
				m_pElems[1]->SkipData(*m_pStream, m_pElems[1]->Generic().Context);
				delete m_pElems[1];
				
				m_pElems[1] = m_pStream->FindNextElement(m_pElems[0]->Generic().Context, UpperElementLevel, 0xFFFFFFFFL, bReadDummyElements);
			}
	}
	
	return 0;
}

// ----------------------------------------------------------------------------

// parsing KaxTrackEntry SubElements
int MatroskaReader::InitTrack(EbmlElement **elems, int &UEL, KaxTrackInfoStruct *pTrackInfos)
{
	EbmlElement			*plev3, *plev4, *plev5;
	int					UpElLev = UEL;
	
    NOTE("CKaxDemuxFilter::InitTrack");
	
	plev3 = plev4 = plev5 = NULL;
	
	// PrE - computer video player profile
	while (1)
	{
		if ((plev3 = m_pStream->FindNextElement(elems[2]->Generic().Context, UpElLev, 0xFFFFFFFFL, bReadDummyElements)) == NULL) 
			break;
		
		if (UpElLev != 0) 
			break;
		
		if (EbmlId(*plev3) == KaxTrackNumber::ClassInfos.GlobalId)
		{
			KaxTrackNumber &TrackNum = *static_cast<KaxTrackNumber*>(plev3);
			TrackNum.ReadData(*m_pKaxIO);
			pTrackInfos->Number = TrackNum;
		}
		else if (EbmlId(*plev3) == KaxTrackType::ClassInfos.GlobalId)
		{
			KaxTrackType &TrackType = *static_cast<KaxTrackType*>(plev3);
			TrackType.ReadData(*m_pKaxIO);
			pTrackInfos->Type = TrackType;
		}
		else if (EbmlId(*plev3) == KaxTrackFlagEnabled::ClassInfos.GlobalId)
		{
			KaxTrackFlagEnabled &FlagEnabled = *static_cast<KaxTrackFlagEnabled*>(plev3);
			FlagEnabled.ReadData(*m_pKaxIO);
			pTrackInfos->Enabled = FlagEnabled;
		}
		else if (EbmlId(*plev3) == KaxTrackFlagDefault::ClassInfos.GlobalId) 
		{
			KaxTrackFlagDefault &FlagDefault = *static_cast<KaxTrackFlagDefault*>(plev3);
			FlagDefault.ReadData(*m_pKaxIO);
			pTrackInfos->Default = FlagDefault;
		}
		else if (EbmlId(*plev3) == KaxTrackFlagLacing::ClassInfos.GlobalId)
		{
			KaxTrackFlagLacing &FlagLacing= *static_cast<KaxTrackFlagLacing*>(plev3);
			FlagLacing.ReadData(*m_pKaxIO);
			pTrackInfos->Lacing = FlagLacing;
		}
		else if (EbmlId(*plev3) == KaxTrackMinCache::ClassInfos.GlobalId) 
		{
			KaxTrackMinCache &MinCache = *static_cast<KaxTrackMinCache*>(plev3);
			MinCache.ReadData(*m_pKaxIO);
			pTrackInfos->MinCache = MinCache;
		}
		else if (EbmlId(*plev3) == KaxTrackMaxCache::ClassInfos.GlobalId) 
		{
			KaxTrackMaxCache &MaxCache = *static_cast<KaxTrackMaxCache*>(plev3);
			MaxCache.ReadData(*m_pKaxIO);
			pTrackInfos->MaxCache = MaxCache;
		}
		else if (EbmlId(*plev3) == KaxTrackName::ClassInfos.GlobalId) 
		{
			KaxTrackName &Name = *static_cast<KaxTrackName*>(plev3);
			Name.ReadData(*m_pKaxIO);
			pTrackInfos->Name = Name;
		}
		else if (EbmlId(*plev3) == KaxTrackLanguage::ClassInfos.GlobalId) 
		{
			KaxTrackLanguage &Language = *static_cast<KaxTrackLanguage*>(plev3);
			Language.ReadData(*m_pKaxIO);
			pTrackInfos->Language = Language;
		}
		else if (EbmlId(*plev3) == KaxCodecID::ClassInfos.GlobalId) 
		{
			KaxCodecID &CodecID = *static_cast<KaxCodecID*>(plev3);
			CodecID.ReadData(*m_pKaxIO);
			pTrackInfos->CodecID = (binary*)malloc(CodecID.GetSize() * sizeof(binary));
			memcpy(pTrackInfos->CodecID, CodecID.GetBuffer(), CodecID.GetSize());
		}
		else if (EbmlId(*plev3) == KaxCodecPrivate::ClassInfos.GlobalId)
		{
			KaxCodecPrivate &CodecPrivate = *static_cast<KaxCodecPrivate*>(plev3);
			CodecPrivate.ReadData(*m_pKaxIO);
			pTrackInfos->CodecPrivateLen = CodecPrivate.GetSize();
			pTrackInfos->CodecPrivate = (binary*)malloc(CodecPrivate.GetSize() * sizeof(binary));
			memcpy(pTrackInfos->CodecPrivate, CodecPrivate.GetBuffer(), CodecPrivate.GetSize());			
		}
		else if (EbmlId(*plev3) == KaxCodecName::ClassInfos.GlobalId) 
		{
			KaxCodecName &CodecName = *static_cast<KaxCodecName*>(plev3);
			CodecName.ReadData(*m_pKaxIO);
			pTrackInfos->CodecName = CodecName;
		}
		else if (EbmlId(*plev3) == KaxCodecDecodeAll::ClassInfos.GlobalId)
		{
			KaxCodecDecodeAll &CodecDecodeAll = *static_cast<KaxCodecDecodeAll*>(plev3);
			CodecDecodeAll.ReadData(*m_pKaxIO);
			pTrackInfos->CodecDecodeAll = CodecDecodeAll;
		}
		else if (EbmlId(*plev3) == KaxTrackOverlay::ClassInfos.GlobalId) 
		{
			KaxTrackOverlay &Overlay = *static_cast<KaxTrackOverlay*>(plev3);
			Overlay.ReadData(*m_pKaxIO);
			pTrackInfos->Overlay = Overlay;
		}
		else if (EbmlId(*plev3) == KaxTrackVideo::ClassInfos.GlobalId) 
		{
			pTrackInfos->pVideoInfo = new KaxVideoTrackInfoStruct;
			memset(pTrackInfos->pVideoInfo, 0, sizeof(KaxVideoTrackInfoStruct));
			
			while (1) 
			{
				if ((plev4 = m_pStream->FindNextElement(plev3->Generic().Context, UpElLev, 0xFFFFFFFFL, bReadDummyElements)) == NULL) 
					break;
				
				if (UpElLev != 0)
					break;
				
				if (EbmlId(*plev4) == KaxVideoFlagInterlaced::ClassInfos.GlobalId) 
				{
					KaxVideoFlagInterlaced &Interlaced = *static_cast<KaxVideoFlagInterlaced*>(plev4);
					Interlaced.ReadData(*m_pKaxIO);
					pTrackInfos->pVideoInfo->Interlaced = Interlaced;
				}
				else if (EbmlId(*plev4) == KaxVideoStereoMode::ClassInfos.GlobalId)
				{
					KaxVideoStereoMode &StereoMode = *static_cast<KaxVideoStereoMode*>(plev4);
					StereoMode.ReadData(*m_pKaxIO);
					pTrackInfos->pVideoInfo->StereoMode = StereoMode;
				}
				else if (EbmlId(*plev4) == KaxVideoPixelWidth::ClassInfos.GlobalId)
				{
					KaxVideoPixelWidth &PixelWidth = *static_cast<KaxVideoPixelWidth*>(plev4);
					PixelWidth.ReadData(*m_pKaxIO);
					pTrackInfos->pVideoInfo->PixelWidth = PixelWidth;
				}
				else if (EbmlId(*plev4) == KaxVideoPixelHeight::ClassInfos.GlobalId)
				{
					KaxVideoPixelHeight &PixelHeight = *static_cast<KaxVideoPixelHeight*>(plev4);
					PixelHeight.ReadData(*m_pKaxIO);
					pTrackInfos->pVideoInfo->PixelHeight = PixelHeight;
				}
				
				if (UpElLev > 0) 
				{
					UpElLev--;
					delete plev4;
					plev4 = plev5;
					if (UpElLev > 0) 
						break;
				}
				else {
					plev4->SkipData(*m_pStream, plev4->Generic().Context);
					delete plev4;
				}
			}
		}
		else if (EbmlId(*plev3) == KaxTrackAudio::ClassInfos.GlobalId)
		{
			pTrackInfos->pAudioInfo = new KaxAudioTrackInfoStruct;
			memset(pTrackInfos->pAudioInfo, 0, sizeof(KaxAudioTrackInfoStruct));
			
			while (1)
			{
				if ((plev4 = m_pStream->FindNextElement(plev3->Generic().Context, UpElLev, 0xFFFFFFFFL, bReadDummyElements)) == NULL) break;
				if (UpElLev != 0) break;
				
				if (EbmlId(*plev4) == KaxAudioSamplingFreq::ClassInfos.GlobalId) 
				{
					KaxAudioSamplingFreq &SampFreq = *static_cast<KaxAudioSamplingFreq*>(plev4);
					SampFreq.ReadData(*m_pKaxIO);
					pTrackInfos->pAudioInfo->SamplingFreq = SampFreq;
				}
				else if (EbmlId(*plev4) == KaxAudioChannels::ClassInfos.GlobalId)
				{
					KaxAudioChannels &Channels = *static_cast<KaxAudioChannels*>(plev4);
					Channels.ReadData(*m_pKaxIO);
					pTrackInfos->pAudioInfo->Channels = Channels;
				}
				else if (EbmlId(*plev4) == KaxAudioPosition::ClassInfos.GlobalId)
				{
					KaxAudioPosition &ChanPos = *static_cast<KaxAudioPosition*>(plev4);
					ChanPos.ReadData(*m_pKaxIO);
					pTrackInfos->pAudioInfo->Position = ChanPos.GetBuffer();
				}
				else if (EbmlId(*plev4) == KaxAudioBitDepth::ClassInfos.GlobalId) 
				{
					KaxAudioBitDepth &BitDepth = *static_cast<KaxAudioBitDepth*>(plev4);
					BitDepth.ReadData(*m_pKaxIO);
					pTrackInfos->pAudioInfo->BitDepth = BitDepth;
					}/*
					 else if (EbmlId(*plev4) == KaxAudioSubTrack::ClassInfos.GlobalId) {
					 
					}*/
				
				if (UpElLev > 0) 
				{
					UpElLev--;
					delete plev4;
					plev4 = plev5;
					if (UpElLev > 0)
						break;
				}
				else {
					plev4->SkipData(*m_pStream, plev4->Generic().Context);
					delete plev4;
				}
			}
		}
		
		if (UpElLev > 0) 
		{
			UpElLev--;
			delete plev3;
			plev3 = plev4;
			if (UpElLev > 0)
				break;
		}
		else
		{
			plev3->SkipData(*m_pStream, plev3->Generic().Context);
			delete plev3;
		}
	}
	
	elems[3] = plev3;
	elems[4] = plev4;
	elems[5] = plev5;
	
	UEL = UpElLev;
	return 0;
}

// ----------------------------------------------------------------------------

int MatroskaReader::GetTrackNumber()
{
	return m_TrackCount;
}

// ----------------------------------------------------------------------------

KaxTrackInfoStruct* MatroskaReader::GetTrackInfo(int i)
{
	return m_Tracks[i];
}

// ----------------------------------------------------------------------------

KaxBlock *MatroskaReader::GetNextBlock() 
{
	KaxCluster	*SegCluster;
	KaxBlock	*pBlock;
	
	switch (m_iLevel) 
	{
	case -1: goto bgroup;
	case 0: break;
	case 1: goto level1;
	case 2: goto level2;
	case 3: goto level3;
	default: return NULL;
	}
	
	m_iLevel = 0;
	if ((m_pElems[0] = m_pStream->FindNextID(KaxSegment::ClassInfos, 0xFFFFFFFFL)) == NULL)
		return NULL;
	
	while (1) 
	{
		if (m_iUpElLev != 0) break;
		
		// segment (level 0)
		if (EbmlId(*m_pElems[0]) == KaxSegment::ClassInfos.GlobalId)
		{
			m_iLevel = 1;
			
			if ((m_pElems[1] = m_pStream->FindNextElement(m_pElems[0]->Generic().Context, m_iUpElLev, 0xFFFFFFFFL, bReadDummyElements)) == NULL)
				break;

			while (1)
			{
				if (m_iUpElLev != 0)
					break;
level1:
				// cluster (level 1)
				if (EbmlId(*m_pElems[1]) == KaxCluster::ClassInfos.GlobalId) 
				{
					SegCluster = static_cast<KaxCluster *>(m_pElems[1]);
					
					m_iLevel = 2;
					if ((m_pElems[2] = m_pStream->FindNextElement(m_pElems[1]->Generic().Context, m_iUpElLev, 0xFFFFFFFFL, bReadDummyElements)) == NULL)
						break;

					while (1)
					{
						if (m_iUpElLev != 0) break;
level2:
						if (EbmlId(*m_pElems[2]) == KaxClusterTimecode::ClassInfos.GlobalId)
						{
							KaxClusterTimecode &ClusterTime = *static_cast<KaxClusterTimecode*>(m_pElems[2]);
							ClusterTime.ReadData(*m_pKaxIO);
							SegCluster->InitTimecode(uint32(ClusterTime));
						}
						else if (EbmlId(*m_pElems[2]) == KaxBlockGroup::ClassInfos.GlobalId)
						{							
							m_iLevel = 3;
							if ((m_pElems[3] = m_pStream->FindNextElement(m_pElems[2]->Generic().Context, m_iUpElLev, 0xFFFFFFFFL, bReadDummyElements)) == NULL) 
								break;

							while (1) 
							{
								if (m_iUpElLev != 0) 
									break;
level3:
								// for now we're only interested in BLOX
								if (EbmlId(*m_pElems[3]) == KaxBlock::ClassInfos.GlobalId) 
								{
									pBlock = static_cast<KaxBlock*>(m_pElems[3]);
									pBlock->SetParent(*((KaxCluster*)m_pElems[1]));
									pBlock->ReadData(*m_pKaxIO);
									m_iLevel = -1;	// means we're in blockgroup
									return pBlock;
								}
								else if (EbmlId(*m_pElems[3]) == KaxBlockDuration::ClassInfos.GlobalId)
								{
									// TODO : we need the BlockDuration for subtitles
									KaxBlockDuration &kaxBlockDuration = *static_cast<KaxBlockDuration*>(m_pElems[3]);
									/*
									kaxBlockDuration.ReadData(*m_pKaxIO);
									uint32 kbdur =  uint32(kaxBlockDuration) * TimeScale;
									*/
								}
bgroup:
								if (m_iUpElLev > 0) 
								{
									m_iUpElLev--;
									
									if (!(EbmlId(*m_pElems[3]) == KaxBlock::ClassInfos.GlobalId))
										delete m_pElems[3];		// done inside outpin's deliverblock()
									
									m_pElems[3] = m_pElems[4];
									if (m_iUpElLev > 0) 
										break;
								}
								else 
								{
									m_pElems[3]->SkipData(*m_pStream, m_pElems[3]->Generic().Context);
									
									if (!(EbmlId(*m_pElems[3]) == KaxBlock::ClassInfos.GlobalId))
										delete m_pElems[3];		// done inside outpin's deliverblock()
									
									if ((m_pElems[3] = m_pStream->FindNextElement(m_pElems[2]->Generic().Context, m_iUpElLev, 0xFFFFFFFFL, bReadDummyElements)) == NULL)
										return NULL;		// xyx - not that good - NULL may be signal for unknown element
								}
							}
						}
						
						if (m_iUpElLev > 0)
						{
							m_iUpElLev--;
							delete m_pElems[2];
							m_pElems[2] = m_pElems[3];
							if (m_iUpElLev > 0)
								break;
						}
						else 
						{
							m_pElems[2]->SkipData(*m_pStream, m_pElems[2]->Generic().Context);
							delete m_pElems[2];
							
							if ((m_pElems[2] = m_pStream->FindNextElement(m_pElems[1]->Generic().Context, m_iUpElLev, 0xFFFFFFFFL, bReadDummyElements)) == NULL)
								break;
						}
					}
				}
				/*
				else if (m_pElems[1]->IsDummy())//EbmlId(*m_pElems[1]) == DummyRawElement::ClassInfos.GlobalId)
				break;//continue;	// skip() = assert() !!!
				*/		
				if (m_iUpElLev > 0) 
				{
					m_iUpElLev--;
					delete m_pElems[1];
					m_pElems[1] = m_pElems[2];
					if (m_iUpElLev > 0)
						break;
				}
				else 
				{
					m_pElems[1]->SkipData(*m_pStream, m_pElems[1]->Generic().Context);
					delete m_pElems[1];
					
					if ((m_pElems[1] = m_pStream->FindNextElement(m_pElems[0]->Generic().Context, m_iUpElLev, 0xFFFFFFFFL, bReadDummyElements)) == NULL)
						break;
				}
			}
		}
		
		if (m_iUpElLev > 0)
		{
			m_iUpElLev--;
			delete m_pElems[0];
			m_pElems[0] = m_pElems[1];
			if (m_iUpElLev > 0)
				break;
		}
		else {
			m_pElems[0]->SkipData(*m_pStream, m_pElems[0]->Generic().Context);
			delete m_pElems[0];
			
			if ((m_pElems[0] = m_pStream->FindNextID(KaxSegment::ClassInfos, 0xFFFFFFFFL)) == NULL)
				break;
		}
	}
	
	return NULL;
}

// ----------------------------------------------------------------------------

void  MatroskaReader::Reset()
{
	m_pElems[0] = 0;
	m_pElems[1] = 0;
	m_iUpElLev = 0;
	m_iLevel = 0;
	m_pKaxIO->setFilePointer(0);	
}

// ----------------------------------------------------------------------------