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
	\version \$Id: KaxContexts.h,v 1.6 2003/06/11 20:50:54 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_CONTEXTS_H
#define LIBMATROSKA_CONTEXTS_H

#include "KaxTypes.h"
#include "EbmlElement.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

extern const EbmlSemanticContext KaxSegment_Context;
extern const EbmlSemanticContext KaxAttachements_Context;
extern const EbmlSemanticContext KaxAttached_Context;
extern const EbmlSemanticContext KaxFileDescription_Context;
extern const EbmlSemanticContext KaxFileName_Context;
extern const EbmlSemanticContext KaxMimeType_Context;
extern const EbmlSemanticContext KaxFileData_Context;
extern const EbmlSemanticContext KaxChapters_Context;
extern const EbmlSemanticContext KaxCluster_Context;
extern const EbmlSemanticContext KaxTags_Context;
extern const EbmlSemanticContext KaxTag_Context;
extern const EbmlSemanticContext KaxBlockGroup_Context;
extern const EbmlSemanticContext KaxReferencePriority_Context;
extern const EbmlSemanticContext KaxReferenceBlock_Context;
extern const EbmlSemanticContext KaxReferenceVirtual_Context;
extern const EbmlSemanticContext KaxCues_Context;
extern const EbmlSemanticContext KaxInfo_Context;
extern const EbmlSemanticContext KaxSeekHead_Context;
extern const EbmlSemanticContext KaxTracks_Context;
extern const EbmlSemanticContext KaxTrackEntry_Context;
extern const EbmlSemanticContext KaxTrackNumber_Context;
extern const EbmlSemanticContext KaxTrackType_Context;
extern const EbmlSemanticContext KaxTrackFlagEnabled_Context;
extern const EbmlSemanticContext KaxTrackFlagDefault_Context;
extern const EbmlSemanticContext KaxTrackFlagLacing_Context;
extern const EbmlSemanticContext KaxTrackName_Context;
extern const EbmlSemanticContext KaxTrackLanguage_Context;
extern const EbmlSemanticContext KaxCodecID_Context;
extern const EbmlSemanticContext KaxCodecPrivate_Context;
extern const EbmlSemanticContext KaxCodecName_Context;
extern const EbmlSemanticContext KaxCodecSettings_Context;
extern const EbmlSemanticContext KaxCodecInfoURL_Context;
extern const EbmlSemanticContext KaxCodecDownloadURL_Context;
extern const EbmlSemanticContext KaxCodecDecodeAll_Context;
extern const EbmlSemanticContext KaxTrackOverlay_Context;

extern const EbmlSemanticContext & GetKaxGlobal_Context();
extern const EbmlSemanticContext & GetKaxTagsGlobal_Context();

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_CONTEXTS_H
