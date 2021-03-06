/*
   mkvextract -- extract tracks from Matroska files into other files

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   $Id: xtr_base.cpp 3469 2007-01-15 19:02:18Z mosu $

   extracts tracks from Matroska files into other files

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#include "os.h"

#include <matroska/KaxTracks.h>
#include <matroska/KaxTrackEntryData.h>
#include <matroska/KaxTrackAudio.h>
#include <matroska/KaxTrackVideo.h>

#include "common.h"
#include "commonebml.h"
#include "matroska.h"

#include "xtr_aac.h"
#include "xtr_avc.h"
#include "xtr_wav.h"
#include "xtr_base.h"
#include "xtr_mpeg1_2.h"

using namespace std;
using namespace libmatroska;

xtr_base_c::xtr_base_c(const string &_codec_id,
                       int64_t _tid,
                       track_spec_t &tspec,
                       const char *_container_name):
    codec_id(_codec_id), file_name(tspec.out_name),
    container_name(NULL == _container_name ? "raw data" : _container_name),
    master(NULL), out(NULL), dump_out(NULL), tid(_tid), default_duration(0), bytes_written(0) {
    p_frame_cache = new frame_cache_c ();
}

xtr_base_c::~xtr_base_c() {
    if (out)
        delete out;
    
    if (dump_out)
        delete dump_out;

    delete p_frame_cache;
}

void
xtr_base_c::create_file(xtr_base_c *_master, matroska_track_c *ptrack) {
    try {
        out = new mm_ringbuffer_io_c(ptrack->mp_cache_buffer, ptrack->m_cache_buffer_size);
    } catch(...) {
        mxerror("Failed to create the file '%s': %d (%s)\n", file_name.c_str(),
                errno, strerror(errno));
    }

    default_duration = ptrack->m_default_duration;
}

void
xtr_base_c::create_file(xtr_base_c *_master,
                        KaxTrackEntry &track) {
    if (NULL != _master)
        mxerror("Cannot write track " LLD " with the CodecID '%s' to the file "
                "'%s' because track " LLD " with the CodecID '%s' is already "
                "being written to the same file.\n", tid, codec_id.c_str(),
                file_name.c_str(), _master->tid, _master->codec_id.c_str());

    try {
        out = new mm_file_io_c(file_name, MODE_CREATE);
    } catch(...) {
        mxerror("Failed to create the file '%s': %d (%s)\n", file_name.c_str(),
                errno, strerror(errno));
    }

    default_duration = kt_get_default_duration(track);
}

void
xtr_base_c::handle_frame(memory_cptr &frame,
                         KaxBlockAdditions *additions,
                         int64_t timecode,
                         int64_t duration,
                         int64_t bref,
                         int64_t fref,
                         bool keyframe,
                         bool discardable,
                         bool references_valid) {
    int frame_size = 0;
    uint8_t *p_frame_buf = (uint8_t *)out->getFilePointer();

    frame_size += out->write(frame->get(), frame->get_size());
    bytes_written += frame->get_size();
    
    frame_c f;
    
    f.m_pts = timecode;
    f.m_duration = duration;
    f.m_pdata = p_frame_buf;
    f.m_size = frame_size;
    f.m_flag = FRAME_FLAG_FRAME_START | FRAME_FLAG_PTS_VALID;

    // Always pus the new frame to the back of the deque
    p_frame_cache->push_back (&f);
}

uint32 xtr_base_c::get_next_frame (void* buffer, size_t size) {
    uint32 ret = 0;
    uint32 _size;

    if (p_frame_cache->size() <= 0)
        return 0;

    frame_c *pframe = (frame_c *)(p_frame_cache->begin());
    assert (pframe != NULL);

    _size = (size < pframe->m_size) ? size : pframe->m_size;
    ret = out->read (buffer, _size);

    if (dump_out)
        dump_out->write (buffer, ret);

    pframe->m_size -= ret;
    if (pframe->m_size <= 0) {
        p_frame_cache->pop_front ();
    }

    return ret;
}

uint32 xtr_base_c::pre_buffer (float percent) {
    fprintf (stderr, "%s %s %d!\n", __FILE__, __FUNCTION__, __LINE__);

    return out->pre_buffer (percent);
}

uint32 xtr_base_c::get_next_frame (frame_c &frame) {
    uint32 ret = 0;
    uint32 _size;

    uint32 size = frame.m_size;
    uint8* buffer = frame.m_pdata;

    if (p_frame_cache->size() <= 0)
        return 0;

    frame_c *pframe = (frame_c *)(p_frame_cache->begin());
    assert (pframe != NULL);

    _size = (size < pframe->m_size) ? size : pframe->m_size;
    ret = out->read (buffer, _size);

    frame.m_pts = pframe->m_pts;
    frame.m_duration = pframe->m_duration;
    frame.m_size = ret;
    frame.m_flag = pframe->m_flag;

    //fprintf (stderr, "%s: m_flag = %d\n", __FUNCTION__, frame.m_flag);
    if (dump_out)
        dump_out->write (buffer, ret);
    
    // Reset the frame flag
    pframe->m_pts = -1;
    pframe->m_flag = 0;
    pframe->m_size -= ret;
    if (pframe->m_size <= 0) {
        frame.m_flag |= FRAME_FLAG_FRAME_END;
        p_frame_cache->pop_front ();
    }

    return ret;
}

uint32 xtr_base_c::get_next_framesize () {
    if (p_frame_cache->size() <= 0)
        return 0;

    frame_c *pframe = p_frame_cache->begin();

    //fprintf (stderr, "Frame: address = %p, size = %d!\n", pframe->m_pdata, pframe->m_size);

    return pframe->m_size;
}

void
xtr_base_c::finish_track() {

}

void
xtr_base_c::finish_file() {
    if (out)
        out->flush ();
}

void
xtr_base_c::headers_done() {
}

xtr_base_c *
xtr_base_c::create_extractor(const string &new_codec_id,
                             int64_t new_tid,
                             track_spec_t &tspec) {
    // Raw format
    if (1 == tspec.extract_raw)
        return new xtr_base_c(new_codec_id, new_tid, tspec);
    else if (2 == tspec.extract_raw)
        return new xtr_fullraw_c(new_codec_id, new_tid, tspec);

    // Audio formats
    else if (new_codec_id == MKV_A_AC3)
        return new xtr_base_c(new_codec_id, new_tid, tspec, "Dolby Digital (AC3)");
    else if (starts_with_case(new_codec_id, "A_MPEG/L"))
        return new xtr_base_c(new_codec_id, new_tid, tspec, "MPEG-1 Audio Layer 2/3");
    else if (new_codec_id == MKV_A_DTS)
        return new xtr_base_c(new_codec_id, new_tid, tspec, "Digital Theater System (DTS)");
    else if (new_codec_id == MKV_A_PCM)
        return new xtr_wav_c(new_codec_id, new_tid, tspec);
    else if (starts_with_case(new_codec_id, "A_AAC"))
        return new xtr_aac_c(new_codec_id, new_tid, tspec);

    // Video formats
    else if (new_codec_id == MKV_V_MSCOMP)
        return new xtr_base_c(new_codec_id, new_tid, tspec);
    else if (new_codec_id == MKV_V_MPEG4_AVC)
        return new xtr_avc_c(new_codec_id, new_tid, tspec);
    else if ((new_codec_id == MKV_V_MPEG1) ||
             (new_codec_id == MKV_V_MPEG2))
        return new xtr_mpeg1_2_video_c(new_codec_id, new_tid, tspec);

    return NULL;
}

void
xtr_fullraw_c::create_file(xtr_base_c *_master,
                           KaxTrackEntry &track) {
    KaxCodecPrivate *priv;

    xtr_base_c::create_file(_master, track);

    priv = FINDFIRST(&track, KaxCodecPrivate);

    if ((NULL != priv) && (0 != priv->GetSize()))
        out->write(priv->GetBuffer(), priv->GetSize());
}

void
xtr_fullraw_c::handle_codec_state(memory_cptr &codec_state) {
    out->write(codec_state->get(), codec_state->get_size());
}
