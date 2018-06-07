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
	\version \$Id: KaxTracks.cpp,v 1.12 2003/06/06 23:31:30 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxTracks.h"

// sub elements
#include "KaxTrackEntryData.h"
#include "KaxTrackAudio.h"
#include "KaxTrackVideo.h"
#include "KaxContexts.h"

START_LIBMATROSKA_NAMESPACE

const EbmlSemantic KaxTracks_ContextList[1] =
{
	EbmlSemantic(true, false, KaxTrackEntry::ClassInfos),
};

const EbmlSemantic KaxTrackEntry_ContextList[22] =
{
	EbmlSemantic(true , true, KaxTrackNumber::ClassInfos),
	EbmlSemantic(true , true, KaxTrackUID::ClassInfos),
	EbmlSemantic(true , true, KaxTrackType::ClassInfos),
	EbmlSemantic(false, true, KaxTrackFlagEnabled::ClassInfos),
	EbmlSemantic(false, true, KaxTrackFlagDefault::ClassInfos),
	EbmlSemantic(false, true, KaxTrackFlagLacing::ClassInfos),
	EbmlSemantic(true,  true, KaxTrackMinCache::ClassInfos),
	EbmlSemantic(false, true, KaxTrackMaxCache::ClassInfos),
	EbmlSemantic(false, true, KaxTrackDefaultDuration::ClassInfos),
	EbmlSemantic(true,  true, KaxTrackTimecodeScale::ClassInfos),
	EbmlSemantic(false, true, KaxTrackName::ClassInfos),
	EbmlSemantic(false, true, KaxTrackLanguage::ClassInfos),
	EbmlSemantic(true , true, KaxCodecID::ClassInfos),
	EbmlSemantic(false, true, KaxCodecPrivate::ClassInfos),
	EbmlSemantic(false, true, KaxCodecName::ClassInfos),
	EbmlSemantic(false, true, KaxCodecSettings::ClassInfos),
	EbmlSemantic(false, true, KaxCodecInfoURL::ClassInfos),
	EbmlSemantic(false, true, KaxCodecDownloadURL::ClassInfos),
	EbmlSemantic(false, true, KaxCodecDecodeAll::ClassInfos),
	EbmlSemantic(false, true, KaxTrackOverlay::ClassInfos),
	EbmlSemantic(false, true, KaxTrackAudio::ClassInfos),
	EbmlSemantic(false, true, KaxTrackVideo::ClassInfos),
};

const EbmlSemanticContext KaxTracks_Context = EbmlSemanticContext(countof(KaxTracks_ContextList), KaxTracks_ContextList, &KaxSegment_Context, *GetKaxGlobal_Context, &KaxTracks::ClassInfos);
const EbmlSemanticContext KaxTrackEntry_Context = EbmlSemanticContext(countof(KaxTrackEntry_ContextList), KaxTrackEntry_ContextList, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackEntry::ClassInfos);

EbmlId KaxTracks_TheId    (0x1654AE6B, 4);
EbmlId KaxTrackEntry_TheId(0xAE, 1);

const EbmlCallbacks KaxTracks::ClassInfos(KaxTracks::Create, KaxTracks_TheId, "Tracks", KaxTracks_Context);
const EbmlCallbacks KaxTrackEntry::ClassInfos(KaxTrackEntry::Create, KaxTrackEntry_TheId, "TrackEntry", KaxTrackEntry_Context);

KaxTracks::KaxTracks()
	:EbmlMaster(KaxTracks_Context)
{}

KaxTrackEntry::KaxTrackEntry()
	:EbmlMaster(KaxTrackEntry_Context)
	,bGlobalTimecodeScaleIsSet(false)
{}

void KaxTrackEntry::EnableLacing(bool bEnable)
{
	KaxTrackFlagLacing & myLacing = GetChild<KaxTrackFlagLacing>(*this);
	*(static_cast<EbmlUInteger *>(&myLacing)) = bEnable ? 1 : 0;
}

END_LIBMATROSKA_NAMESPACE
