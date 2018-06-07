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
	\version \$Id: KaxInfoData.cpp,v 1.4 2003/04/28 10:06:10 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author John Cannon      <spyder2555 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxInfoData.h"
#include "KaxContexts.h"

START_LIBMATROSKA_NAMESPACE

EbmlId KaxSegmentUID_TheId     (0x73A4, 2);
EbmlId KaxSegmentFilename_TheId(0x7384, 2);
EbmlId KaxPrevUID_TheId        (0x3CB923, 3);
EbmlId KaxPrevFilename_TheId   (0x3C83AB, 3);
EbmlId KaxNextUID_TheId        (0x3EB923, 3);
EbmlId KaxNextFilename_TheId   (0x3E83BB, 3);
EbmlId KaxTimecodeScale_TheId  (0x2AD7B1, 3);
EbmlId KaxDuration_TheId       (0x4489, 2);
EbmlId KaxDateUTC_TheId        (0x4461, 2);
EbmlId KaxTitle_TheId          (0x7BA9, 2);

const EbmlSemanticContext KaxSegmentUID_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxSegmentUID::ClassInfos);
const EbmlSemanticContext KaxSegmentFilename_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxSegmentFilename::ClassInfos);
const EbmlSemanticContext KaxPrevUID_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxPrevUID::ClassInfos);
const EbmlSemanticContext KaxPrevFilename_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxPrevFilename::ClassInfos);
const EbmlSemanticContext KaxNextUID_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxNextUID::ClassInfos);
const EbmlSemanticContext KaxNextFilename_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxNextFilename::ClassInfos);
const EbmlSemanticContext KaxTimecodeScale_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxTimecodeScale::ClassInfos);
const EbmlSemanticContext KaxDuration_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxDuration::ClassInfos);
const EbmlSemanticContext KaxDateUTC_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxDateUTC::ClassInfos);
const EbmlSemanticContext KaxTitle_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxTitle::ClassInfos);


const EbmlCallbacks KaxSegmentUID::ClassInfos(KaxSegmentUID::Create, KaxSegmentUID_TheId, "SegmentUID", KaxSegmentUID_Context);
const EbmlCallbacks KaxSegmentFilename::ClassInfos(KaxSegmentFilename::Create, KaxSegmentFilename_TheId, "SegmentFilename", KaxSegmentFilename_Context);
const EbmlCallbacks KaxPrevUID::ClassInfos(KaxPrevUID::Create, KaxPrevUID_TheId, "PrevUID", KaxPrevUID_Context);
const EbmlCallbacks KaxPrevFilename::ClassInfos(KaxPrevFilename::Create, KaxPrevFilename_TheId, "PrevFilename", KaxPrevFilename_Context);
const EbmlCallbacks KaxNextUID::ClassInfos(KaxNextUID::Create, KaxNextUID_TheId, "NextUID", KaxNextUID_Context);
const EbmlCallbacks KaxNextFilename::ClassInfos(KaxNextFilename::Create, KaxNextFilename_TheId, "NextFilename", KaxNextFilename_Context);
const EbmlCallbacks KaxTimecodeScale::ClassInfos(KaxTimecodeScale::Create, KaxTimecodeScale_TheId, "TimecodeScale", KaxTimecodeScale_Context);
const EbmlCallbacks KaxDuration::ClassInfos(KaxDuration::Create, KaxDuration_TheId, "Duration", KaxDuration_Context);
const EbmlCallbacks KaxDateUTC::ClassInfos(KaxDateUTC::Create, KaxDateUTC_TheId, "DateUTC", KaxDateUTC_Context);
const EbmlCallbacks KaxTitle::ClassInfos(KaxTitle::Create, KaxTitle_TheId, "Title", KaxTitle_Context);

END_LIBMATROSKA_NAMESPACE
