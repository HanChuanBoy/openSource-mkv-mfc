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
	\version \$Id: KaxTrackAudio.cpp,v 1.5 2003/05/20 21:42:27 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxTrackAudio.h"

// sub elements
#include "KaxContexts.h"

START_LIBMATROSKA_NAMESPACE

const EbmlSemantic KaxTrackAudio_ContextList[4] =
{
	EbmlSemantic(true , true, KaxAudioSamplingFreq::ClassInfos),
	EbmlSemantic(true , true, KaxAudioChannels::ClassInfos),
	EbmlSemantic(false, true, KaxAudioPosition::ClassInfos),
	EbmlSemantic(false, true, KaxAudioBitDepth::ClassInfos),
};

const EbmlSemanticContext KaxTrackAudio_Context = EbmlSemanticContext(countof(KaxTrackAudio_ContextList), KaxTrackAudio_ContextList, &KaxTrackEntry_Context, *GetKaxGlobal_Context, &KaxTrackAudio::ClassInfos);
const EbmlSemanticContext KaxAudioSamplingFreq_Context = EbmlSemanticContext(0, NULL, &KaxTrackAudio_Context, *GetKaxGlobal_Context, &KaxAudioSamplingFreq::ClassInfos);
const EbmlSemanticContext KaxAudioChannels_Context = EbmlSemanticContext(0, NULL, &KaxTrackAudio_Context, *GetKaxGlobal_Context, &KaxAudioChannels::ClassInfos);
const EbmlSemanticContext KaxAudioPosition_Context = EbmlSemanticContext(0, NULL, &KaxTrackAudio_Context, *GetKaxGlobal_Context, &KaxAudioPosition::ClassInfos);
const EbmlSemanticContext KaxAudioBitDepth_Context = EbmlSemanticContext(0, NULL, &KaxTrackAudio_Context, *GetKaxGlobal_Context, &KaxAudioBitDepth::ClassInfos);

EbmlId KaxTrackAudio_TheId       (0xE1, 1);
EbmlId KaxAudioSamplingFreq_TheId(0xB5, 1);
EbmlId KaxAudioChannels_TheId    (0x9F, 1);
EbmlId KaxAudioPosition_TheId    (0x7D7B, 2);
EbmlId KaxAudioBitDepth_TheId    (0x6264, 2);

const EbmlCallbacks KaxTrackAudio::ClassInfos(KaxTrackAudio::Create, KaxTrackAudio_TheId, "TrackAudio", KaxTrackAudio_Context);
const EbmlCallbacks KaxAudioSamplingFreq::ClassInfos(KaxAudioSamplingFreq::Create, KaxAudioSamplingFreq_TheId, "AudioSamplingFreq", KaxAudioSamplingFreq_Context);
const EbmlCallbacks KaxAudioChannels::ClassInfos(KaxAudioChannels::Create, KaxAudioChannels_TheId, "AudioChannels", KaxAudioChannels_Context);
const EbmlCallbacks KaxAudioPosition::ClassInfos(KaxAudioPosition::Create, KaxAudioPosition_TheId, "AudioPosition", KaxAudioPosition_Context);
const EbmlCallbacks KaxAudioBitDepth::ClassInfos(KaxAudioBitDepth::Create, KaxAudioBitDepth_TheId, "AudioBitDepth", KaxAudioBitDepth_Context);

KaxTrackAudio::KaxTrackAudio()
	:EbmlMaster(KaxTrackAudio_Context)
{}

END_LIBMATROSKA_NAMESPACE
