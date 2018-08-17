/*
   mkvextract -- extract tracks from Matroska files into other files

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   $Id: xtr_avc.cpp 3436 2007-01-07 20:13:06Z mosu $

   extracts tracks from Matroska files into other files

   Written by Matt Rice <topquark@sluggy.net>.
*/

#include "common.h"
#include "commonebml.h"

#include "xtr_avc.h"

#include "matroska_demux.h"
#include "matroska_track.h"

static const binary start_code[4] = { 0x00, 0x00, 0x00, 0x01 };

xtr_avc_c::xtr_avc_c(const string &_codec_id,
                     int64_t _tid,
                     track_spec_t &tspec):
    xtr_base_c(_codec_id, _tid, tspec) {
}

int xtr_avc_c::write_nal(const binary *data,
                     int &pos,
                     int data_size,
                     int nal_size_size) {
    int i, ret = 0;
    int nal_size = 0;

    for (i = 0; i < nal_size_size; ++i)
        nal_size = (nal_size << 8) | data[pos++];

    if ((pos + nal_size) > data_size)
        mxerror("Track " LLD ": nal too big\n", tid);

    ret += out->write(start_code, 4);
    ret += out->write(data+pos, nal_size);
    pos += nal_size;
    
    return ret;
}

void xtr_avc_c::create_file(xtr_base_c *_master,
                       KaxTrackEntry &track) {

    xtr_base_c::create_file(_master, track);

    KaxCodecPrivate *priv = FINDFIRST(&track, KaxCodecPrivate);
    if (NULL == priv)
        mxerror("Track " LLD " with the CodecID '%s' is missing the \"codec "
                "private\" element and cannot be extracted.\n", tid,
                codec_id.c_str());

    if (priv->GetSize() < 6)
        mxerror("Track " LLD " CodecPrivate is too small.\n", tid);

    binary *buf = priv->GetBuffer();
    nal_size_size = 1 + (buf[4] & 3);
  
    int profile, level;
    int pos = 6, numsps = buf[5] & 0x1f, numpps;
	
    profile = buf[1];
    level = buf[3];
//    for (i = 0; (i < numsps) && (priv->GetSize() > pos); ++i)
//        write_nal(buf, pos, priv->GetSize(), 2);
 
    if (priv->GetSize() <= pos)
        return;

    numpps = buf[pos++];

    printf ("nal_length_size = %d, profile = %d, level = %d, numsps = %d, numpps = %d!\n", nal_size_size, profile, level, numsps, numpps);
//    for (i = 0; (i < numpps) && (priv->GetSize() > pos); ++i)
//        write_nal(buf, pos, priv->GetSize(), 2);
}


void xtr_avc_c::create_file(xtr_base_c *_master, matroska_track_c *ptrack) {
    try {
        out = new mm_ringbuffer_io_c(ptrack->mp_cache_buffer, ptrack->m_cache_buffer_size);
    } catch(...) {
        mxerror("Failed to create the file '%s': %d (%s)\n", file_name.c_str(),
                errno, strerror(errno));
    }


    char filename[32];
    sprintf (filename, "%s.out", codec_id.c_str());
    try {
        dump_out = new mm_file_io_c(filename, MODE_CREATE);
    } catch(...) {
        mxerror("Failed to create the dump file: %s, %d (%s)\n", filename, errno, strerror(errno));
    }

    default_duration = ptrack->m_default_duration;

    if (NULL == ptrack->mp_codec_private || ptrack->m_codec_private_len < 6)
        mxerror("Track %02d CodecPrivate is too small.\n", ptrack->m_track_num);

    binary *buf = ptrack->mp_codec_private; //priv->GetBuffer();
    nal_size_size = 1 + (buf[4] & 3);

    uint8_t *p_frame_buf = (uint8_t *)out->getFilePointer();
    
    int profile, level;
    int i, pos = 6, numsps = buf[5] & 0x1f, numpps;
    int frame_size = 0;

    profile = buf[1];
    level = buf[3];
    for (i = 0; (i < numsps) && (ptrack->m_codec_private_len > pos); ++i)
        frame_size += write_nal(buf, pos, ptrack->m_codec_private_len, 2);

    if (ptrack->m_codec_private_len > pos) {
        numpps = buf[pos++];

        for (i = 0; (i < numpps) && (ptrack->m_codec_private_len > pos); ++i)
            frame_size += write_nal(buf, pos, ptrack->m_codec_private_len, 2);
    }

    printf ("nal_length_size = %d, profile = %d, level = %d, numsps = %d, numpps = %d!\n", nal_size_size, profile, level, numsps, numpps);

    matroska_video_track_c *pvideo = (matroska_video_track_c *)ptrack->mp_track;
    pvideo->mb_h264_track = true;
    pvideo->m_h264_length_size = nal_size_size;
    pvideo->m_h264_profile = profile;
    pvideo->m_h264_level = level;

    frame_c f;
    f.m_pts = 0;
    f.m_duration = default_duration;
    f.m_pdata = p_frame_buf;
    f.m_size = frame_size;
    f.m_flag = FRAME_FLAG_FRAME_START | FRAME_FLAG_PTS_VALID;

    // Always pus the new frame to the back of the deque
    p_frame_cache->push_back (&f);

    // Get the AVC1 data immediately
    delete [] ptrack->mp_codec_private;
    ptrack->mp_codec_private = new uint8_t[frame_size];
    ptrack->m_codec_private_len = frame_size;
    
    get_next_frame (ptrack->mp_codec_private, ptrack->m_codec_private_len);

    if (dump_out)
        dump_out->write (ptrack->mp_codec_private, ptrack->m_codec_private_len);
}


void
xtr_avc_c::handle_frame(memory_cptr &frame,
                        KaxBlockAdditions *additions,
                        int64_t timecode,
                        int64_t duration,
                        int64_t bref,
                        int64_t fref,
                        bool keyframe,
                        bool discardable,
                        bool references_valid) {
    int pos, frame_size = 0;
    uint8_t *p_frame_buf = NULL;
    
    binary *buf = (binary *)frame->get();

    pos = 0;
    p_frame_buf = (uint8_t *)out->getFilePointer();
    
    while (frame->get_size() > pos)
        frame_size += write_nal(buf, pos, frame->get_size(), nal_size_size);
    
    frame_c f;
    
    f.m_pts = timecode;
    f.m_duration = duration;
    f.m_pdata = p_frame_buf;
    f.m_size = frame_size;
    f.m_flag = FRAME_FLAG_FRAME_START | FRAME_FLAG_PTS_VALID;

    // Always pus the new frame to the back of the deque
    p_frame_cache->push_back (&f);
}
