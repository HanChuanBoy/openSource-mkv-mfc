#ifndef __RMMKVAPI_H__
#define __RMMKVAPI_H__

#ifndef ALLOW_OS_CODE
#define ALLOW_OS_CODE 1
#endif //ALLOW_OS_CODE

#include <pthread.h>

#include "../../rmdef/rmdef.h"
#include "../../rmlibcw/include/rmfile.h"
#include "../../rmlibcw/include/rmlibcw.h"

#include "matroska_common.h"
#include "matroska_demux.h"
#include "matroska_track.h"
#include "frame_cache.h"

typedef enum {
    DEMUX_STATE_FIRST = 0,
    DEMUX_STATE_READY,
    DEMUX_STATE_PLAY,
    DEMUX_STATE_STOP,
    DEMUX_STATE_PAUSE,
    DEMUX_STATE_FINISH,
    DEMUX_STATE_LAST
} DemuxState;

#define MKV_AU_START  1
#define MKV_CTS_VALID 2
#define MKV_AU_END    4

typedef struct _MKVTrackCacheBuffer {
    RMuint8 *buffer;
    RMuint32 buffer_size;
} MKVTrackCacheBuffer;

typedef struct _RMmkvClient {
	matroska_demux_c *pdemux;

    pthread_t demux_thread;
    RMbool running;
    DemuxState state;

    MKVTrackCacheBuffer video_cache;
    MKVTrackCacheBuffer audio_cache;

} RMmkvClient, *ExternalRMmkvClient;

typedef struct _RMmkvTrack {
	matroska_demux_c *pdemux;
	matroska_track_c *ptrack;
} RMmkvTrack, *ExternalRMmkvTrack;

typedef struct tagRMmkvSample {
	RMuint32 size;
	RMuint8 *buf;
	RMuint32 DTS_LSB;
	RMuint32 DTS_MSB;
	RMuint32 CTS_LSB;
	RMuint32 CTS_MSB;
	RMuint32 flags;
} RMmkvSample;

typedef enum {
    RM_MKV_TYPE_TRACK_NOT_DEFINED = 7563,
	RM_MKV_VIDEO_MPEG4_TRACK,
	RM_MKV_VIDEO_AVI_TRACK,
	RM_MKV_VIDEO_DIVX3_TRACK,
	RM_MKV_VIDEO_WMV9_TRACK,
	RM_MKV_VIDEO_MPEG1_TRACK,
	RM_MKV_VIDEO_MPEG2_TRACK,
	RM_MKV_AUDIO_AAC_TRACK,
	RM_MKV_AUDIO_MP3_TRACK,
	RM_MKV_AUDIO_AC3_TRACK,
	RM_MKV_AUDIO_PCM_TRACK,
	RM_MKV_AUDIO_RPCM_TRACK,
	RM_MKV_AUDIO_DTS_TRACK,
	RM_MKV_AUDIO_MPEG2_TRACK,
	RM_MKV_AUDIO_WMA_TRACK,
	RM_MKV_AUDIO_WMAPRO_TRACK,
	RM_MKV_VIDEO_ONLY_TRACK,
	RM_MKV_VIDEO_H264_TRACK
} RMmkvTrackType;


/*
 * Matroska Codec IDs. Strings.
 */

#define MATROSKA_CODEC_ID_V_VFW_FOURCC   "V_MS/VFW/FOURCC"
#define MATROSKA_CODEC_ID_V_UNCOMPRESSED "V_UNCOMPRESSED"
#define MATROSKA_CODEC_ID_V_MPEG4_SP     "V_MPEG4/ISO/SP"
#define MATROSKA_CODEC_ID_V_MPEG4_ASP    "V_MPEG4/ISO/ASP"
#define MATROSKA_CODEC_ID_V_MPEG4_AP     "V_MPEG4/ISO/AP"
#define MATROSKA_CODEC_ID_V_MPEG4_AVC    "V_MPEG4/ISO/AVC"
#define MATROSKA_CODEC_ID_V_MSMPEG4V3    "V_MPEG4/MS/V3"
#define MATROSKA_CODEC_ID_V_MPEG1        "V_MPEG1"
#define MATROSKA_CODEC_ID_V_MPEG2        "V_MPEG2"
#define MATROSKA_CODEC_ID_V_MPEG2        "V_MPEG2"
#define MATROSKA_CODEC_ID_V_REAL_RV10    "V_REAL/RV10"
#define MATROSKA_CODEC_ID_V_REAL_RV20    "V_REAL/RV20"
#define MATROSKA_CODEC_ID_V_REAL_RV30    "V_REAL/RV30"
#define MATROSKA_CODEC_ID_V_REAL_RV40    "V_REAL/RV40"
#define MATROSKA_CODEC_ID_V_MJPEG        "V_MJPEG"
#define MATROSKA_CODEC_ID_V_THEORA       "V_THEORA"

#define MATROSKA_CODEC_ID_A_MPEG1_L1     "A_MPEG/L1"
#define MATROSKA_CODEC_ID_A_MPEG1_L2     "A_MPEG/L2"
#define MATROSKA_CODEC_ID_A_MPEG1_L3     "A_MPEG/L3"
#define MATROSKA_CODEC_ID_A_PCM_INT_BE   "A_PCM/INT/BIG"
#define MATROSKA_CODEC_ID_A_PCM_INT_LE   "A_PCM/INT/LIT"
#define MATROSKA_CODEC_ID_A_PCM_FLOAT    "A_PCM/FLOAT/IEEE"
#define MATROSKA_CODEC_ID_A_AC3          "A_AC3"
#define MATROSKA_CODEC_ID_A_DTS          "A_DTS"
#define MATROSKA_CODEC_ID_A_VORBIS       "A_VORBIS"
#define MATROSKA_CODEC_ID_A_ACM          "A_MS/ACM"
#define MATROSKA_CODEC_ID_A_AAC          "A_AAC"
#define MATROSKA_CODEC_ID_A_REAL_14_4    "A_REAL/14_4"
#define MATROSKA_CODEC_ID_A_REAL_28_8    "A_REAL/28_8"
#define MATROSKA_CODEC_ID_A_REAL_COOK    "A_REAL/COOK"
#define MATROSKA_CODEC_ID_A_REAL_SIPR    "A_REAL/SIPR"
#define MATROSKA_CODEC_ID_A_REAL_RALF    "A_REAL/RALF"
#define MATROSKA_CODEC_ID_A_REAL_ATRC    "A_REAL/ATRC"

#define MATROSKA_CODEC_ID_S_TEXT_UTF8    "S_TEXT/UTF8"
#define MATROSKA_CODEC_ID_S_TEXT_SSA     "S_TEXT/SSA"
#define MATROSKA_CODEC_ID_S_TEXT_ASS     "S_TEXT/ASS"
#define MATROSKA_CODEC_ID_S_TEXT_USF     "S_TEXT/USF"
#define MATROSKA_CODEC_ID_S_UTF8         "S_UTF8"        /* deprecated */
#define MATROSKA_CODEC_ID_S_SSA          "S_SSA"         /* deprecated */
#define MATROSKA_CODEC_ID_S_ASS          "S_ASS"         /* deprecated */
#define MATROSKA_CODEC_ID_S_VOBSUB       "S_VOBSUB"

/**
   Opens a mkv client on a local file.

   @param filename file to open.
   @param mkvc returned client.
     @return RM_ERROR if the url is not valid.
           RM_ERROR_NOT_MKV if the url is not a mkv url.  
*/
RM_EXTERN_C RMstatus RMOpenFileMKV(const char *filename, ExternalRMmkvClient *mkvc);


/**
   Set track cache buffer.

   @param mkvc client 
   @param vcache video track cache.
   @param acache audio track cache.
*/
RM_EXTERN_C RMstatus RMSetAVTrackCacheMKV(ExternalRMmkvClient mkvc, MKVTrackCacheBuffer *vcache, MKVTrackCacheBuffer *acache);

/**
   Opens a mkv client on a local file.

   @param file a file already opened by application. This allows application to implement its own file operations.
   @param mkvc returned client.
   @return RM_ERROR if the url is not valid.
           RM_ERROR_NOT_MKV if the url is not a mkv url.  
*/
RM_EXTERN_C RMstatus RMOpenExternalFileMKV(RMfile file, ExternalRMmkvClient *mkvc);

/**
   Seeks to the time in seconds in the movie. 
   
   @param mkvc client 
   @param time  time in seconds from the beginning to seek to.
   @return time in second we really seek.
*/
RM_EXTERN_C RMreal RMSeekMKV(ExternalRMmkvClient mkvc, RMreal time);

/**
   Seeks to the time in milliseconds in the movie. 
   
   @param mkvc client 
   @param time_ms  time in milliseconds from the beginning to seek to.
   @return time in milliseconds we really seeked to.
*/
RM_EXTERN_C RMuint32 RMSeekMKVmillisec(ExternalRMmkvClient mkvc, RMuint32 time_ms);

/**
   Prints debug output concerning the opened mkv presentation.

   @param mkvc client
*/
RM_EXTERN_C void RMPrintMKV(ExternalRMmkvClient mkvc);


/**
   Tries to get a video track ID contained in the mkv presentation.

   @param mkvc client 
   @return trackID if there is a valid video track
           0 if there is no video track.
*/
RM_EXTERN_C RMuint32 RMGetMKVVideoTrackID(ExternalRMmkvClient mkvc);


/**
   Tries to get an audio track ID contained in the mkv presentation.

   @param mkvc client 
   @return trackID if there is a valid audio track
           0 if there is no audio track.
*/
RM_EXTERN_C RMuint32 RMGetMKVAudioTrackID(ExternalRMmkvClient mkvc);


/**
   Opens a mkv track from the presentation.

   @param mkvc client 
   @param trackID       
   @return valid track handle
           NULL if the trackID is not valid.
*/
RM_EXTERN_C ExternalRMmkvTrack RMOpenMKVTrack(ExternalRMmkvClient mkvc, RMuint32 trackID);


/**
   Closes the mkv track.

   @param mkvc  client
   @param mkvt  track to close
*/
RM_EXTERN_C void RMCloseMKVTrack(ExternalRMmkvClient mkvc, 
				 ExternalRMmkvTrack mkvt);


/**
   Closes the mkv client.

   @param mkvc client
*/
RM_EXTERN_C void RMCloseMKV(ExternalRMmkvClient mkvc);


/* MKV TRACK INTERFACE */

/**
   Prints debug output from the track.

   @param mkvt track
*/
RM_EXTERN_C void RMPrintMKVTrack(ExternalRMmkvTrack mkvt);


/**
   Dumps the track bitstream in a file. The dumped data does not
   contained the DSI.

   @param mkvt  
   @param dumpname is a pointer to a buffer of bytes that may contain
   a traditional representation of a filename (char*) but also a 
   unicode representation.
*/
RM_EXTERN_C void RMDumpMKVTrack(ExternalRMmkvTrack mkvt, 
				const RMuint8 *dumpname);


/**
   Gets the timescale for this track. This is the number of CTS units
   per second.

   @param mkvt track
   @return timescale
*/
RM_EXTERN_C RMuint64 RMGetMKVTrackTimeScale(ExternalRMmkvTrack mkvt);


/**
   Gets the size in bytes of the next sample to get.

   @param mkvt track
   @return size of the next sample.
*/
RM_EXTERN_C RMuint32 RMGetNextMKVSampleSize(ExternalRMmkvTrack mkvt);


/**
   Gets the next sample. The sample buffer must be allocated before
   calling this function. It should be allocated fot maxSize. If the
   sample size is lesser than maxSize, then the sample contains the
   complete Access Unit. Otherwise the sample contains only maxSize of
   the AU. In case of an incomplete AU, the next call will send the
   rest of the AU. There are flags in the sample to tell if this
   sample contained the beginning of an AU and if it is also the end
   of one.

   @param mkvt track
   @param mkvs sample 
   @param maxSize size of the allocated buffer in sample      
   @return TRUE if there the sample returned is valid.
           FALSE if the sample is not valid. (normally end of track)
*/
RM_EXTERN_C RMbool RMGetNextMKVSample(ExternalRMmkvTrack mkvt, 
				      RMmkvSample *mkvs, 
				      RMuint32 maxSize);



/**
   Works as the get next sample but gets only the I frames.

   @param mkvt track
   @param mkvs sample 
   @param maxSize size of the allocated buffer in sample      
   @return TRUE if there the sample returned is valid.
           FALSE if the sample is not valid. (normally end of track)
*/
RM_EXTERN_C RMbool RMGetNextMKVRandomAccessSample(ExternalRMmkvTrack mkvt, RMmkvSample *mkvs, RMuint32 maxSize);

     
/**
   Works as the get next sample but gets only the I frames and it goes backward.
   
   @param mkvt track
   @param mkvs sample 
   @param maxSize size of the allocated buffer in sample      
   @return TRUE if there the sample returned is valid.
           FALSE if the sample is not valid. (normally end of track)
*/
RM_EXTERN_C RMbool RMGetPrevMKVRandomAccessSample(ExternalRMmkvTrack mkvt, RMmkvSample *mkvs, RMuint32 maxSize);

/**
   Gets the DSI from the track. The buffer is allocated in this
   function. It is also freed when the track is closed.

   @param mkvt track
   @param size  size of the DSI
   @return dsi buffer
*/
RM_EXTERN_C RMuint8 *RMGetMKVTrackDSI(ExternalRMmkvTrack mkvt, 
				      RMuint32 *size);

/**
   Returns the duration of the track in timescale unit.

   @param mkvt  
   @return track duration
*/
RM_EXTERN_C RMuint32 RMGetMKVTrackDuration(ExternalRMmkvTrack mkvt);


/**
   returns the number of samples in this track.

   @param mkvt  
   @return 
*/
RM_EXTERN_C RMuint32 RMGetMKVTrackSampleCount(ExternalRMmkvTrack mkvt);

/**
   Returns the amount of random access sample in the track.

   @param mkvt  
   @return 
*/
RM_EXTERN_C RMuint32 RMGetMKVTrackRandomAccessSampleCount(ExternalRMmkvTrack mkvt);

/**
   Returns the vop time increment in timescale unit.

   @param mkvt  
   @return 
*/
RM_EXTERN_C RMuint32 RMGetMKVTrackVopTimeIncrement(ExternalRMmkvTrack mkvt);


RM_EXTERN_C RMstatus RMGetMKVTrackDeinterlaced(ExternalRMmkvTrack mkvt, RMbool *deinterlaced);


RM_EXTERN_C RMuint32 RMGetMKVNumberOfVideoTracks(ExternalRMmkvClient mkvc);
RM_EXTERN_C RMuint32 RMGetMKVNumberOfAudioTracks(ExternalRMmkvClient mkvc);
RM_EXTERN_C RMuint32 RMGetMKVNumberOfSPUTracks(ExternalRMmkvClient mkvc);
RM_EXTERN_C RMuint32 RMGetMKVNumberOfSubtitleTracks(ExternalRMmkvClient mkvc);
RM_EXTERN_C RMstatus RMGetMKVVideoTrackIDByIndex(ExternalRMmkvClient mkvc, RMuint32 index, RMuint32 *trackID);
RM_EXTERN_C RMstatus RMGetMKVAudioTrackIDByIndex(ExternalRMmkvClient mkvc, RMuint32 index, RMuint32 *trackID);
RM_EXTERN_C RMstatus RMGetMKVSPUTrackIDByIndex(ExternalRMmkvClient mkvc, RMuint32 index, RMuint32 *trackID);
RM_EXTERN_C RMstatus RMGetMKVSubtitleTrackIDByIndex(ExternalRMmkvClient mkvc, RMuint32 index, RMuint32 *trackID);
RM_EXTERN_C RMstatus RMGetMKVMovieTimescale(ExternalRMmkvClient mkvc, RMuint64 *timescale);
RM_EXTERN_C RMuint32 RMSeekMKVTrack(ExternalRMmkvClient mkvc, RMuint32 time_ms, ExternalRMmkvTrack mkvt);
RM_EXTERN_C RMstatus RMGetMKVTrackWidth(ExternalRMmkvTrack mkvt, RMuint32 *width);
RM_EXTERN_C RMstatus RMGetMKVTrackHeight(ExternalRMmkvTrack mkvt, RMuint32 *height);
RM_EXTERN_C RMstatus RMGetMKVTrackSize(ExternalRMmkvTrack mkvt, RMuint32 *sizeTrack, RMuint32 *sampleCount);
RM_EXTERN_C RMstatus RMGetMKVTrackNextSampleSize(ExternalRMmkvTrack mkvt, RMuint32 *size);
RM_EXTERN_C RMstatus RMGetMKVTrackNextReadPos(ExternalRMmkvTrack mkvt, RMuint64 *nextReadPos);
RM_EXTERN_C RMstatus RMGetMKVTrackStartupDelay(ExternalRMmkvTrack mkvt, RMuint32 *startup_delay);

RM_EXTERN_C RMbool   RMisMKVH264Track(ExternalRMmkvTrack mkvt);
RM_EXTERN_C RMstatus RMGetMKVH264Level(ExternalRMmkvTrack mkvt, RMuint32 *level);
RM_EXTERN_C RMstatus RMGetMKVH264Profile(ExternalRMmkvTrack mkvt, RMuint32 *profile);
RM_EXTERN_C RMstatus RMGetMKVH264LengthSize(ExternalRMmkvTrack mkvt, RMuint32 *lengthSize);

RM_EXTERN_C RMstatus RMGetMKVTrackType(ExternalRMmkvTrack mkvt, RMmkvTrackType *type);
RM_EXTERN_C RMstatus RMGetMKVTrackSampleRate(ExternalRMmkvTrack mkvt, RMuint32 *sampleRate);
RM_EXTERN_C RMstatus RMGetMKVTrackChannelCount(ExternalRMmkvTrack mkvt, RMuint32 *channelCount);
RM_EXTERN_C RMstatus RMGetMKVTrackBitPerSample(ExternalRMmkvTrack mkvt, RMuint32 *bitPerSample);

#endif // __RMMKVAPI_H__
