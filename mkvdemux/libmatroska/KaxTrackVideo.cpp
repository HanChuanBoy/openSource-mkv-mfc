/****************************************************************************
** libmatroska : parse Matroska files, see http://www.matroska.org/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Steve Lhomme.  All rights reserved.
**
** This file is part of libmatroska.
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
	\version \$Id: KaxTrackVideo.cpp,v 1.6 2003/06/05 18:34:19 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxTrackVideo.h"

// sub elements
#include "KaxContexts.h"

START_LIBMATROSKA_NAMESPACE

const EbmlSemantic KaxTrackVideo_ContextList[11] =
{
	EbmlSemantic(false, true, KaxVideoFlagInterlaced::ClassInfos),
	EbmlSemantic(false, true, KaxVideoStereoMode::ClassInfos),
	EbmlSemantic(true , true, KaxVideoPixelWidth::ClassInfos),
	EbmlSemantic(true , true, KaxVideoPixelHeight::ClassInfos),
	EbmlSemantic(false, true, KaxVideoDisplayWidth::ClassInfos),
	EbmlSemantic(false, true, KaxVideoDisplayHeight::ClassInfos),
	EbmlSemantic(false, true, KaxVideoDisplayUnit::ClassInfos),
	EbmlSemantic(false, true, KaxVideoAspectRatio::ClassInfos),
	EbmlSemantic(false, true, KaxVideoColourSpace::ClassInfos),
	EbmlSemantic(false, true, KaxVideoGamma::ClassInfos),
	EbmlSemantic(false, true, KaxVideoFrameRate::ClassInfos),
};

const EbmlSemanticContext KaxTrackVideo_Context = EbmlSemanticContext(countof(KaxTrackVideo_ContextList), KaxTrackVideo_ContextList, &KaxTrackEntry_Context, *GetKaxGlobal_Context, &KaxTrackVideo::ClassInfos);
const EbmlSemanticContext KaxVideoFlagInterlaced_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoFlagInterlaced::ClassInfos);
const EbmlSemanticContext KaxVideoStereoMode_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoStereoMode::ClassInfos);
const EbmlSemanticContext KaxVideoPixelWidth_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoPixelWidth::ClassInfos);
const EbmlSemanticContext KaxVideoPixelHeight_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoPixelHeight::ClassInfos);
const EbmlSemanticContext KaxVideoDisplayWidth_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoDisplayWidth::ClassInfos);
const EbmlSemanticContext KaxVideoDisplayHeight_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoDisplayHeight::ClassInfos);
const EbmlSemanticContext KaxVideoDisplayUnit_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoDisplayUnit::ClassInfos);
const EbmlSemanticContext KaxVideoAspectRatio_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoAspectRatio::ClassInfos);
const EbmlSemanticContext KaxVideoColourSpace_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoColourSpace::ClassInfos);
const EbmlSemanticContext KaxVideoGamma_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoGamma::ClassInfos);
const EbmlSemanticContext KaxVideoFrameRate_Context = EbmlSemanticContext(0, NULL, &KaxTrackVideo_Context, *GetKaxGlobal_Context, &KaxVideoFrameRate::ClassInfos);

EbmlId KaxTrackVideo_TheId         (0xE0, 1);
EbmlId KaxVideoFlagInterlaced_TheId(0x9A, 1);
EbmlId KaxVideoStereoMode_TheId    (0x53B9, 2);
EbmlId KaxVideoPixelWidth_TheId    (0xB0, 1);
EbmlId KaxVideoPixelHeight_TheId   (0xBA, 1);
EbmlId KaxVideoDisplayWidth_TheId  (0x54B0, 2);
EbmlId KaxVideoDisplayHeight_TheId (0x54BA, 2);
EbmlId KaxVideoDisplayUnit_TheId   (0x54B2, 2);
EbmlId KaxVideoAspectRatio_TheId   (0x54B3, 1);
EbmlId KaxVideoColourSpace_TheId   (0x2EB524, 3);
EbmlId KaxVideoGamma_TheId         (0x2FB523, 3);
EbmlId KaxVideoFrameRate_TheId     (0x2383E3, 3);

const EbmlCallbacks KaxTrackVideo::ClassInfos(KaxTrackVideo::Create, KaxTrackVideo_TheId, "TrackAudio", KaxTrackVideo_Context);
const EbmlCallbacks KaxVideoFlagInterlaced::ClassInfos(KaxVideoFlagInterlaced::Create, KaxVideoFlagInterlaced_TheId, "VideoFlagInterlaced", KaxVideoFlagInterlaced_Context);
const EbmlCallbacks KaxVideoStereoMode::ClassInfos(KaxVideoStereoMode::Create, KaxVideoStereoMode_TheId, "VideoStereoMode", KaxVideoStereoMode_Context);
const EbmlCallbacks KaxVideoPixelWidth::ClassInfos(KaxVideoPixelWidth::Create, KaxVideoPixelWidth_TheId, "VideoPixelWidth", KaxVideoPixelWidth_Context);
const EbmlCallbacks KaxVideoPixelHeight::ClassInfos(KaxVideoPixelHeight::Create, KaxVideoPixelHeight_TheId, "VideoPixelHeight", KaxVideoPixelHeight_Context);
const EbmlCallbacks KaxVideoDisplayWidth::ClassInfos(KaxVideoDisplayWidth::Create, KaxVideoDisplayWidth_TheId, "VideoDisplayWidth", KaxVideoDisplayWidth_Context);
const EbmlCallbacks KaxVideoDisplayHeight::ClassInfos(KaxVideoDisplayHeight::Create, KaxVideoDisplayHeight_TheId, "VideoDisplayHeight", KaxVideoDisplayHeight_Context);
const EbmlCallbacks KaxVideoDisplayUnit::ClassInfos(KaxVideoDisplayUnit::Create, KaxVideoDisplayUnit_TheId, "VideoDisplayUnit", KaxVideoDisplayUnit_Context);
const EbmlCallbacks KaxVideoAspectRatio::ClassInfos(KaxVideoAspectRatio::Create, KaxVideoAspectRatio_TheId, "VideoAspectRatio", KaxVideoAspectRatio_Context);
const EbmlCallbacks KaxVideoColourSpace::ClassInfos(KaxVideoColourSpace::Create, KaxVideoColourSpace_TheId, "VideoColourSpace", KaxVideoColourSpace_Context);
const EbmlCallbacks KaxVideoGamma::ClassInfos(KaxVideoGamma::Create, KaxVideoGamma_TheId, "VideoGamma", KaxVideoGamma_Context);
const EbmlCallbacks KaxVideoFrameRate::ClassInfos(KaxVideoFrameRate::Create, KaxVideoFrameRate_TheId, "VideoFrameRate", KaxVideoFrameRate_Context);

KaxTrackVideo::KaxTrackVideo()
	:EbmlMaster(KaxTrackVideo_Context)
{}

uint32 KaxVideoFrameRate::RenderData(IOCallback & output, bool bSaveDefault)
{
	assert(false); // no you are not allowed to use this element !
	return -1;
}

END_LIBMATROSKA_NAMESPACE
