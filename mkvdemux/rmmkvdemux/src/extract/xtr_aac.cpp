/*
   mkvextract -- extract tracks from Matroska files into other files

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   $Id: xtr_aac.cpp 3132 2005-10-22 10:12:17Z mosu $

   extracts tracks from Matroska files into other files

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#include "aac_common.h"
#include "common.h"
#include "commonebml.h"
#include "matroska.h"

#include "xtr_aac.h"

xtr_aac_c::xtr_aac_c(const string &_codec_id,
                     int64_t _tid,
                     track_spec_t &tspec):
  xtr_base_c(_codec_id, _tid, tspec),
  channels(0), id(0), profile(0), srate_idx(0) {
}

void
xtr_aac_c::create_file(xtr_base_c *_master,
                       KaxTrackEntry &track) {
    int sfreq;

    xtr_base_c::create_file(_master, track);

    channels = kt_get_a_channels(track);
    sfreq = (int)kt_get_a_sfreq(track);

    if (codec_id == MKV_A_AAC) {
        KaxCodecPrivate *priv;
        int output_sfreq;
        bool is_sbr;

        priv = FINDFIRST(&track, KaxCodecPrivate);
        if (NULL == priv)
            mxerror("Track " LLD " with the CodecID '%s' is missing the \"codec "
                    "private\" element and cannot be extracted.\n", tid,
                    codec_id.c_str());

        output_sfreq = 0;
        is_sbr = false;
        if (!parse_aac_data(priv->GetBuffer(), priv->GetSize(), profile, channels,
                            sfreq, output_sfreq, is_sbr))
            mxerror("Track " LLD " with the CodecID '%s' contains invalid \"codec "
                    "private\" data for AAC.\n", tid, codec_id.c_str());
        id = 0;

    } else {
        // A_AAC/MPEG4/MAIN
        // 0123456789012345
        if (codec_id[10] == '4')
            id = 0;
        else if (codec_id[10] == '2')
            id = 1;
        else
            mxerror("Track ID " LLD " has an unknown AAC type.\n", tid);

        if (!strcmp(&codec_id[12], "MAIN"))
            profile = 0;
        else if (!strcmp(&codec_id[12], "LC") ||
                 (strstr(&codec_id[12], "SBR") != NULL))
            profile = 1;
        else if (!strcmp(&codec_id[12], "SSR"))
            profile = 2;
        else if (!strcmp(&codec_id[12], "LTP"))
            profile = 3;
        else
            mxerror("Track ID " LLD " has an unknown AAC type.\n", tid);
    }

    if (92017 <= sfreq)
        srate_idx = 0;
    else if (75132 <= sfreq)
        srate_idx = 1;
    else if (55426 <= sfreq)
        srate_idx = 2;
    else if (46009 <= sfreq)
        srate_idx = 3;
    else if (37566 <= sfreq)
        srate_idx = 4;
    else if (27713 <= sfreq)
        srate_idx = 5;
    else if (23004 <= sfreq)
        srate_idx = 6;
    else if (18783 <= sfreq)
        srate_idx = 7;
    else if (13856 <= sfreq)
        srate_idx = 8;
    else if (11502 <= sfreq)
        srate_idx = 9;
    else if (9391 <= sfreq)
        srate_idx = 10;
    else
        srate_idx = 11;
}

void
xtr_aac_c::create_file(xtr_base_c *_master,
                       matroska_track_c  *ptrack) {
    int sfreq;
    fprintf (stderr, "%s %d\n", __FUNCTION__, __LINE__);

    try {
        out = new mm_ringbuffer_io_c(ptrack->mp_cache_buffer, ptrack->m_cache_buffer_size);
    } catch(...) {
        mxerror("Failed to create the file '%s': %d (%s)\n", file_name.c_str(),
                errno, strerror(errno));
    }
    fprintf (stderr, "%s %d\n", __FUNCTION__, __LINE__);

    default_duration = ptrack->m_default_duration;

    if (NULL == ptrack->mp_codec_private)
        mxerror("Track %02d CodecPrivate is too small.\n", ptrack->m_track_num);

    channels = ptrack->TrackChannels ();
    sfreq = (int)ptrack->TrackSamplingFreq ();
    fprintf (stderr, "%s %d\n", __FUNCTION__, __LINE__);


    if (codec_id == MKV_A_AAC) {
        int output_sfreq;
        bool is_sbr;
        fprintf (stderr, "%s %d\n", __FUNCTION__, __LINE__);


        binary *buf = ptrack->mp_codec_private; //priv->GetBuffer();
        fprintf (stderr, "%s %d\n", __FUNCTION__, __LINE__);


        output_sfreq = 0;
        is_sbr = false;
        if (!parse_aac_data(ptrack->mp_codec_private, ptrack->m_codec_private_len, profile, channels, sfreq, output_sfreq, is_sbr))
            fprintf (stderr, "Track " LLD " with the CodecID '%s' contains invalid \"codec "
                     "private\" data for AAC.\n", tid, codec_id.c_str());
        fprintf (stderr, "%s %d\n", __FUNCTION__, __LINE__);


        id = 0;
    } else {
        // A_AAC/MPEG4/MAIN
        // 0123456789012345
        if (codec_id[10] == '4')
            id = 0;
        else if (codec_id[10] == '2')
            id = 1;
        else
            fprintf (stderr, "Track ID " LLD " has an unknown AAC type.\n", tid);

        if (!strcmp(&codec_id[12], "MAIN"))
            profile = 0;
        else if (!strcmp(&codec_id[12], "LC") ||
                 (strstr(&codec_id[12], "SBR") != NULL))
            profile = 1;
        else if (!strcmp(&codec_id[12], "SSR"))
            profile = 2;
        else if (!strcmp(&codec_id[12], "LTP"))
            profile = 3;
        else
            fprintf (stderr, "Track ID " LLD " has an unknown AAC type.\n", tid);
    }

    if (92017 <= sfreq)
        srate_idx = 0;
    else if (75132 <= sfreq)
        srate_idx = 1;
    else if (55426 <= sfreq)
        srate_idx = 2;
    else if (46009 <= sfreq)
        srate_idx = 3;
    else if (37566 <= sfreq)
        srate_idx = 4;
    else if (27713 <= sfreq)
        srate_idx = 5;
    else if (23004 <= sfreq)
        srate_idx = 6;
    else if (18783 <= sfreq)
        srate_idx = 7;
    else if (13856 <= sfreq)
        srate_idx = 8;
    else if (11502 <= sfreq)
        srate_idx = 9;
    else if (9391 <= sfreq)
        srate_idx = 10;
    else
        srate_idx = 11;
}


void
xtr_aac_c::handle_frame(memory_cptr &frame,
                        KaxBlockAdditions *additions,
                        int64_t timecode,
                        int64_t duration,
                        int64_t bref,
                        int64_t fref,
                        bool keyframe,
                        bool discardable,
                        bool references_valid) {
    char adts[56 / 8];
    int len;

    // Recreate the ADTS headers. What a fun. Like runing headlong into
    // a solid wall. But less painful. Well such is life, you know.
    // But then again I've just seen a beautiful girl walking by my
    // window, and suddenly the world is a bright place. Everything's
    // a matter of perspective. And if I didn't enjoy writing even this
    // code then I wouldn't do it at all. So let's get to it!

    // sync word, 12 bits
    adts[0] = 0xff;
    adts[1] = 0xf0;

    // ID, 1 bit
    adts[1] |= id << 3;
    // layer: 2 bits = 00

    // protection absent: 1 bit = 1 (ASSUMPTION!)
    adts[1] |= 1;

    // profile, 2 bits
    adts[2] = profile << 6;

    // sampling frequency index, 4 bits
    adts[2] |= srate_idx << 2;

    // private, 1 bit = 0 (ASSUMPTION!)

    // channels, 3 bits
    adts[2] |= (channels & 4) >> 2;
    adts[3] = (channels & 3) << 6;

    // original/copy, 1 bit = 0(ASSUMPTION!)

    // home, 1 bit = 0 (ASSUMPTION!)

    // copyright id bit, 1 bit = 0 (ASSUMPTION!)

    // copyright id start, 1 bit = 0 (ASSUMPTION!)

    // frame length, 13 bits
    len = frame->get_size() + 7;
    adts[3] |= len >> 11;
    adts[4] = (len >> 3) & 0xff;
    adts[5] = (len & 7) << 5;

    // adts buffer fullness, 11 bits, 0x7ff = VBR (ASSUMPTION!)
    adts[5] |= 0x1f;
    adts[6] = 0xfc;

    // number of raw frames, 2 bits, 0 (meaning 1 frame) (ASSUMPTION!)

    int frame_size = 0;
    uint8_t *p_frame_buf = NULL;
    
    // Write the ADTS header and the data itself.
    frame_size += out->write(adts, 56 / 8);
    frame_size += out->write(frame->get(), frame->get_size());

    p_frame_buf = (uint8_t *)out->getFilePointer();
    
    frame_c f;
    
    f.m_pts = timecode;
    f.m_duration = duration;
    f.m_pdata = p_frame_buf;
    f.m_size = frame_size;
    f.m_flag = FRAME_FLAG_FRAME_START | FRAME_FLAG_PTS_VALID;

    // Always pus the new frame to the back of the deque
    p_frame_cache->push_back (&f);
}
