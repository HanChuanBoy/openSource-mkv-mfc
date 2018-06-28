/*
   mkvextract -- extract tracks from Matroska files into other files

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   $Id: xtr_wav.cpp 3126 2005-10-19 16:52:55Z mosu $

   extracts tracks from Matroska files into other files

   Written by Moritz Bunkus <moritz@bunkus.org>
   and Steve Lhomme <steve.lhomme@free.fr>.
*/

#include "os.h"

#include "common.h"
#include "commonebml.h"
#include "xtr_wav.h"

#include "matroska_demux.h"
#include "matroska_track.h"

xtr_wav_c::xtr_wav_c(const string &_codec_id,
                     int64_t _tid,
                     track_spec_t &tspec):
  xtr_base_c(_codec_id, _tid, tspec) {

  memset(&wh, 0, sizeof(wave_header));
}

void
xtr_wav_c::create_file(xtr_base_c *_master,
                       matroska_track_c  *ptrack) {
    int channels, sfreq, bps;

    try {
        out = new mm_ringbuffer_io_c(ptrack->mp_cache_buffer, ptrack->m_cache_buffer_size);
    } catch(...) {
        mxerror("Failed to create the file '%s': %d (%s)\n", file_name.c_str(),
                errno, strerror(errno));
    }

    default_duration = ptrack->m_default_duration;

    if (NULL == ptrack->mp_codec_private)
        mxerror("Track %02d CodecPrivate is too small.\n", ptrack->m_track_num);

    channels = ptrack->TrackChannels ();
    sfreq = (int)ptrack->TrackSamplingFreq ();
    bps = ptrack->TrackBPS ();

    if (-1 == bps)
        mxerror("Track " LLD " with the CodecID '%s' is missing the \"bits per "
                "second (bps)\" element and cannot be extracted.\n",
                tid, codec_id.c_str());

    memcpy(&wh.riff.id, "RIFF", 4);
    memcpy(&wh.riff.wave_id, "WAVE", 4);
    memcpy(&wh.format.id, "fmt ", 4);
    put_uint32_le(&wh.format.len, 16);
    put_uint16_le(&wh.common.wFormatTag, 1);
    put_uint16_le(&wh.common.wChannels, channels);
    put_uint32_le(&wh.common.dwSamplesPerSec, sfreq);
    put_uint32_le(&wh.common.dwAvgBytesPerSec, channels * sfreq * bps / 8);
    put_uint16_le(&wh.common.wBlockAlign, 4);
    put_uint16_le(&wh.common.wBitsPerSample, bps);
    memcpy(&wh.data.id, "data", 4);

    uint8_t *p_frame_buf = (uint8_t *)out->getFilePointer();
    int frame_size = 0;

    frame_size += out->write(&wh, sizeof(wave_header));

    frame_c f;
    f.m_pts = 0;
    f.m_duration = default_duration;
    f.m_pdata = p_frame_buf;
    f.m_size = frame_size;
    f.m_flag = FRAME_FLAG_FRAME_START | FRAME_FLAG_PTS_VALID;

    // Always pus the new frame to the back of the deque
    p_frame_cache->push_back (&f);
}

void
xtr_wav_c::create_file(xtr_base_c *_master,
                       KaxTrackEntry &track) {
    int channels, sfreq, bps;

    channels = kt_get_a_channels(track);
    sfreq = (int)kt_get_a_sfreq(track);
    bps = kt_get_a_bps(track);
    if (-1 == bps)
        mxerror("Track " LLD " with the CodecID '%s' is missing the \"bits per "
                "second (bps)\" element and cannot be extracted.\n",
                tid, codec_id.c_str());

    xtr_base_c::create_file(_master, track);

    memcpy(&wh.riff.id, "RIFF", 4);
    memcpy(&wh.riff.wave_id, "WAVE", 4);
    memcpy(&wh.format.id, "fmt ", 4);
    put_uint32_le(&wh.format.len, 16);
    put_uint16_le(&wh.common.wFormatTag, 1);
    put_uint16_le(&wh.common.wChannels, channels);
    put_uint32_le(&wh.common.dwSamplesPerSec, sfreq);
    put_uint32_le(&wh.common.dwAvgBytesPerSec, channels * sfreq * bps / 8);
    put_uint16_le(&wh.common.wBlockAlign, 4);
    put_uint16_le(&wh.common.wBitsPerSample, bps);
    memcpy(&wh.data.id, "data", 4);

    out->write(&wh, sizeof(wave_header));
}

void
xtr_wav_c::finish_file() {
    //out->setFilePointer(0);
    //put_uint32_le(&wh.riff.len, bytes_written + 36);
    //put_uint32_le(&wh.data.len, bytes_written);
    //out->write(&wh, sizeof(wave_header));
}
