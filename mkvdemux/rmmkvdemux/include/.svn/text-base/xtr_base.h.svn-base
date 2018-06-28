/*
   mkvextract -- extract tracks from Matroska files into other files

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   $Id: xtr_base.h 3469 2007-01-15 19:02:18Z mosu $

   extracts tracks from Matroska files into other files

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#ifndef __XTR_BASE_H
#define __XTR_BASE_H

#include "os.h"

#include <string>
#include <deque>
#include <matroska/KaxTracks.h>

#include <pthread.h>

#include "common.h"

#include "matroska_common.h"
#include "matroska_demux.h"
#include "matroska_track.h"

#include "frame_cache.h"

using namespace std;
using namespace libmatroska;


class xtr_base_c {
 public:
    string codec_id, file_name, container_name;
    xtr_base_c *master;
    mm_io_c *out;
    mm_io_c *dump_out;
    int64_t tid;
    int64_t default_duration;

    int64_t bytes_written;
    
    frame_cache_c     *p_frame_cache;

 public:
    xtr_base_c(const string &_codec_id, int64_t _tid, track_spec_t &tspec,
               const char *_container_name = NULL);
    virtual ~xtr_base_c();

    virtual void create_file(xtr_base_c *_master, KaxTrackEntry &track);
    virtual void create_file(xtr_base_c *_master, matroska_track_c *ptrack);
    virtual void handle_frame(memory_cptr &frame, KaxBlockAdditions *additions,
                              int64_t timecode, int64_t duration, int64_t bref,
                              int64_t fref, bool keyframe, bool discardable,
                              bool references_valid);

    virtual uint32 get_next_frame (void* buffer, size_t size);
    virtual uint32 get_next_frame (frame_c &frame);

    virtual uint32 pre_buffer (float percent);

    virtual uint32 get_next_framesize ();

    virtual void handle_codec_state(memory_cptr &codec_state) {
    };
    virtual void finish_track();
    virtual void finish_file();

    virtual void reset () {
        out->flush ();
        p_frame_cache->clear ();
    }

    virtual void headers_done();

    virtual const char *get_container_name() {
        return container_name.c_str();
    };

    static xtr_base_c *create_extractor(const string &new_codec_id,
                                        int64_t new_tid, track_spec_t &tspec);

};

class xtr_fullraw_c : public xtr_base_c {
 public:
    xtr_fullraw_c(const string &_codec_id, int64_t _tid, track_spec_t &tspec):
        xtr_base_c(_codec_id, _tid, tspec) {}
    virtual void create_file(xtr_base_c *_master, KaxTrackEntry &track);
    virtual void handle_codec_state(memory_cptr &codec_state);
};

#endif
