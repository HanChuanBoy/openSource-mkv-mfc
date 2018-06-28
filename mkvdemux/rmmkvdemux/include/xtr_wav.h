/*
   mkvextract -- extract tracks from Matroska files into other files

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   $Id: xtr_wav.h 3461 2007-01-12 14:44:59Z mosu $

   extracts tracks from Matroska files into other files

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#ifndef __XTR_WAV_H
#define __XTR_WAV_H

#include "os.h"

#include "xtr_base.h"

struct riff_struct 
{
    uint8_t id[4];   /* RIFF */
    uint32_t len;
    uint8_t wave_id[4]; /* WAVE */
};


struct chunk_struct 
{
	uint8_t id[4];
	uint32_t len;
};

struct common_struct 
{
	uint16_t wFormatTag;
	uint16_t wChannels;
	uint32_t dwSamplesPerSec;
	uint32_t dwAvgBytesPerSec;
	uint16_t wBlockAlign;
	uint16_t wBitsPerSample;  /* Only for PCM */
};

struct wave_header 
{
	struct riff_struct   riff;
	struct chunk_struct  format;
	struct common_struct common;
	struct chunk_struct  data;
};

class xtr_wav_c: public xtr_base_c {
 public:
    wave_header wh;

 public:
    xtr_wav_c(const string &_codec_id, int64_t _tid, track_spec_t &tspec);

    virtual void create_file(xtr_base_c *_master, matroska_track_c *ptrack);

    virtual void create_file(xtr_base_c *_master, KaxTrackEntry &track);
    virtual void finish_file();
};

#endif
