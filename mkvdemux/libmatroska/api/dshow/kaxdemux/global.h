
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

// libebml includes
#include "EbmlTypes.h"
#include "EbmlHead.h"
#include "EbmlSubHead.h"
#include "EbmlStream.h"
#include "EbmlContexts.h"

// libmatroska includes
#include "KaxConfig.h"
#include "KaxBlock.h"
#include "KaxSegment.h"
#include "KaxContexts.h"
#include "KaxTracks.h"
#include "KaxInfo.h"
#include "KaxInfoData.h"
#include "KaxCluster.h"
#include "KaxClusterData.h"
#include "KaxTrackAudio.h"
#include "KaxTrackVideo.h"


struct KaxVideoTrackInfoStruct {
	uint8		Interlaced;
	uint8		StereoMode;
	uint32		PixelWidth;
	uint32		PixelHeight;
	uint32		DisplayWidth;
	uint32		DisplayHeight;
	uint32		DisplayUnit;
	uint32		AspectRatio;
};

struct KaxAudioTrackInfoStruct {
	float		SamplingFreq;
	uint32		Channels;
	void		*Position;
	uint32		BitDepth;			// optional for PrE
	uint32		SubTrackName;
	uint32		SubTrackISRC;
};

struct KaxTrackInfoStruct {
	uint8		Number;
	uint8		Type;
	uint8		Enabled;
	uint8		Default;
	uint8		Lacing;
	uint32		MinCache;				// possible range ??
	uint32		MaxCache;				// possible range ??
	std::wstring Name;
	std::string	Language;
	binary		*CodecID;
	binary		*CodecPrivate;
	uint32		CodecPrivateLen;
	std::wstring CodecName;
	uint8		CodecDecodeAll;
	uint32		Overlay;				// possible range ??
	union {
		KaxVideoTrackInfoStruct	*pVideoInfo;
		KaxAudioTrackInfoStruct	*pAudioInfo;
	};
};

#endif __GLOBAL_H__
