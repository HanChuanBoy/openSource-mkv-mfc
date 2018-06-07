/****************************************************************************
** libmatroska : parse Matroska files, see http://www.matroska.org/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Steve Lhomme.  All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding an other license may use this file in accordance with 
** the Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.matroska.org/license/qpl/ for QPL licensing information.
** See http://www.matroska.org/license/gpl/ for GPL licensing information.
**
** Contact license@matroska.org if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
/*!
	\file
	\version \$Id: global.h,v 1.6 2003/06/07 13:01:36 robux4 Exp $
	\author Jan-D. Schlenker <myfun @ users.corecodec.org>
	\author Christophe Paris <toffparis @ users.sf.net>
*/

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
#include "KaxSeekHead.h"


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
	uint8		 Number;
	uint8		 Type;
	uint8		 Enabled;
	uint8		 Default;
	uint8		 Lacing;
	uint32		 MinCache;				// possible range ??
	uint32		 MaxCache;				// possible range ??
	std::wstring Name;
	std::string	 Language;
	std::string	 CodecID;
	binary		*CodecPrivate;
	uint32		 CodecPrivateLen;
	std::wstring CodecName;
	uint8		 CodecDecodeAll;
	uint32		 Overlay;				// possible range ??
	union {
		KaxVideoTrackInfoStruct	VideoInfo;
		KaxAudioTrackInfoStruct	AudioInfo;
	};
	~KaxTrackInfoStruct()
	{
		if (CodecPrivate != NULL) 
			free(CodecPrivate);
		if (Type == track_audio)
			delete AudioInfo.Position;
	}
};

#endif __GLOBAL_H__
