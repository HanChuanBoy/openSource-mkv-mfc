#ifndef __MATROSKA_TRACK_H
#define __MATROSKA_TRACK_H

#include "matroska_common.h"

#include <string>

using namespace std;

typedef enum matroska_track_type {
    MATROSKA_TRACK_TYPE_UNKNOWN = 0,
    MATROSKA_TRACK_TYPE_VIDEO,
    MATROSKA_TRACK_TYPE_AUDIO,
    MATROSKA_TRACK_TYPE_SUBTITLE,
    MATROSKA_TRACK_TYPE_BUTTONS,
    MATROSKA_TRACK_TYPE_INVALID
} matroska_track_type_t;

#define track_type2name(x) ((x==MATROSKA_TRACK_TYPE_VIDEO)?"video": \
                           (x==MATROSKA_TRACK_TYPE_AUDIO)?"audio": \
                           (x==MATROSKA_TRACK_TYPE_SUBTITLE?"subtitle":"unknown"))

class xtr_base_c;

class matroska_base_track_c {
 public:
    matroska_base_track_c () {}
    virtual ~matroska_base_track_c () {}
    virtual void dump_track () = 0;
};

class matroska_video_track_c : public matroska_base_track_c {
 public:
    matroska_video_track_c () :
        mb_interlaced_flag(false),
        m_pixel_width(0),
        m_pixel_height(0),
        m_display_width(0),
        m_display_height(0),
        mb_h264_track(false),
        m_h264_length_size(0),
        m_h264_profile(0),
        m_h264_level(0),
        mb_deinterlaced(false)
        {
        }

    ~matroska_video_track_c ()
        {
        }

    void dump_track () {
        printf ("\t\tWidth   : %d\n", m_pixel_width);
        printf ("\t\tHeight  : %d\n", m_pixel_height);
    }

 public:
    bool             mb_interlaced_flag;
    uint32_t         m_pixel_width;
    uint32_t         m_pixel_height;
    uint32_t         m_display_width;
    uint32_t         m_display_height;
    float            m_frame_rate;

    bool             mb_h264_track;
    uint32_t         m_h264_length_size;
    uint32_t         m_h264_profile;
    uint32_t         m_h264_level;

    bool             mb_deinterlaced;
};

class matroska_audio_track_c : public matroska_base_track_c {
 public:
    matroska_audio_track_c () :
        m_sampling_freq(0),
        m_out_sampling_freq(0),
        m_channels(0),
        m_bits_per_sample(0)
        {
        }

    ~matroska_audio_track_c ()
        {
        }

    void dump_track () {
        printf ("\t\tFreqency: %f\n", m_sampling_freq);
        printf ("\t\tChannels: %d\n", m_channels);
        printf ("\t\tbps     : %d\n", m_bits_per_sample);
    }

 public:
    float            m_sampling_freq;
    float            m_out_sampling_freq;
    uint32_t         m_channels;
    uint32_t         m_bits_per_sample;

};

class matroska_subtitle_track_c : public matroska_base_track_c {
 public:
    matroska_subtitle_track_c () :
        m_type(0),
        m_width(0),
        m_height(0)
        {
        }

    ~matroska_subtitle_track_c ()
        {
        }

 public:
    uint8_t         m_type;
    uint32_t        m_width;
    uint32_t        m_height;
    uint32_t        m_palette[16];
    uint32_t        m_custom_colors;
    uint32_t        m_colors[4];
};


class matroska_track_c {
 public:
    matroska_track_c () :
        m_track_type(MATROSKA_TRACK_TYPE_UNKNOWN),
        mp_track(NULL),
        mp_extractor(NULL),
        m_track_num(0),
        m_track_uid(0),
        m_default_duration(0),
        m_timecode_scale(0),
        mp_codec_private(NULL),
        m_codec_private_len (0),
        mp_cache_buffer(NULL),
        m_cache_buffer_size(4*1024*1024-1024),
        mb_default_flag(false),
        mb_enabled_flag(false),
        m_compress_algo(0),
        m_last_pts(0)
        {
        }

    ~matroska_track_c ()
        {
        }
    
    void dump_track () {
        printf ("Track %02d : %s track.\n", m_track_num, track_type2name(m_track_type));
        printf ("\tDuration : %lld!\n", m_default_duration);
        printf ("\tTimecode : %f!\n", m_timecode_scale);
        printf ("\tLanguage : %s\n", m_language.c_str ());
        printf ("\tCodecID  : %s\n", m_codec_id.c_str ());
        printf ("\tDefault  : %s\n", mb_default_flag?"yes":"no");
        printf ("\tEnabled  : %s\n", mb_enabled_flag?"yes":"no");
        printf ("\tCodecPriv: Length %d\n", m_codec_private_len);

        mp_track->dump_track ();
    }

    float TrackTimecodeScale () {
        return m_timecode_scale;
    }

    uint8_t* TrackDSI (size_t &size) {
        size = m_codec_private_len;
        return mp_codec_private;
    }

    uint32_t TrackDuration () {
        return m_default_duration;
    }

    bool IsH264Track () {
        if (m_track_type != MATROSKA_TRACK_TYPE_VIDEO)
            return false;
        
        matroska_video_track_c *pvideo = (matroska_video_track_c *)mp_track;
        return pvideo->mb_h264_track;
    }

    uint32_t H264LengthSize () {
        if (m_track_type != MATROSKA_TRACK_TYPE_VIDEO)
            return false;

        matroska_video_track_c *pvideo = (matroska_video_track_c *)mp_track;
        return pvideo->m_h264_length_size;
    }

    uint32_t H264Level () {
        if (m_track_type != MATROSKA_TRACK_TYPE_VIDEO)
            return false;

        matroska_video_track_c *pvideo = (matroska_video_track_c *)mp_track;

        return pvideo->m_h264_level;
    }

    uint32_t H264Profile () {
        if (m_track_type != MATROSKA_TRACK_TYPE_VIDEO)
            return false;

        matroska_video_track_c *pvideo = (matroska_video_track_c *)mp_track;

        return pvideo->m_h264_profile;
    }

    uint32_t TrackWidth () {
        if (m_track_type != MATROSKA_TRACK_TYPE_VIDEO)
            return false;

        matroska_video_track_c *pvideo = (matroska_video_track_c *)mp_track;

        return pvideo->m_pixel_width;
    }

    uint32_t TrackHeight () {
        if (m_track_type != MATROSKA_TRACK_TYPE_VIDEO)
            return false;

        matroska_video_track_c *pvideo = (matroska_video_track_c *)mp_track;

        return pvideo->m_pixel_height;
    }


    bool TrackDeinterlaced () {
        if (m_track_type != MATROSKA_TRACK_TYPE_VIDEO)
            return false;

        matroska_video_track_c *pvideo = (matroska_video_track_c *)mp_track;

        return pvideo->mb_deinterlaced;
    }

    float TrackSamplingFreq () {
        if (m_track_type != MATROSKA_TRACK_TYPE_AUDIO)
            return -1.0;

        matroska_audio_track_c *paudio = (matroska_audio_track_c *)mp_track;

        return paudio->m_sampling_freq;
    }

    float TrackOutSamplingFreq () {
        if (m_track_type != MATROSKA_TRACK_TYPE_AUDIO)
            return -1.0;

        matroska_audio_track_c *paudio = (matroska_audio_track_c *)mp_track;

        return paudio->m_out_sampling_freq;
    }

    uint32_t TrackChannels () {
        if (m_track_type != MATROSKA_TRACK_TYPE_AUDIO)
            return 0;

        matroska_audio_track_c *paudio = (matroska_audio_track_c *)mp_track;
        
        return paudio->m_channels;
    }

    uint32_t TrackBPS () {
        if (m_track_type != MATROSKA_TRACK_TYPE_AUDIO)
            return 0;

        matroska_audio_track_c *paudio = (matroska_audio_track_c *)mp_track;

        return paudio->m_bits_per_sample;
    }

    void SetCacheBuffer (uint8_t *pbuf, uint32_t size) {
        fprintf (stderr, "%s %d: buf = 0x%p, size = %d!\n", __FUNCTION__, __LINE__, pbuf, size);
        mp_cache_buffer = pbuf;
        m_cache_buffer_size = size;
    }

 public:
    matroska_track_type_t         m_track_type;
    matroska_base_track_c         *mp_track;
    xtr_base_c                    *mp_extractor;

    uint32_t         m_track_num;
    uint32_t         m_track_uid;

    uint64_t         m_default_duration;
    float            m_timecode_scale;
    
    string           m_track_name;
    string           m_language;
    string           m_codec_id;
    string           m_codec_name;

    uint8_t          *mp_codec_private;
    uint32_t         m_codec_private_len;

    uint8_t          *mp_cache_buffer;
    uint32_t         m_cache_buffer_size;

    bool             mb_default_flag;
    bool             mb_enabled_flag;
    uint32_t         m_compress_algo;

    int64_t          m_last_pts;
};

#endif

