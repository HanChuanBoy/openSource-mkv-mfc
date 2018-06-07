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
	\version \$Id: KaxInfo.cpp,v 1.7 2003/05/21 13:30:11 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxInfo.h"
#include "KaxInfoData.h"

#include "KaxContexts.h"

// sub elements
START_LIBMATROSKA_NAMESPACE

const EbmlSemantic KaxInfo_ContextList[12] =
{
	EbmlSemantic(false, true, KaxSegmentUID::ClassInfos),
	EbmlSemantic(false, true, KaxSegmentFilename::ClassInfos),
	EbmlSemantic(false, true, KaxPrevUID::ClassInfos),
	EbmlSemantic(false, true, KaxPrevFilename::ClassInfos),
	EbmlSemantic(false, true, KaxNextUID::ClassInfos),
	EbmlSemantic(false, true, KaxNextFilename::ClassInfos),
	EbmlSemantic(true,  true, KaxTimecodeScale::ClassInfos),
	EbmlSemantic(false, true, KaxDuration::ClassInfos),
	EbmlSemantic(false, true, KaxDateUTC::ClassInfos),
	EbmlSemantic(false, true, KaxTitle::ClassInfos),
	EbmlSemantic(true,  true, KaxMuxingApp::ClassInfos),
	EbmlSemantic(true,  true, KaxWritingApp::ClassInfos),
};

const EbmlSemanticContext KaxInfo_Context = EbmlSemanticContext(countof(KaxInfo_ContextList), KaxInfo_ContextList, &KaxSegment_Context, *GetKaxGlobal_Context, &KaxInfo::ClassInfos);
const EbmlSemanticContext KaxMuxingApp_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxMuxingApp::ClassInfos);
const EbmlSemanticContext KaxWritingApp_Context = EbmlSemanticContext(0, NULL, &KaxInfo_Context, *GetKaxGlobal_Context, &KaxWritingApp::ClassInfos);

EbmlId KaxInfo_TheId      (0x1549A966, 4);
EbmlId KaxMuxingApp_TheId (0x4D80, 2);
EbmlId KaxWritingApp_TheId(0x5741, 2);

const EbmlCallbacks KaxInfo::ClassInfos(KaxInfo::Create, KaxInfo_TheId, "Info", KaxInfo_Context);
const EbmlCallbacks KaxMuxingApp::ClassInfos(KaxMuxingApp::Create, KaxMuxingApp_TheId, "MuxingApp", KaxMuxingApp_Context);
const EbmlCallbacks KaxWritingApp::ClassInfos(KaxWritingApp::Create, KaxWritingApp_TheId, "WritingApp", KaxWritingApp_Context);

KaxInfo::KaxInfo()
	:EbmlMaster(KaxInfo_Context)
{}

END_LIBMATROSKA_NAMESPACE
