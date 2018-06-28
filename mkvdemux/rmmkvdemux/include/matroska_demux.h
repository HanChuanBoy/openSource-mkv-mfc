#ifndef __MATROSKA_DEMUX_H
#define __MATROSKA_DEMUX_H

#include <string>
#include <vector>
#include <pthread.h>

//#include "chapters.h"
#include "common.h"
#include "commonebml.h"
#include "matroska.h"
#include "mm_io.h"

#include "matroska_common.h"
#include "matroska_track.h"
#include "frame_cache.h"

using namespace std;
using namespace libebml;
using namespace libmatroska;

struct master_sorter_t {
    int m_index;
    int64_t m_pos;

    inline master_sorter_t(int index, int64_t pos):
        m_index(index), m_pos(pos) { }

    inline bool operator <(const master_sorter_t &cmp) const {
        return m_pos < cmp.m_pos;
    }
};

class matroska_index_c {
 public:
    matroska_index_c (uint32_t track_num, uint32_t block_num, uint64_t pos, uint64_t timecode, bool key) :
        m_track_num (track_num),
        m_block_num (block_num),
        m_pos (pos),
        m_timecode (timecode),
        mb_key (key)
        {}

    matroska_index_c () {}

    void set_value (uint64_t timecode, uint32_t track_num, uint32_t block_num, uint64_t pos) {
        m_timecode = timecode;
        m_track_num = track_num;
        m_block_num = block_num;
        m_pos = pos;
    }

    void dump () {
        fprintf (stderr, "Index: track[%02d] block[%02d] position[%lld] timecode[%lld]\n", m_track_num, m_block_num, m_pos, m_timecode);
    }

 public:
    uint32_t            m_track_num;
    uint32_t            m_block_num;
    uint64_t            m_pos;
    uint64_t            m_timecode;
    bool                mb_key;
} ;

class xtr_base_c;
class matroska_demux_c {
 public:
    matroska_demux_c () :
		mp_io_in(NULL),
        mp_es(NULL),
		m_timecode_scale(TIMECODE_SCALE),
        mb_require_to_close(false),
        mb_need_to_seek(false),
        mb_demuxing(false),
        m_seek_position(0),
        m_seek_time(0),
		m_file_size(0),
        m_cues_position(-1),
        m_chapters_position(-1),
        m_tags_position(-1),
		mb_tracks_found(false)
        {
            m_num_tracks = m_num_video_tracks = m_num_audio_tracks = m_num_subtitle_tracks = 0;
            mp_video_track = mp_audio_track = mp_subtitle_track = NULL;
            mp_l0 = mp_l1 = mp_l2 = mp_l3 = mp_l4 = mp_l5 = mp_l6 = NULL;

            pthread_mutex_init (&m_lock, NULL);
            pthread_cond_init (&m_demux_cond, NULL);
        }
    ~matroska_demux_c ()
        {
            m_tracks.clear ();
            if (m_num_indexes > 0 && mp_indexes) {
                delete [] mp_indexes;
                mp_indexes = NULL;
            }
            pthread_cond_destroy (&m_demux_cond);
            pthread_mutex_destroy (&m_lock);
        }

    MKV_RT Open (const string &filename);
    MKV_RT Close ();
    MKV_RT Demux ();

    MKV_RT GetVideoFrame (uint8_t *buffer, size_t &size);
    MKV_RT GetVideoNextFramesize (size_t &size);
    
    MKV_RT PreparePlayback ();

    MKV_RT DumpTracks ();

    uint64_t Seek (uint64_t timecode);

    MKV_RT Eof () {
        //assert (mp_io_in != NULL);
        if (mp_io_in->eof ())
            return MKV_RT_EOF;
        
        return MKV_RT_EGENERIC;
    }

    uint64_t MovieTimecodeScale () {
        return 1000000000/m_timecode_scale;
    }

    uint32_t GetVideoTrackID ();
    uint32_t GetAudioTrackID ();

    matroska_track_c *GetTrackByID (uint32_t trackID);
    matroska_track_c *OpenTrack (matroska_track_c *ptrack);
    MKV_RT CloseTrack (matroska_track_c *ptrack);

 private:
	void read_master(EbmlMaster *m, EbmlStream *es, const EbmlSemanticContext &ctx, int &upper_lvl_el, EbmlElement *&l2); 
	void sort_master(EbmlMaster &m);

    int open_video_track (matroska_track_c *ptrack);
    int open_audio_track (matroska_track_c *ptrack);
    int open_subtitle_track (matroska_track_c *ptrack);
    int open_extractor (matroska_track_c *ptrack);
    int close_extractor (xtr_base_c *extractor);

    matroska_track_c* find_track(uint32_t tnum);    
    matroska_track_c* find_track_by_uid(uint32_t tuid);

    void handle_ebml_head(EbmlElement *l0, mm_io_c *in, EbmlStream *es);

	void handle_info (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3);

	void handle_seek_head (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3);

	void handle_tracks (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, 
                        EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3, EbmlElement *&l4);

	void handle_video_track (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, EbmlElement *&l3, EbmlElement *&l4, matroska_track_c *&ptrack);

	void handle_audio_track (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, EbmlElement *&l3, EbmlElement *&l4, matroska_track_c *&ptrack);

	void handle_cues (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, 
                      EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3, EbmlElement *&l4, EbmlElement *&l5);

	void handle_attachments (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el, EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3);

    void handle_cluster (mm_io_c *&in, EbmlStream *&es, int &upper_lvl_el,
                         EbmlElement *&l1, EbmlElement *&l2, EbmlElement *&l3,
                         KaxCluster *&cluster, int64_t file_size);

    void handle_simpleblock(KaxSimpleBlock &simpleblock, KaxCluster &cluster);
    void handle_blockgroup(KaxBlockGroup &blockgroup, KaxCluster &cluster, uint64_t tc_scale);

    // Demux one cluster
    void demux_cluster ();

    void reset_demuxer ();

 public:
    int track_get_next_frame (matroska_track_c *ptrack, uint8_t *buffer, size_t size);

    int track_get_next_frame (matroska_track_c *ptrack, frame_c &frame);

    int track_get_next_framesize (matroska_track_c *ptrack, size_t &size);

    int track_pre_buffer (matroska_track_c *ptrack, float percent);

    int pre_buffer (float percent);

    uint32_t NumberOfVideoTracks () {
        return m_num_video_tracks;
    }

    uint32_t NumberOfAudioTracks () {
        return m_num_audio_tracks;
    }

    uint32_t NumberOfSubtitleTracks () {
        return m_num_subtitle_tracks;
    }


    /*
     * Get the duration in miniseconds
    */
    uint64_t GetMovieDuration () {
        return m_duration/1000000000; 
    }

    uint32_t GetVideoTrackIDByIndex (uint32_t index);
    uint32_t GetAudioTrackIDByIndex (uint32_t index);
    uint32_t GetSPUTrackIDByIndex (uint32_t index);
    uint32_t GetSubtitleTrackIDByIndex (uint32_t index);

 private:
	mm_io_c                   *mp_io_in;
  	EbmlStream                *mp_es;

    KaxSegment                *mp_segment;
	uint64_t                  m_timecode_scale;
    uint64_t                  m_duration;
    
    bool                      mb_has_seekhead;
    
    /* seek info */
    matroska_index_c          *mp_indexes;
    int32_t                   m_num_indexes;

    bool                      mb_first_cluster_found;
    bool                      mb_skip_to_timecode;
    bool                      mb_skip_for_track;

    bool                      mb_require_to_close;
    bool                      mb_need_to_seek;

    bool                      mb_demuxing;

    int64_t                   m_seek_position;
    uint64_t                  m_seek_time;

    /* tracks */
    uint32_t                  m_num_tracks;
    uint32_t                  m_num_video_tracks;
    uint32_t                  m_num_audio_tracks;
    uint32_t                  m_num_subtitle_tracks;

	uint64_t                  m_file_size;
	uint64_t                  m_current_pos;
    uint64_t                  m_first_cluster_pos;
    uint64_t                  m_first_cluster_timecode;
    bool                      mb_first_cluster_timecode;

    int64_t                   m_cues_position;
    int64_t                   m_chapters_position;
    int64_t                   m_tags_position;

    vector<matroska_track_c*> m_tracks;
	bool                      mb_tracks_found;

    matroska_track_c          *mp_video_track;
    matroska_track_c          *mp_audio_track;
    matroska_track_c          *mp_subtitle_track;

    pthread_mutex_t           m_lock;
    pthread_cond_t            m_demux_cond;

    EbmlElement *mp_l0;
    EbmlElement *mp_l1;
    EbmlElement *mp_l2;
    EbmlElement *mp_l3;
    EbmlElement *mp_l4;
    EbmlElement *mp_l5;
    EbmlElement *mp_l6;
};

#define UTF2STR(s) UTFstring_to_cstrutf8(UTFstring(s)).c_str()

#define demux_in_parent(p) \
  (!p->IsFiniteSize() || \
   (mp_io_in->getFilePointer() < \
    (p->GetElementPosition() + p->HeadSize() + p->GetSize())))

#endif
