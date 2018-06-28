/*
 *
 */

#include <typeinfo>

#include <unistd.h>
#include <execinfo.h>
#include "matroska_track.h"
#include "matroska_demux.h"
#include "xtr_base.h"
size_t c_aml_strlcpy(char*dst,const char*src,size_t size)
{
	size_t len = 0;
	while( ++len < size && *src) {
		*dst++ = *src++;
	}
	if(len <= size)
	{
		*dst=0;
	}
	return len-1;
}

size_t c_aml_strlcat(char *dst, const char *src, size_t size)
{
    size_t len = strlen(dst);
    printf("len=%d xuhang aa\n",len);
    return len + c_aml_strlcpy(dst + len, src, size);
}
void print_stacktrace()
{
    pid_t myPid = getpid();
    string pstackCommand= "pstack ";
    string pid;
    sprintf((char*)pid.c_str(),"%d",myPid);
    printf("len=%d \n",strlen(pid.c_str()));
    c_aml_strlcat((char*)pstackCommand.c_str(),pid.c_str(),strlen(pid.c_str())+1);
    printf("pid ---xuhang =%s \n",pstackCommand.c_str());
    system(pstackCommand.c_str());make_heap
}
/*
string format_binary(EbmlBinary &bin, int max_len = 10) {
    int len, i;
    string result;

    if (bin.GetSize() > max_len)
        len = max_len;
    else
        len = bin.GetSize();
    char *buffer = new char[40 + len * 5 + 1 + 3 + 24];
    const binary *b = bin.GetBuffer();
    buffer[0] = 0;
    mxprints(buffer, "length " LLD ", data:", bin.GetSize());
    for (i = 0; i < len; i++)
        mxprints(&buffer[strlen(buffer)], " 0x%02x", b[i]);
    if (len < bin.GetSize())
        mxprints(&buffer[strlen(buffer)], "...");
    result = buffer;
    strip(result);
    delete [] buffer;

    return result;
}
*/

void matroska_demux_c::sort_master(EbmlMaster &m) {
    int i;
    vector<EbmlElement *> tmp;
    vector<master_sorter_t> sort_me;

    for (i = 0; m.ListSize() > i; ++i)
        sort_me.push_back(master_sorter_t(i, m[i]->GetElementPosition()));
    sort(sort_me.begin(), sort_me.end());

    for (i = 0; sort_me.size() > i; ++i)
        tmp.push_back(m[sort_me[i].m_index]);
    m.RemoveAll();

    for (i = 0; tmp.size() > i; ++i)
        m.PushElement(*tmp[i]);
}


void matroska_demux_c::read_master(EbmlMaster *m, EbmlStream *es, const EbmlSemanticContext &ctx, int &upper_lvl_el, EbmlElement *&l2) {
    m->Read(*es, ctx, upper_lvl_el, l2, true);
    if (m->ListSize() == 0)
        return;

    sort_master(*m);
}

matroska_track_c* matroska_demux_c::find_track(uint32_t tnum) {
    if (m_tracks.size() == 0)
        return NULL;
    print_stacktrace();
    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        if (((matroska_track_c *)*i)->m_track_num == tnum)
            return ((matroska_track_c *)*i);
    }
    
    return NULL;
}

matroska_track_c* matroska_demux_c::find_track_by_uid(uint32_t tuid) {
    if (m_tracks.size() == 0)
        return NULL;
    
    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        if (((matroska_track_c *)*i)->m_track_uid == tuid)
            return ((matroska_track_c *)*i);
    }
    
    return NULL;
}

void matroska_demux_c::handle_cues (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, 
	EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3, EbmlElement *&l4, EbmlElement *&l5) {
    EbmlMaster *m1, *m2, *m3;
    int i1, i2, i3;

    uint32_t track_num = 0, block_num = 0;
    uint64_t pos = 0;
    uint64_t timecode = 0;
    bool key = false;
    printf("timecode=%s\n", timecode);
    print_stacktrace();
    upper_lvl_el = 0;
    m1 = static_cast<EbmlMaster *>(l1);
    read_master(m1, es, l1->Generic().Context, upper_lvl_el, l3);

    m_num_indexes = m1->ListSize();
    mp_indexes = new matroska_index_c[m_num_indexes];

    for (i1 = 0; i1 < m1->ListSize(); i1++) {
        l2 = (*m1)[i1];

        if (is_id(l2, KaxCuePoint)) {
            m2 = static_cast<EbmlMaster *>(l2);
            track_num = block_num = 0;
            pos = timecode = 0;
            key = true;

            for (i2 = 0; i2 < m2->ListSize(); i2++) {
                l3 = (*m2)[i2];

                if (is_id(l3, KaxCueTime)) {
                    KaxCueTime &cue_time = *static_cast<KaxCueTime *>(l3);
                    timecode = uint64(cue_time);
                } else if (is_id(l3, KaxCueTrackPositions)) {

                    m3 = static_cast<EbmlMaster *>(l3);
                    for (i3 = 0; i3 < m3->ListSize(); i3++) {
                        l4 = (*m3)[i3];

                        if (is_id(l4, KaxCueTrack)) {
                            KaxCueTrack &cue_track = *static_cast<KaxCueTrack *>(l4);
                            track_num = (uint32_t)cue_track;

                        } else if (is_id(l4, KaxCueClusterPosition)) {
                            KaxCueClusterPosition &cue_cp =
                                *static_cast<KaxCueClusterPosition *>(l4);
                            pos = uint64(cue_cp);

                        } else if (is_id(l4, KaxCueBlockNumber)) {
                            KaxCueBlockNumber &cue_bn =
                                *static_cast<KaxCueBlockNumber *>(l4);
                            block_num = (uint32_t)cue_bn;
                        }
                    } //else if (!is_global(es, l3, 3))
                } // while (l3 != NULL)                        
            } 
        }    
        // Store the index 
        matroska_index_c *pindex = &mp_indexes[i1];
        pindex->set_value (timecode, track_num, block_num, pos);
    } // while (l2 != NULL)
}

void matroska_demux_c::handle_attachments (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3) {
    EbmlMaster *m1, *m2;
    int i1, i2;

    upper_lvl_el = 0;
    m1 = static_cast<EbmlMaster *>(l1);
    read_master(m1, es, l1->Generic().Context, upper_lvl_el, l3);

    for (i1 = 0; i1 < m1->ListSize(); i1++) {
        l2 = (*m1)[i1];

        if (is_id(l2, KaxAttached)) {

            m2 = static_cast<EbmlMaster *>(l2);
            for (i2 = 0; i2 < m2->ListSize(); i2++) {
                l3 = (*m2)[i2];

                if (is_id(l3, KaxFileDescription)) {
//                    KaxFileDescription &f_desc =
//                        *static_cast<KaxFileDescription *>(l3);
                    ;
                } else if (is_id(l3, KaxFileName)) {
//                    KaxFileName &f_name =
//                        *static_cast<KaxFileName *>(l3);
                    ;
                } else if (is_id(l3, KaxMimeType)) {
//                    KaxMimeType &mime_type =
//                        *static_cast<KaxMimeType *>(l3);
                    ;
                } else if (is_id(l3, KaxFileData)) {
//                    KaxFileData &f_data =
//                        *static_cast<KaxFileData *>(l3);
                    ;
                } else if (is_id(l3, KaxFileUID)) {
//                    KaxFileUID &f_uid =
//                        *static_cast<KaxFileUID *>(l3);
                    ;
                } //else if (!is_global(es, l3, 3))
                //show_unknown_element(l3, 3);

            } // while (l3 != NULL)

        } //else if (!is_global(es, l2, 2))
        //show_unknown_element(l2, 2);

    } // while (l2 != NULL)
}

void matroska_demux_c::handle_seek_head (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3) {
    EbmlMaster *m1, *m2;
    int i1, i2, i;

    upper_lvl_el = 0;
    m1 = static_cast<EbmlMaster *>(l1);
    read_master(m1, es, l1->Generic().Context, upper_lvl_el, l3);

    mb_has_seekhead = true;
    for (i1 = 0; i1 < m1->ListSize(); i1++) {
        l2 = (*m1)[i1];

        if (is_id(l2, KaxSeek)) {
            EbmlId id = EbmlVoid::ClassInfos.GlobalId;
            int64_t i_pos = -1;

            m2 = static_cast<EbmlMaster *>(l2);
            for (i2 = 0; i2 < m2->ListSize(); i2++) {
                l3 = (*m2)[i2];
                if (is_id(l3, KaxSeekID)) {
                    KaxSeekID &sid = *(KaxSeekID*)l3;
                    id = EbmlId(sid.GetBuffer(), sid.GetSize ());
                } else if (is_id(l3, KaxSeekPosition)) {
                    KaxSeekPosition &seek_pos = static_cast<KaxSeekPosition &>(*l3);
                    i_pos = uint64(seek_pos);
                } //else if (!is_global(es, l3, 3))
                //show_unknown_element(l3, 3);

            } // while (l3 != NULL)

            if (i_pos >= 0) {
                if (id == KaxCues::ClassInfos.GlobalId) {
                    fprintf (stderr, "cues at %lld!\n", i_pos);
                    m_cues_position = static_cast<KaxSegment*>(mp_l0)->GetGlobalPosition(i_pos);
                } else if (id == KaxChapters::ClassInfos.GlobalId) {
                    fprintf (stderr, "chapters at %lld!\n", i_pos);
                    m_chapters_position =static_cast<KaxSegment*>(mp_l0)->GetGlobalPosition(i_pos);// mp_l0->GetGlobalPosition(i_pos);
                } else if (id == KaxTags::ClassInfos.GlobalId) {
                    fprintf (stderr, "tags at %lld!\n", i_pos);
                    m_tags_position =static_cast<KaxSegment*>(mp_l0)->GetGlobalPosition(i_pos);// mp_l0->GetGlobalPosition(i_pos);
                }
            }

        } //else if (!is_global(es, l2, 2))
        //show_unknown_element(l2, 2);

    } // while (l2 != NULL)
}

void matroska_demux_c::handle_audio_track (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el,
                                           EbmlElement *&l3, EbmlElement *&l4, matroska_track_c *&ptrack) {
    EbmlMaster *m3;
    int i3;

    m_num_audio_tracks++;
    matroska_audio_track_c *patrack = new matroska_audio_track_c ();
    ptrack->mp_track = patrack;

    m3 = static_cast<EbmlMaster *>(l3);
    for (i3 = 0; i3 < m3->ListSize(); i3++) {
        l4 = (*m3)[i3];

        if (is_id(l4, KaxAudioSamplingFreq)) {
            KaxAudioSamplingFreq &freq =
                *static_cast<KaxAudioSamplingFreq *>(l4);
            patrack->m_sampling_freq = (float)freq;

        } else if (is_id(l4, KaxAudioOutputSamplingFreq)) {
            KaxAudioOutputSamplingFreq &ofreq =
                *static_cast<KaxAudioOutputSamplingFreq *>(l4);
            patrack->m_out_sampling_freq = (float)ofreq;

        } else if (is_id(l4, KaxAudioChannels)) {
            KaxAudioChannels &channels =
                *static_cast<KaxAudioChannels *>(l4);
            patrack->m_channels = (uint32_t)channels;

#if MATROSKA_VERSION >= 2
        } else if (is_id(l4, KaxAudioPosition)) {
            //string strc;

            //KaxAudioPosition &positions =
            //    *static_cast<KaxAudioPosition *>(l4);
            //strc = format_binary(positions);
#endif

        } else if (is_id(l4, KaxAudioBitDepth)) {
            KaxAudioBitDepth &bps =
                *static_cast<KaxAudioBitDepth *>(l4);
            patrack->m_bits_per_sample = uint32(bps);

        } //else if (!is_global(es, l4, 4))
        //show_unknown_element(l4, 4);

    } // while (l4 != NULL)
}

void matroska_demux_c::handle_video_track (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, 
                                           EbmlElement *&l3, EbmlElement *&l4, matroska_track_c *&ptrack) {
    EbmlMaster *m3;
    int i3;
    string strc;

    m_num_video_tracks++;
    matroska_video_track_c *pvtrack = new matroska_video_track_c ();
    ptrack->mp_track = pvtrack;

    m3 = static_cast<EbmlMaster *>(l3);
    for (i3 = 0; i3 < m3->ListSize(); i3++) {
        l4 = (*m3)[i3];

        if (is_id(l4, KaxVideoPixelWidth)) {
            KaxVideoPixelWidth &width =
                *static_cast<KaxVideoPixelWidth *>(l4);
            pvtrack->m_pixel_width = uint32(width);

        } else if (is_id(l4, KaxVideoPixelHeight)) {
            KaxVideoPixelHeight &height =
                *static_cast<KaxVideoPixelHeight *>(l4);
            pvtrack->m_pixel_height = uint32(height);

        } else if (is_id(l4, KaxVideoDisplayWidth)) {
            KaxVideoDisplayWidth &width =
                *static_cast<KaxVideoDisplayWidth *>(l4);
            pvtrack->m_display_width = uint32(width);

        } else if (is_id(l4, KaxVideoDisplayHeight)) {
            KaxVideoDisplayHeight &height =
                *static_cast<KaxVideoDisplayHeight *>(l4);
            pvtrack->m_display_height = uint32(height);

        } else if (is_id(l4, KaxVideoPixelCropLeft)) {
//            KaxVideoPixelCropLeft &left =
//                *static_cast<KaxVideoPixelCropLeft *>(l4);
        } else if (is_id(l4, KaxVideoPixelCropTop)) {
//            KaxVideoPixelCropTop &top =
//                *static_cast<KaxVideoPixelCropTop *>(l4);
        } else if (is_id(l4, KaxVideoPixelCropRight)) {
//            KaxVideoPixelCropRight &right =
//                *static_cast<KaxVideoPixelCropRight *>(l4);
        } else if (is_id(l4, KaxVideoPixelCropBottom)) {
//            KaxVideoPixelCropBottom &bottom =
//                *static_cast<KaxVideoPixelCropBottom *>(l4);
#if MATROSKA_VERSION >= 2
        } else if (is_id(l4, KaxVideoDisplayUnit)) {
//            KaxVideoDisplayUnit &unit =
//                *static_cast<KaxVideoDisplayUnit *>(l4);
        } else if (is_id(l4, KaxVideoGamma)) {
//            KaxVideoGamma &gamma =
//                *static_cast<KaxVideoGamma *>(l4);
        } else if (is_id(l4, KaxVideoFlagInterlaced)) {
            KaxVideoFlagInterlaced &f_interlaced =
                *static_cast<KaxVideoFlagInterlaced *>(l4);
            fprintf (stderr, "%s %d: Video track %d\n", __FUNCTION__, __LINE__, uint32(f_interlaced));
            pvtrack->mb_deinterlaced = (uint8(f_interlaced)>0) ? true : false;
        } else if (is_id(l4, KaxVideoStereoMode)) {
//            KaxVideoStereoMode &stereo =
//                *static_cast<KaxVideoStereoMode *>(l4);
        } else if (is_id(l4, KaxVideoAspectRatio)) {
//            KaxVideoAspectRatio &ar_type =
//                *static_cast<KaxVideoAspectRatio *>(l4);
#endif
        } else if (is_id(l4, KaxVideoColourSpace)) {
            //KaxVideoColourSpace &cspace =
            //    *static_cast<KaxVideoColourSpace *>(l4);
            //strc = format_binary(cspace);
        } else if (is_id(l4, KaxVideoFrameRate)) {
            KaxVideoFrameRate &framerate =
                *static_cast<KaxVideoFrameRate *>(l4);
            pvtrack->m_frame_rate = float(framerate);

        } //else if (!is_global(es, l4, 4))
        //show_unknown_element(l4, 4);

    } // while (l4 != NULL)
}

void matroska_demux_c::handle_tracks (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, 
	EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3, EbmlElement *&l4) {
    EbmlMaster *m1, *m2;
    int i1, i2;
    string strc;

    if (mb_tracks_found) {
        fprintf (stderr, "Tracks already parsed!\n");
        return;
    }

    // Yep, we've found our KaxTracks element. Now find all tracks
    // contained in this segment.
    upper_lvl_el = 0;
    m1 = static_cast<EbmlMaster *>(l1);
    read_master(m1, es, l1->Generic().Context, upper_lvl_el, l3);

    for (i1 = 0; i1 < m1->ListSize(); i1++) {
        l2 = (*m1)[i1];

        if (is_id(l2, KaxTrackEntry)) {
            // We actually found a track entry :) We're happy now.
            m_num_tracks++;

            m2 = static_cast<EbmlMaster *>(l2);
            matroska_track_c *ptrack = new matroska_track_c ();
            m_tracks.push_back (ptrack);

            for (i2 = 0; i2 < m2->ListSize(); i2++) {
                l3 = (*m2)[i2];
                
                // Now evaluate the data belonging to this track
                if (is_id(l3, KaxTrackAudio)) {
                    handle_audio_track (in, es, upper_lvl_el, l3, l4, ptrack);
				}

                else if (is_id(l3, KaxTrackVideo)) {
                    handle_video_track (in, es, upper_lvl_el, l3, l4, ptrack);
				}

                else if (is_id(l3, KaxTrackNumber)) {
                    KaxTrackNumber &tnum = *static_cast<KaxTrackNumber *>(l3);
                    if (find_track((uint32_t)tnum) == NULL) {
                        ptrack->m_track_num = (uint32_t)tnum;
                    }

                } else if (is_id(l3, KaxTrackUID)) {
                    KaxTrackUID &tuid = *static_cast<KaxTrackUID *>(l3);
                    if (find_track_by_uid((uint32_t)tuid) == NULL) {
                        ptrack->m_track_uid = (uint32_t)tuid;
                    }

                } else if (is_id(l3, KaxTrackType)) {
                    KaxTrackType &ttype = *static_cast<KaxTrackType *>(l3);

                    switch (uint8(ttype)) {
                    case track_audio:
                        ptrack->m_track_type = MATROSKA_TRACK_TYPE_AUDIO;
                        break;
                    case track_video:
                        ptrack->m_track_type = MATROSKA_TRACK_TYPE_VIDEO;
                        break;
                    case track_subtitle:
                        ptrack->m_track_type = MATROSKA_TRACK_TYPE_SUBTITLE;
                        break;
                    case track_buttons:
                        ptrack->m_track_type = MATROSKA_TRACK_TYPE_BUTTONS;
                        break;
                    default:
                        ptrack->m_track_type = MATROSKA_TRACK_TYPE_UNKNOWN;
                        break;
                    }

#if MATROSKA_VERSION >= 2
                } else if (is_id(l3, KaxTrackFlagEnabled)) {
                    KaxTrackFlagEnabled &fenabled =
                        *static_cast<KaxTrackFlagEnabled *>(l3);
                    ptrack->mb_enabled_flag = uint64(fenabled)?true:false;

#endif

                    //} else if (is_id(l3, KaxTrackName)) {
                    //KaxTrackName &name = *static_cast<KaxTrackName *>(l3);
                    //ptrack->m_track_name = string(name);

                } else if (is_id(l3, KaxCodecID)) {
                    KaxCodecID &codec_id = *static_cast<KaxCodecID *>(l3);
                    ptrack->m_codec_id = string(codec_id);

                } else if (is_id(l3, KaxCodecPrivate)) {
                    KaxCodecPrivate &c_priv = *static_cast<KaxCodecPrivate *>(l3);
                    ptrack->m_codec_private_len = c_priv.GetSize();
                    ptrack->mp_codec_private = new uint8_t[ptrack->m_codec_private_len];
                    memcpy (ptrack->mp_codec_private, c_priv.GetBuffer(), ptrack->m_codec_private_len);


                } else if (is_id(l3, KaxCodecName)) {
                    KaxCodecName &c_name = *static_cast<KaxCodecName *>(l3);
                    ptrack->m_codec_name = string(UTF2STR(c_name));
#if MATROSKA_VERSION >= 2
                } else if (is_id(l3, KaxCodecSettings)) {
//                    KaxCodecSettings &c_sets =
//                        *static_cast<KaxCodecSettings *>(l3);
                } else if (is_id(l3, KaxCodecInfoURL)) {
//                    KaxCodecInfoURL &c_infourl =
//                        *static_cast<KaxCodecInfoURL *>(l3);
                } else if (is_id(l3, KaxCodecDownloadURL)) {
//                    KaxCodecDownloadURL &c_downloadurl =
//                        *static_cast<KaxCodecDownloadURL *>(l3);
                } else if (is_id(l3, KaxCodecDecodeAll)) {
//                    KaxCodecDecodeAll &c_decodeall =
//                        *static_cast<KaxCodecDecodeAll *>(l3);
                } else if (is_id(l3, KaxTrackOverlay)) {
//                    KaxTrackOverlay &overlay = *static_cast<KaxTrackOverlay *>(l3);
#endif // MATROSKA_VERSION >= 2

                } else if (is_id(l3, KaxTrackMinCache)) {
//                    KaxTrackMinCache &min_cache =
//                        *static_cast<KaxTrackMinCache *>(l3);
                } else if (is_id(l3, KaxTrackMaxCache)) {
//                    KaxTrackMaxCache &max_cache =
//                        *static_cast<KaxTrackMaxCache *>(l3);
                } else if (is_id(l3, KaxTrackDefaultDuration)) {
                    KaxTrackDefaultDuration &def_duration =
                        *static_cast<KaxTrackDefaultDuration *>(l3);
                    ptrack->m_default_duration = uint64(def_duration);
                } else if (is_id(l3, KaxTrackFlagLacing)) {
//                    KaxTrackFlagLacing &f_lacing =
//                        *static_cast<KaxTrackFlagLacing *>(l3);
                } else if (is_id(l3, KaxTrackFlagDefault)) {
                    KaxTrackFlagDefault &f_default =
                        *static_cast<KaxTrackFlagDefault *>(l3);
                    ptrack->mb_default_flag = uint64(f_default)?true:false;

                } else if (is_id(l3, KaxTrackFlagForced)) {
//                    KaxTrackFlagForced &f_forced =
//                        *static_cast<KaxTrackFlagForced *>(l3);
                } else if (is_id(l3, KaxTrackLanguage)) {
                    KaxTrackLanguage &language =
                        *static_cast<KaxTrackLanguage *>(l3);
                    ptrack->m_language = string(language);

                } else if (is_id(l3, KaxTrackTimecodeScale)) {
                    KaxTrackTimecodeScale &tm_timecode_scale =
                        *static_cast<KaxTrackTimecodeScale *>(l3);
                    ptrack->m_timecode_scale = float(tm_timecode_scale);
                } else if (is_id(l3, KaxMaxBlockAdditionID)) {
//                    KaxMaxBlockAdditionID &max_block_add_id =
//                        *static_cast<KaxMaxBlockAdditionID *>(l3);
                } //else if (is_id(l3, KaxContentEncodings))
                //handle(content_encodings);

                //else if (!is_global(es, l3, 3))
                //    show_unknown_element(l3, 3);

            }
        } //else if (!is_global(es, l2, 2))
          //  show_unknown_element(l2, 2);
    }
    
    mb_tracks_found = true;
}

void matroska_demux_c::handle_blockgroup(KaxBlockGroup &blockgroup, KaxCluster &cluster, uint64_t tc_scale) {
    KaxBlock *block;
    KaxBlockDuration *kduration;
    KaxReferenceBlock *kreference;
    KaxBlockAdditions *kadditions;
    KaxCodecState *kcstate;
    xtr_base_c *extractor;
    int64_t duration, fref, bref;
    int i;

    // Only continue if this block group actually contains a block.
    block = FINDFIRST(&blockgroup, KaxBlock);
    if ((NULL == block) || (0 == block->NumberFrames()))
        return;

    block->SetParent(cluster);

    // Do we need this block group?
    extractor = NULL;
    for (i = 0; i < m_tracks.size(); i++)
        if (block->TrackNum() == m_tracks[i]->m_track_num) {
            extractor = m_tracks[i]->mp_extractor;
            break;
        }
    
    if (NULL == extractor)
        return;

    // Next find the block duration if there is one.
    kduration = FINDFIRST(&blockgroup, KaxBlockDuration);
    if (NULL == kduration)
        duration = -1;
    else
        duration = uint64(*kduration) * tc_scale;

    // Now find backward and forward references.
    bref = 0;
    fref = 0;
    kreference = FINDFIRST(&blockgroup, KaxReferenceBlock);
    for (i = 0; (2 > i) && (NULL != kreference); i++) {
        if (int64(*kreference) < 0)
            bref = int64(*kreference);
        else
            fref = int64(*kreference);
        kreference = FINDNEXT(&blockgroup, KaxReferenceBlock, kreference);
    }

    // Any block additions present?
    kadditions = FINDFIRST(&blockgroup, KaxBlockAdditions);

    if (0 > duration)
        duration = extractor->default_duration * block->NumberFrames();

    kcstate = FINDFIRST(&blockgroup, KaxCodecState);
    if (NULL != kcstate) {
        memory_cptr codec_state(new memory_c(kcstate->GetBuffer(),
                                             kcstate->GetSize(), false));
        extractor->handle_codec_state(codec_state);
    }

    for (i = 0; i < block->NumberFrames(); i++) {
        int64_t this_timecode, this_duration;

        // Need to seek, give up the current cluster
        if (mb_need_to_seek || mb_require_to_close) {
            fprintf (stderr, "We need to seek or close, give up the cluster!\n");
            break;
        }

        if (0 > duration) {
            this_timecode = block->GlobalTimecode();
            this_duration = duration;
        } else {
            this_timecode = block->GlobalTimecode() + i * duration /
                block->NumberFrames();
            this_duration = duration / block->NumberFrames();
        }

        if (mb_skip_to_timecode) {
            if (this_timecode < m_seek_time) 
                continue;
            else {
                mb_skip_to_timecode = false;
                m_seek_time = 0;
            }
        } 
        
        DataBuffer &data = block->GetBuffer(i);
        memory_cptr frame(new memory_c(data.Buffer(), data.Size(), false));
        extractor->handle_frame(frame, kadditions, this_timecode, this_duration,
                                bref, fref, false, false, true);
    }
}

void matroska_demux_c::handle_simpleblock(KaxSimpleBlock &simpleblock, KaxCluster &cluster) {
    xtr_base_c *extractor;
    int64_t duration;
    size_t i;
    int a=simpleblock.NumberFrames();
    if (0 == simpleblock.NumberFrames())
        return;

    simpleblock.SetParent(cluster);
    int b=simpleblock.TrackNum();
    // Do we need this block group?
    extractor = NULL;
    for (i = 0; i < m_tracks.size(); i++) {
		int c=m_tracks[i]->m_track_num;
        if (simpleblock.TrackNum() == m_tracks[i]->m_track_num) {
            extractor = m_tracks[i]->mp_extractor;
            break;
        }
    }
    if (NULL == extractor)
        return;

    duration = extractor->default_duration * simpleblock.NumberFrames();

    for (i = 0; i < simpleblock.NumberFrames(); i++) {
        int64_t this_timecode, this_duration;

        // Need to seek, give up the current cluster
        if (mb_need_to_seek || mb_require_to_close) {
            fprintf (stderr, "We need to seek or close, give up the cluster!\n");
            break;
        }

        if (0 > duration) {
            this_timecode = simpleblock.GlobalTimecode();
            this_duration = duration;
        } else {
            this_timecode = simpleblock.GlobalTimecode() + i * duration /
                simpleblock.NumberFrames();
            this_duration = duration / simpleblock.NumberFrames();
        }

        if (mb_skip_to_timecode) {
            if (this_timecode < m_seek_time) 
                continue;
            else if (!simpleblock.IsKeyframe() || simpleblock.IsDiscardable())
                continue;
            else {
                mb_skip_to_timecode = false;
                m_seek_time = 0;
            }
        } 

        DataBuffer &data = simpleblock.GetBuffer(i);
        memory_cptr frame(new memory_c(data.Buffer(), data.Size(), false));
        extractor->handle_frame(frame, NULL, this_timecode, this_duration,
                                -1, -1, simpleblock.IsKeyframe(),
                                simpleblock.IsDiscardable(), false);
    }
}

void matroska_demux_c::handle_cluster (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el,
                                       EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3,
                                       KaxCluster *&cluster, int64_t file_size) {
    EbmlMaster *m1;
    int i1;

    cluster = (KaxCluster *)l1;
    printf("handle_cluster \n");
    upper_lvl_el = 0;
    m1 = static_cast<EbmlMaster *>(l1);
    read_master(m1, es, l1->Generic().Context, upper_lvl_el, l3);
    printf("read_master \n");
    for (i1 = 0; i1 < m1->ListSize(); i1++) {
        l2 = (*m1)[i1];
        printf("m1->ListSize() =%d\n",m1->ListSize());
        // Need to seek, give up the current cluster
        if (mb_need_to_seek || mb_require_to_close) {
            fprintf (stderr, "We need to seek or close, give up the cluster!\n");
            break;
        }
        printf("l2->GlobalId() =%d\n",l2->Generic().GlobalId);
        if (is_id(l2, KaxClusterTimecode)) {
            int64_t cluster_tc;
			 printf("l2->KaxClusterTimecode() =%d\n",l2->Generic().GlobalId);
            KaxClusterTimecode &ctc = *static_cast<KaxClusterTimecode *>(l2);
            cluster_tc = uint64(ctc);
            //fprintf (stderr, "Cluster timecode: %.3fs\n", (float)cluster_tc * (float)m_timecode_scale / 1000000000.0);
            cluster->InitTimecode(cluster_tc, m_timecode_scale);
            if (!mb_first_cluster_timecode) {
                m_first_cluster_timecode = uint64(cluster_tc) * m_timecode_scale / 1000000;
                mb_first_cluster_timecode = true;
            }

        } else if (is_id(l2, KaxClusterPosition)) {
            printf("l2->KaxClusterPosition() =%d\n",l2->Generic().GlobalId);
            KaxClusterPosition &c_pos =
                *static_cast<KaxClusterPosition *>(l2);
            //fprintf (stderr, "Cluster position: " LLU "\n", uint64(c_pos));

        } else if (is_id(l2, KaxClusterPrevSize)) {
            printf("l2->KaxClusterPrevSize() =%d\n",l2->Generic().GlobalId);
            KaxClusterPrevSize &c_psize =
                *static_cast<KaxClusterPrevSize *>(l2);
            //fprintf (stderr, "Cluster previous size: " LLU "\n", uint64(c_psize));

        } //else if (is_id(l2, KaxClusterSilentTracks))
        //handle2(silent_track, cluster);

        else if (is_id(l2, KaxBlockGroup)) {
			printf("l2->KaxBlockGroup() =%d\n",l2->Generic().GlobalId);
            l2->Read(*es, KaxBlockGroup::ClassInfos.Context, upper_lvl_el, l3, true);
            handle_blockgroup(*static_cast<KaxBlockGroup *>(l2), *cluster, m_timecode_scale);
        } else if (is_id(l2, KaxSimpleBlock)) {
            printf("l2->KaxSimpleBlock() i1 =%d\n",i1);
			print_stacktrace();
			if(i1==242)
			  int d=1;
            l2->Read(*es, KaxSimpleBlock::ClassInfos.Context, upper_lvl_el, l3, true);
			printf(" prepare KaxSimpleBlock() i1 =%d\n",i1);
            handle_simpleblock(*static_cast<KaxSimpleBlock *>(l2), *cluster);
        }
    } // while (l2 != NULL)
}

void matroska_demux_c::handle_info (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3) {
    EbmlMaster *m1;
    int i1, i;

    // General info about this Matroska file
    upper_lvl_el = 0;
    m1 = static_cast<EbmlMaster *>(l1);
    read_master(m1, es, l1->Generic().Context, upper_lvl_el, l3);

    for (i1 = 0; i1 < m1->ListSize(); i1++) {
        l2 = (*m1)[i1];

        if (is_id(l2, KaxTimecodeScale)) {
            KaxTimecodeScale &km_timecode_scale = *static_cast<KaxTimecodeScale *>(l2);
            m_timecode_scale = uint64(km_timecode_scale);

        } else if (is_id(l2, KaxDuration)) {
            KaxDuration &duration = *static_cast<KaxDuration *>(l2);
			m_duration = uint64_t(double(duration) * m_timecode_scale);
        } else if (is_id(l2, KaxMuxingApp)) {
//            KaxMuxingApp &muxingapp = *static_cast<KaxMuxingApp *>(l2);

        } else if (is_id(l2, KaxWritingApp)) {
//            KaxWritingApp &writingapp = *static_cast<KaxWritingApp *>(l2);

        } else if (is_id(l2, KaxDateUTC)) {
            struct tm tmutc;
            time_t temptime;
            char buffer[40];
            KaxDateUTC &dateutc = *static_cast<KaxDateUTC *>(l2);
            temptime = dateutc.GetEpochDate();
            if ((gmtime_r(&temptime, &tmutc) != NULL) &&
                (asctime_r(&tmutc, buffer) != NULL)) {
                buffer[strlen(buffer) - 1] = 0;
            }
        } else if (is_id(l2, KaxSegmentUID)) {
            KaxSegmentUID &uid = *static_cast<KaxSegmentUID *>(l2);
            char *buffer = new char[uid.GetSize() * 5 + 1];
            const binary *b = uid.GetBuffer();
            buffer[0] = 0;
            for (i = 0; i < uid.GetSize(); i++)
                mxprints(&buffer[strlen(buffer)], " 0x%02x", b[i]);
            delete [] buffer;

        } else if (is_id(l2, KaxSegmentFamily)) {
            KaxSegmentFamily &uid = *static_cast<KaxSegmentFamily *>(l2);
            char *buffer = new char[uid.GetSize() * 5 + 1];
            const binary *b = uid.GetBuffer();
            buffer[0] = 0;
            for (i = 0; i < uid.GetSize(); i++)
                mxprints(&buffer[strlen(buffer)], " 0x%02x", b[i]);
            delete [] buffer;

        } //else if (is_id(l2, KaxChapterTranslate))
        //handle(chaptertranslate);

        else if (is_id(l2, KaxPrevUID)) {
            KaxPrevUID &uid = *static_cast<KaxPrevUID *>(l2);
            char* buffer = new char[uid.GetSize() * 5 + 1];
            const binary *b = uid.GetBuffer();
            buffer[0] = 0;
            for (i = 0; i < uid.GetSize(); i++)
                mxprints(&buffer[strlen(buffer)], " 0x%02x", b[i]);
            delete [] buffer;

        } else if (is_id(l2, KaxPrevFilename)) {
//            KaxPrevFilename &filename = *static_cast<KaxPrevFilename *>(l2);
        } else if (is_id(l2, KaxNextUID)) {
            KaxNextUID &uid = *static_cast<KaxNextUID *>(l2);
            char *buffer = new char[uid.GetSize() * 5 + 1];
            const binary *b = uid.GetBuffer();
            buffer[0] = 0;
            for (i = 0; i < uid.GetSize(); i++)
                mxprints(&buffer[strlen(buffer)], " 0x%02x", b[i]);
            delete [] buffer;

        } else if (is_id(l2, KaxNextFilename)) {
//            KaxNextFilename &filename = *static_cast<KaxNextFilename *>(l2);
        } else if (is_id(l2, KaxSegmentFilename)) {
//            KaxSegmentFilename &filename =
//                *static_cast<KaxSegmentFilename *>(l2);
        } else if (is_id(l2, KaxTitle)) {
//            KaxTitle &title = *static_cast<KaxTitle *>(l2);
        }
    }
}

void matroska_demux_c::handle_ebml_head(EbmlElement *l0, mm_io_c *in, EbmlStream *es) {
    int upper_lvl_el;
    EbmlElement *e;

    while (demux_in_parent(l0)) {
        upper_lvl_el = 0;
        e = es->FindNextElement(l0->Generic().Context, upper_lvl_el, 0xFFFFFFFFL, true);
        if (NULL == e)
            return;

        e->ReadData(*in);

       // if (is_id(e, EVersion))
       //     fprintf (stderr, "EBML version: " LLU, uint64(*static_cast<EbmlUInteger *>(e)));
       // else if (is_id(e, EReadVersion))
       //     fprintf (stderr, "EBML read version: " LLU, uint64(*static_cast<EbmlUInteger *>(e)));
       // else if (is_id(e, EMaxIdLength))
       //     fprintf (stderr, "EBML maximum ID length: " LLU, uint64(*static_cast<EbmlUInteger *>(e)));
       // else if (is_id(e, EDocType))
       //     fprintf (stderr, "Doc type: %s", string(*static_cast<EbmlString *>(e)).c_str());
       // else if (is_id(e, EDocTypeVersion))
       //     fprintf (stderr, "Doc type version: " LLU, uint64(*static_cast<EbmlUInteger *>(e)));
       // else if (is_id(e, EDocTypeReadVersion))
       //     fprintf (stderr, "Doc type read version: " LLU, uint64(*static_cast<EbmlUInteger *>(e)));
        //else
        //    show_unknown_element(e, 1);

        e->SkipData(*es, e->Generic().Context);
        delete e;
    }
}

/*
 * Get the first available video track ID, in the order of
 * default -> enabled -> first
 */
uint32_t matroska_demux_c::GetVideoTrackID () {
    if (m_num_video_tracks <= 0)
        return 0;

    // Firstly, get the defaulted video track
    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        matroska_track_c *ptrack = (matroska_track_c *)(*i);
        if (ptrack->m_track_type == MATROSKA_TRACK_TYPE_VIDEO) {
            if (ptrack->mb_default_flag) {
                fprintf (stderr, "Select track[%02d] for video playback!\n", ptrack->m_track_num);
                return ptrack->m_track_num;
            }
        }
    }

    // Not found, just get the first found video track
    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        matroska_track_c *ptrack = (matroska_track_c *)(*i);
        if (ptrack->m_track_type == MATROSKA_TRACK_TYPE_VIDEO) {
            fprintf (stderr, "Select track[%02d] for video playback!\n", ptrack->m_track_num);
            return ptrack->m_track_num;
        }
    }

    // Video not available
    return 0;
}


/*
 * Get the first available audio track ID, in the order of
 * default -> enabled -> first
 */
uint32_t matroska_demux_c::GetAudioTrackID () {
    if (m_num_audio_tracks <= 0)
        return 0;

    // Firstly, get the defaulted audio track
    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        matroska_track_c *ptrack = (matroska_track_c *)(*i);
        if (ptrack->m_track_type == MATROSKA_TRACK_TYPE_AUDIO) {
            if (ptrack->mb_default_flag) {
                fprintf (stderr, "Select track[%02d] for audio playback!\n", ptrack->m_track_num);
                return ptrack->m_track_num;
            }
        }
    }

    // Not found, just get the first found audio track
    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        matroska_track_c *ptrack = (matroska_track_c *)(*i);
        if (ptrack->m_track_type == MATROSKA_TRACK_TYPE_AUDIO) {
            fprintf (stderr, "Select track[%02d] for audio playback!\n", ptrack->m_track_num);
            return ptrack->m_track_num;
        }
    }


    // Audio not available
    return 0;
}


uint32_t matroska_demux_c::GetVideoTrackIDByIndex (uint32_t index) {

}

uint32_t matroska_demux_c::GetAudioTrackIDByIndex (uint32_t index) {
    uint32_t idx = 1;

    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        matroska_track_c *ptrack = (matroska_track_c *)(*i);
        if (ptrack->m_track_type == MATROSKA_TRACK_TYPE_AUDIO) {
            if (idx++ == index) {
                fprintf (stderr, "Select track[%02d] for audio playback!\n", ptrack->m_track_num);
                return ptrack->m_track_num;
            }
        }
    }
}

uint32_t matroska_demux_c::GetSPUTrackIDByIndex (uint32_t index) {

}

uint32_t matroska_demux_c::GetSubtitleTrackIDByIndex (uint32_t index) {

}


/*
 * Select default tracks for playback
 */
MKV_RT matroska_demux_c::PreparePlayback () {
    int upper_lvl_el = 0;

    // Load the cues
    if (m_cues_position > 0) {
        mp_es->I_O().setFilePointer (m_cues_position, seek_beginning);
        mp_l1 = mp_es->FindNextElement(mp_l0->Generic().Context, upper_lvl_el, 0xFFFFFFFFL, true);        
        if ((mp_l1 != NULL) && (upper_lvl_el <= 0)) {
            if (is_id (mp_l1, KaxCues))
                handle_cues (mp_io_in, mp_es, upper_lvl_el, mp_l1, mp_l2, mp_l3, mp_l4, mp_l5);
        }
        delete mp_l1;
        mp_l1 = NULL;
    }

    // Set the file pointer to the first cluster
    mp_es->I_O().setFilePointer (m_first_cluster_pos, seek_beginning);

    return MKV_RT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

matroska_track_c* matroska_demux_c::GetTrackByID (uint32_t trackID) {
    matroska_track_c *ptrack = NULL;

    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        ptrack = (matroska_track_c *)(*i);

        if (ptrack->m_track_num == trackID) {
            fprintf (stderr, "%s %s %d!\n", __FILE__, __FUNCTION__, __LINE__);
            return ptrack;
        }
    }

    return ptrack;
}

matroska_track_c* matroska_demux_c::OpenTrack (matroska_track_c *ptrack) {
    assert (ptrack != NULL);
    if (open_extractor (ptrack) > 0) {
        fprintf (stderr, "%s %s %d!\n", __FILE__, __FUNCTION__, __LINE__);

        switch (ptrack->m_track_type) {
        case MATROSKA_TRACK_TYPE_VIDEO:
            fprintf (stderr, "%s %s %d!\n", __FILE__, __FUNCTION__, __LINE__);

            mp_video_track = ptrack;
            break;
        case MATROSKA_TRACK_TYPE_AUDIO:
            fprintf (stderr, "%s %s %d!\n", __FILE__, __FUNCTION__, __LINE__);

            mp_audio_track = ptrack;
            break;
        default:
            break;
        }
        return ptrack;
    }

    return NULL;
}

MKV_RT matroska_demux_c::CloseTrack (matroska_track_c *ptrack) {
    //assert ((ptrack != NULL) && (ptrack->mp_extractor != NULL));

    return MKV_RT_SUCCESS;
}

int matroska_demux_c::open_video_track (matroska_track_c *ptrack) {
    if (NULL == ptrack) {
        fprintf (stderr, "Can't find track\n");
        return -1;
    }

    if (ptrack->m_track_type != MATROSKA_TRACK_TYPE_VIDEO) {
        fprintf (stderr, "Track[%02d] is not a video track!\n", ptrack->m_track_num);
        return -1;
    }
    open_extractor (ptrack);
    mp_video_track = ptrack;

    return 0;
}

int matroska_demux_c::open_audio_track (matroska_track_c *ptrack) {
    if (NULL == ptrack) {
        fprintf (stderr, "Can't find track\n");
        return -1;
    }

    if (ptrack->m_track_type != MATROSKA_TRACK_TYPE_AUDIO) {
        fprintf (stderr, "Track[%02d] is not an audio track!\n", ptrack->m_track_num);
        return -1;
    }

    open_extractor (ptrack);
    mp_audio_track = ptrack;

    return 0;
}

int matroska_demux_c::open_subtitle_track (matroska_track_c *ptrack) {
    if (NULL == ptrack) {
        fprintf (stderr, "Can't find track\n");
        return -1;
    }

    if (ptrack->m_track_type != MATROSKA_TRACK_TYPE_SUBTITLE) {
        fprintf (stderr, "Track[%02d] is not a subtitle track!\n", ptrack->m_track_num);
        return -1;
    }
    open_extractor (ptrack);

    return 0;
}

int matroska_demux_c::open_extractor (matroska_track_c *ptrack) {
    struct track_spec_t tspec;
    char outname[16];

    sprintf (outname, "out.%d", ptrack->m_track_num);
    tspec.tid = ptrack->m_track_num;
    tspec.tuid = ptrack->m_track_uid;
    tspec.out_name = outname;
    fprintf (stderr, "%s %s %d!\n", __FILE__, __FUNCTION__, __LINE__);

    ptrack->mp_extractor = xtr_base_c::create_extractor (ptrack->m_codec_id, ptrack->m_track_num, tspec);

    if (NULL == ptrack->mp_extractor) {
        fprintf (stderr, "Extraction of track number %d with the CodecID '%s' is not supported.\n", ptrack->m_track_num, ptrack->m_codec_id.c_str());
        return -1;
    }

    ptrack->mp_extractor->create_file(NULL, ptrack);
    return 1;
}

int matroska_demux_c::close_extractor (xtr_base_c *extractor) {
    if (NULL == extractor) {
        return -1;
    }

    extractor->finish_track();
    extractor->finish_file();
    delete extractor;

    return 1;
}

MKV_RT matroska_demux_c::DumpTracks () {
    if (m_tracks.size() == 0) {
        fprintf (stderr, "No tracks found!\n");
        return MKV_RT_EEXIT;
    }

    printf ("================================== %02ld Tracks ====================================\n", m_tracks.size ());
    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        matroska_track_c *ptrack = ((matroska_track_c *)*i);
        ptrack->dump_track ();
    }
    printf ("==================================================================================\n");

    return MKV_RT_SUCCESS;
}

int matroska_demux_c::track_get_next_frame (matroska_track_c *ptrack, uint8_t *buffer, size_t size) {
    assert((ptrack != NULL) && (buffer != NULL) && size > 0);

    xtr_base_c *extractor = ptrack->mp_extractor;
    if (NULL == extractor)
        return -1;

    return extractor->get_next_frame (buffer, size);
}

int matroska_demux_c::pre_buffer (float percent) {
    matroska_track_c *ptrack = NULL;

    if (mp_video_track) {
        ptrack = mp_video_track;
    } else if (mp_audio_track) {
        ptrack = mp_audio_track;
    }

    if (ptrack)
        return track_pre_buffer (ptrack, percent);

    return -1;
}

int matroska_demux_c::track_pre_buffer (matroska_track_c *ptrack, float percent) {
    assert(ptrack != NULL);
    xtr_base_c *extractor = ptrack->mp_extractor;

    if (NULL == extractor)
        return -1;    

    return extractor->pre_buffer (percent);
}

int matroska_demux_c::track_get_next_frame (matroska_track_c *ptrack, frame_c &frame) {

    assert((ptrack != NULL) && (frame.m_pdata != NULL) && frame.m_size > 0);

    xtr_base_c *extractor = ptrack->mp_extractor;

    if (NULL == extractor)
        return -1;
    
    int read_size = 0;
    int tmp_size = frame.m_size;
    unsigned char* tmp_buffer = frame.m_pdata;
    unsigned char* ret_buffer = frame.m_pdata;

    bool bfirst_frame = true;

    frame_c tmp_frame;
    
    memset (&frame, 0, sizeof (frame));
    do {
        tmp_size -= read_size;
        tmp_buffer += read_size;
        tmp_frame.m_size = tmp_size;
        tmp_frame.m_pdata = tmp_buffer;
        read_size = extractor->get_next_frame (tmp_frame);
        
        if (bfirst_frame) {
            frame.m_pts = tmp_frame.m_pts;
            frame.m_flag = tmp_frame.m_flag;
            bfirst_frame = false;
        }
        frame.m_size += read_size;
        frame.m_duration += tmp_frame.m_duration;
        
    } while ((read_size < tmp_size) && ((tmp_size - read_size) >= extractor->get_next_framesize ()));

    frame.m_pdata = ret_buffer;

    return frame.m_size;
}

int matroska_demux_c::track_get_next_framesize (matroska_track_c *ptrack, size_t &size) {
    assert(ptrack != NULL);
    size = 0;

    xtr_base_c *extractor = ptrack->mp_extractor;
    size = extractor->get_next_framesize ();
    
    return size;
}

void matroska_demux_c::demux_cluster () {
    int upper_lvl_el = 0;
    KaxCluster *cluster;

    mp_l1 = mp_es->FindNextElement(mp_l0->Generic().Context, upper_lvl_el, 0xFFFFFFFFL, true);
    if ((mp_l1 != NULL) && is_id(mp_l1, KaxCluster)) {
        handle_cluster (mp_io_in, mp_es, upper_lvl_el, mp_l1, mp_l2, mp_l3, cluster, m_file_size);
        mp_l1->SkipData(*mp_es, mp_l1->Generic().Context);
        delete mp_l1;
    }
}

MKV_RT matroska_demux_c::GetVideoNextFramesize (size_t &size) {
    size = 0;

    if (!mp_video_track) 
        return MKV_RT_EGENERIC;

    track_get_next_framesize (mp_video_track, size);

    return (size > 0) ? MKV_RT_SUCCESS : MKV_RT_EGENERIC;
}

MKV_RT matroska_demux_c::GetVideoFrame (uint8_t *buffer, size_t &size) {
    size_t _size = size;

    if (NULL == mp_video_track) 
        return MKV_RT_EGENERIC;

    int ret = track_get_next_frame (mp_video_track, buffer, size);
    size = ret;

    return (ret == _size) ? MKV_RT_SUCCESS : MKV_RT_EGENERIC;
}

MKV_RT matroska_demux_c::Demux () {
    pthread_mutex_lock (&m_lock);
    
    // We are in demux loop
    mb_demuxing = true;

    demux_cluster ();

    // Notify demux finished
    pthread_cond_broadcast (&m_demux_cond);

    mb_demuxing = false;

    pthread_mutex_unlock (&m_lock);
    return MKV_RT_SUCCESS;
}

uint64_t matroska_demux_c::Seek (uint64_t timecode) {
    // Notify demux loop to break
    mb_need_to_seek = true;
    reset_demuxer ();

    pthread_mutex_lock (&m_lock);
    if (mb_demuxing) {
        fprintf (stderr, "Demuxing, please wait...\n");
        pthread_cond_wait (&m_demux_cond, &m_lock);
    }

    reset_demuxer ();

    if (m_num_indexes > 0) {
        int i = 0;
        for (; i<m_num_indexes; i++) {
            if (mp_indexes[i].m_timecode > timecode)
                break;
        }
        
        if (i>0)
            i--;
        
        if (i>=0) {
            m_seek_position = mp_indexes[i].m_pos + 36;
            m_seek_time = mp_indexes[i].m_timecode;

            fprintf (stderr, "Seek to pos[%lld], old_timecode[%lld] - timecode[%lld]\n", m_seek_position, timecode, m_seek_time);
            mp_es->I_O().setFilePointer (m_seek_position, seek_beginning);
            mb_need_to_seek = false;
            
            pthread_mutex_unlock (&m_lock);

            m_seek_time = timecode;
            mb_skip_to_timecode = true;
            
            return mp_indexes[i+1].m_timecode;
        }
    }

    pthread_mutex_unlock (&m_lock);

    return timecode;
}

void matroska_demux_c::reset_demuxer () {
    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        matroska_track_c *ptrack = ((matroska_track_c *)*i);
        xtr_base_c *extractor = ptrack->mp_extractor;

        if (NULL != extractor)
            extractor->reset ();
    }
}

MKV_RT matroska_demux_c::Open (const string &filename) {
    int upper_lvl_el;

    EbmlStream *es = NULL;
    mm_io_c *in = NULL;
    KaxCluster *cluster;

    fprintf (stderr, "%s ready to run..., pid = %u, ppid = %u\n", __FUNCTION__, (uint32_t)getpid(), (uint32_t)pthread_self());

    // open input file
    try {
        mp_io_in = new mm_file_io_c(filename);
    } catch (...) {
        printf("Error: Couldn't open input file %s (%s).\n", filename.c_str(), strerror(errno));
        return MKV_RT_EGENERIC;
    }

	in = mp_io_in;
    in->setFilePointer(0, seek_end);
    m_file_size = in->getFilePointer();
    in->setFilePointer(0, seek_beginning);

    try {
        mp_es = new EbmlStream(*in);
		es = mp_es;

        // Find the EbmlHead element. Must be the first one.
        mp_l0 = es->FindNextID(EbmlHead::ClassInfos, 0xFFFFFFFFL);
        if (mp_l0 == NULL) {
            printf("No EBML head found.");
            delete es;

            return MKV_RT_EGENERIC;
        }
        handle_ebml_head(mp_l0, in, es);
        mp_l0->SkipData(*es, mp_l0->Generic().Context);
        delete mp_l0;
        printf("handle_ebml_head");
        while (1) {
            // Next element must be a segment
            mp_l0 = es->FindNextID(KaxSegment::ClassInfos, 0xFFFFFFFFFFFFFFFFLL);
            if (mp_l0 == NULL) {
                printf("No segment/level 0 element found.");
                delete mp_es;
                delete mp_io_in;
                return MKV_RT_EEXIT;
            }
            if (is_id(mp_l0, KaxSegment)) {
                // if (mp_l0->GetSize() == -1)
                //     show_element(mp_l0, 0, "Segment, size unknown");
                // else
                //     show_element(mp_l0, 0, "Segment, size " LLD, mp_l0->GetSize());
                break;
            }
            //show_element(mp_l0, 0, "Next level 0 element is not a segment but %s", typeid(*mp_l0).name());

            mp_l0->SkipData(*es, mp_l0->Generic().Context);
            delete mp_l0;
        }

        upper_lvl_el = 0;
        // We've got our segment, so let's find the tracks
        mp_l1 = es->FindNextElement(mp_l0->Generic().Context, upper_lvl_el, 0xFFFFFFFFL, true);
        while ((mp_l1 != NULL) && (upper_lvl_el <= 0)) {
            if (is_id(mp_l1, KaxInfo)) {
				printf("KaxInfo \n");
                handle_info (in, es, upper_lvl_el, mp_l1, mp_l2, mp_l3);
			}
            
			else if (is_id(mp_l1, KaxTracks)) {
				printf("KaxTracks \n");
                handle_tracks (in, es, upper_lvl_el, mp_l1, mp_l2, mp_l3, mp_l4);
			}

            else if (is_id(mp_l1, KaxSeekHead)) {
				printf("KaxSeekHead \n");
                handle_seek_head (in, es, upper_lvl_el, mp_l1, mp_l2, mp_l3);
            }
            
            else if (is_id(mp_l1, KaxCluster)) {
				printf("KaxCluster \n");
                static_cast<KaxCluster *>(mp_l1)->SetParent (*static_cast<KaxSegment *>(mp_l0));                
                m_first_cluster_pos = static_cast<KaxCluster *>(mp_l1)->GetPosition ();
                handle_cluster (in, es, upper_lvl_el, mp_l1, mp_l2, mp_l3, cluster, m_file_size);
                return MKV_RT_SUCCESS;
            } else if (is_id(mp_l1, KaxCues)) {
                printf("KaxCues \n");
                handle_cues (in, es, upper_lvl_el, mp_l1, mp_l2, mp_l3, mp_l4, mp_l5);
            }
            // Weee! Attachments!
            else if (is_id(mp_l1, KaxAttachments)) {
				printf("KaxAttachments \n");
                handle_attachments (in, es, upper_lvl_el, mp_l1, mp_l2, mp_l3);
            }

            //else if (is_id(mp_l1, KaxChapters))
            //    //handle(chapters);
            //    //show_element(mp_l1, 1, "Chapters");

            //// Let's handle some TAGS.
            //else if (is_id(mp_l1, KaxTags))
            //    //handle(tags);
            //    show_element(mp_l1, 1, "Tags");
            printf("demux_in_parent before \n");
            if (!demux_in_parent(mp_l0)) {
                delete mp_l1;
                break;
            }
            printf("demux_in_parent after \n");
            if (upper_lvl_el > 0) {
                upper_lvl_el--;
                if (upper_lvl_el > 0)
                    break;
                delete mp_l1;
                mp_l1 = mp_l2;
                continue;

            } else if (upper_lvl_el < 0) {
                upper_lvl_el++;
                if (upper_lvl_el < 0)
                    break;
            }

            mp_l1->SkipData(*es, mp_l1->Generic().Context);
            delete mp_l1;
            mp_l1 = es->FindNextElement(mp_l0->Generic().Context, upper_lvl_el, 0xFFFFFFFFL, true);

        } // while (l1 != NULL)

        return MKV_RT_SUCCESS;
    } catch (...) {
        printf("Caught exception");
        delete in;

        return MKV_RT_EGENERIC;
    }

    return MKV_RT_EEXIT;
}

MKV_RT matroska_demux_c::Close () {
    // Notify demux loop to break
    mb_require_to_close = true;
    reset_demuxer ();

    pthread_mutex_lock (&m_lock);

    if (mb_demuxing) {
        fprintf (stderr, "Demuxing, wait...\n");
        pthread_cond_wait (&m_demux_cond, &m_lock);
    }

    fprintf (stderr, "We are ready to close!\n");
    for (vector<matroska_track_c *>::iterator i=m_tracks.begin(); i<m_tracks.end(); i++) {
        matroska_track_c *ptrack = (matroska_track_c *)(*i);
        fprintf (stderr, "%s %d!\n", __FUNCTION__, __LINE__);

        if (ptrack->mp_codec_private)
            delete[] ptrack->mp_codec_private;
        fprintf (stderr, "%s %d!\n", __FUNCTION__, __LINE__);

        close_extractor (ptrack->mp_extractor);            
        if (ptrack->mp_track)
            delete ptrack->mp_track;
        fprintf (stderr, "%s %d!\n", __FUNCTION__, __LINE__);

        delete ptrack;
        fprintf (stderr, "%s %d!\n", __FUNCTION__, __LINE__);

    }

    fprintf (stderr, "%s %d!\n", __FUNCTION__, __LINE__);

    if (mp_l0) delete mp_l0;

    fprintf (stderr, "%s %d!\n", __FUNCTION__, __LINE__);

    delete mp_es;
    delete mp_io_in;

    fprintf (stderr, "%s %d!\n", __FUNCTION__, __LINE__);

    pthread_mutex_unlock (&m_lock);

    fprintf (stderr, "%s %d!\n", __FUNCTION__, __LINE__);

	return MKV_RT_SUCCESS;
}
