/*
   mkvextract -- extract tracks from Matroska files into other files

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   $Id: xtr_avc.h 3461 2007-01-12 14:44:59Z mosu $

   extracts tracks from Matroska files into other files

   Written by Matt Rice <topquark@sluggy.net>.
*/

#ifndef __XTR_AVC_H
#define __XTR_AVC_H

#include "os.h"

#include "xtr_base.h"

class xtr_avc_c: public xtr_base_c {
 public:
    int nal_size_size;

 public:
    xtr_avc_c(const string &_codec_id, int64_t _tid, track_spec_t &tspec);

    ~xtr_avc_c () {
    }

    virtual void create_file(xtr_base_c *_master, matroska_track_c *ptrack);

    virtual void create_file(xtr_base_c *_master, KaxTrackEntry &track);

    virtual void handle_frame(memory_cptr &frame, KaxBlockAdditions *additions,
                              int64_t timecode, int64_t duration, int64_t bref,
                              int64_t fref, bool keyframe, bool discardable,
                              bool references_valid);

    int write_nal(const binary *data, int &pos, int data_size,
                  int nal_size_size);

    virtual const char *get_container_name() {
        return "AVC/h.264 elementary stream";
    };
};

#endif
