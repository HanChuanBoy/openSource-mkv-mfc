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
	\version \$Id: KaxTrackEntryData.cpp,v 1.6 2003/06/05 20:12:49 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author John Cannon      <spyder2555 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxTrackEntryData.h"
#include "KaxContexts.h"

START_LIBMATROSKA_NAMESPACE

EbmlId KaxTrackNumber_TheId            (0xD7, 1);
EbmlId KaxTrackUID_TheId               (0x73C5, 2);
EbmlId KaxTrackType_TheId              (0x83, 1);
EbmlId KaxTrackFlagEnabled_TheId       (0xB9, 1);
EbmlId KaxTrackFlagDefault_TheId       (0x88, 1);
EbmlId KaxTrackFlagLacing_TheId        (0x9C, 1);
EbmlId KaxTrackMinCache_TheId          (0x6DE7, 2);
EbmlId KaxTrackMaxCache_TheId          (0x6DF8, 2);
EbmlId KaxTrackDefaultDuration_TheId   (0x23E383, 3);
EbmlId KaxTrackTimecodeScale_TheId     (0x23314F, 2);
EbmlId KaxTrackName_TheId              (0x536E, 2);
EbmlId KaxTrackLanguage_TheId          (0x22B59C, 3);
EbmlId KaxCodecID_TheId                (0x86, 1);
EbmlId KaxCodecPrivate_TheId           (0x63A2, 2);
EbmlId KaxCodecName_TheId              (0x258688, 3);
EbmlId KaxCodecSettings_TheId          (0x3A9697, 3);
EbmlId KaxCodecInfoURL_TheId           (0x3B4040, 3);
EbmlId KaxCodecDownloadURL_TheId       (0x26B240, 3);
EbmlId KaxCodecDecodeAll_TheId         (0xAA, 1);
EbmlId KaxTrackOverlay_TheId           (0x6FAB, 2);

const EbmlSemanticContext KaxTrackNumber_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackNumber::ClassInfos);
const EbmlSemanticContext KaxTrackUID_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackUID::ClassInfos);
const EbmlSemanticContext KaxTrackType_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackType::ClassInfos);
const EbmlSemanticContext KaxTrackFlagEnabled_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackFlagEnabled::ClassInfos);
const EbmlSemanticContext KaxTrackFlagDefault_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackFlagDefault::ClassInfos);
const EbmlSemanticContext KaxTrackFlagLacing_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackFlagLacing::ClassInfos);
const EbmlSemanticContext KaxTrackMinCache_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackMinCache::ClassInfos);
const EbmlSemanticContext KaxTrackMaxCache_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackMaxCache::ClassInfos);
const EbmlSemanticContext KaxTrackDefaultDuration_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackDefaultDuration::ClassInfos);
const EbmlSemanticContext KaxTrackTimecodeScale_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackTimecodeScale::ClassInfos);
const EbmlSemanticContext KaxTrackName_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackName::ClassInfos);
const EbmlSemanticContext KaxTrackLanguage_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackLanguage::ClassInfos);
const EbmlSemanticContext KaxCodecID_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxCodecID::ClassInfos);
const EbmlSemanticContext KaxCodecPrivate_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxCodecPrivate::ClassInfos);
const EbmlSemanticContext KaxCodecName_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxCodecName::ClassInfos);
const EbmlSemanticContext KaxCodecSettings_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxCodecSettings::ClassInfos);
const EbmlSemanticContext KaxCodecInfoURL_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxCodecInfoURL::ClassInfos);
const EbmlSemanticContext KaxCodecDownloadURL_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxCodecDownloadURL::ClassInfos);
const EbmlSemanticContext KaxCodecDecodeAll_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxCodecDecodeAll::ClassInfos);
const EbmlSemanticContext KaxTrackOverlay_Context = EbmlSemanticContext(0, NULL, &KaxTracks_Context, *GetKaxGlobal_Context, &KaxTrackOverlay::ClassInfos);

const EbmlCallbacks KaxTrackNumber::ClassInfos(KaxTrackNumber::Create, KaxTrackNumber_TheId, "TrackNumber", KaxTrackNumber_Context);
const EbmlCallbacks KaxTrackUID::ClassInfos(KaxTrackUID::Create, KaxTrackUID_TheId, "TrackUID", KaxTrackUID_Context);
const EbmlCallbacks KaxTrackType::ClassInfos(KaxTrackType::Create, KaxTrackType_TheId, "TrackType", KaxTrackType_Context);
const EbmlCallbacks KaxTrackFlagEnabled::ClassInfos(KaxTrackFlagEnabled::Create, KaxTrackFlagEnabled_TheId, "TrackFlagEnabled", KaxTrackFlagEnabled_Context);
const EbmlCallbacks KaxTrackFlagDefault::ClassInfos(KaxTrackFlagDefault::Create, KaxTrackFlagDefault_TheId, "TrackFlagDefault", KaxTrackFlagDefault_Context);
const EbmlCallbacks KaxTrackFlagLacing::ClassInfos(KaxTrackFlagLacing::Create, KaxTrackFlagLacing_TheId, "TrackFlagLacing", KaxTrackFlagLacing_Context);
const EbmlCallbacks KaxTrackMinCache::ClassInfos(KaxTrackMinCache::Create, KaxTrackMinCache_TheId, "TrackMinCache", KaxTrackMinCache_Context);
const EbmlCallbacks KaxTrackMaxCache::ClassInfos(KaxTrackMaxCache::Create, KaxTrackMaxCache_TheId, "TrackMaxCache", KaxTrackMaxCache_Context);
const EbmlCallbacks KaxTrackDefaultDuration::ClassInfos(KaxTrackDefaultDuration::Create, KaxTrackDefaultDuration_TheId, "TrackDefaultDuration", KaxTrackDefaultDuration_Context);
const EbmlCallbacks KaxTrackTimecodeScale::ClassInfos(KaxTrackTimecodeScale::Create, KaxTrackTimecodeScale_TheId, "TrackTimecodeScale", KaxTrackTimecodeScale_Context);
const EbmlCallbacks KaxTrackName::ClassInfos(KaxTrackName::Create, KaxTrackName_TheId, "TrackName", KaxTrackName_Context);
const EbmlCallbacks KaxTrackLanguage::ClassInfos(KaxTrackLanguage::Create, KaxTrackLanguage_TheId, "TrackLanguage", KaxTrackLanguage_Context);
const EbmlCallbacks KaxCodecID::ClassInfos(KaxCodecID::Create, KaxCodecID_TheId, "CodecID", KaxCodecID_Context);
const EbmlCallbacks KaxCodecPrivate::ClassInfos(KaxCodecPrivate::Create, KaxCodecPrivate_TheId, "CodecPrivate", KaxCodecPrivate_Context);
const EbmlCallbacks KaxCodecName::ClassInfos(KaxCodecName::Create, KaxCodecName_TheId, "CodecName", KaxCodecName_Context);
const EbmlCallbacks KaxCodecSettings::ClassInfos(KaxCodecSettings::Create, KaxCodecSettings_TheId, "CodecSettings", KaxCodecSettings_Context);
const EbmlCallbacks KaxCodecInfoURL::ClassInfos(KaxCodecInfoURL::Create, KaxCodecInfoURL_TheId, "CodecInfoURL", KaxCodecInfoURL_Context);
const EbmlCallbacks KaxCodecDownloadURL::ClassInfos(KaxCodecDownloadURL::Create, KaxCodecDownloadURL_TheId, "CodecDownloadURL", KaxCodecDownloadURL_Context);
const EbmlCallbacks KaxCodecDecodeAll::ClassInfos(KaxCodecDecodeAll::Create, KaxCodecDecodeAll_TheId, "CodecDecodeAll", KaxCodecDecodeAll_Context);
const EbmlCallbacks KaxTrackOverlay::ClassInfos(KaxTrackOverlay::Create, KaxTrackOverlay_TheId, "TrackOverlay", KaxTrackOverlay_Context);

END_LIBMATROSKA_NAMESPACE
