#ifndef __RMMKV_H
#define __RMMKV_H

#include "../../rmdef/rmdef.h"
#include "../../rmlibcw/include/rmfile.h"

#include "../extract/matroska_common.h"
#include "../extract/matroska_demux.h"
#include "../extract/matroska_track.h"

struct _RMmkvClient {
	matroska_demux_c *pdemux;
};

struct _RMmkvTrack {
	matroska_track_c *ptrack;
};

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
	RM_MKV_AUDIO_AAC_TRACK,
	RM_MKV_AUDIO_MP3_TRACK,
	RM_MKV_AUDIO_AC3_TRACK,
	RM_MKV_AUDIO_PCM_TRACK,
	RM_MKV_AUDIO_RPCM_TRACK,
	RM_MKV_AUDIO_MPEG2_TRACK,
	RM_MKV_AUDIO_WMA_TRACK,
	RM_MKV_AUDIO_WMAPRO_TRACK,
	RM_MKV_VIDEO_ONLY_TRACK,
	RM_MKV_MULTICAST_VIDEO_MP4_TRACK,
	RM_MKV_MULTICAST_AUDIO_AAC_TRACK,
	RM_MKV_MULTICAST_RTSP_VIDEO_MP4_TRACK,
	RM_MKV_MULTICAST_RTSP_AUDIO_AAC_TRACK,
	RM_MKV_VIDEO_H264_TRACK
} RMmkvTrackType;

typedef struct _RMmkvClient *ExternalRMmkvClient;
typedef struct _RMmkvTrack  *ExternalRMmkvTrack;

#endif

