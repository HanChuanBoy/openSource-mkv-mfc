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
	\version \$Id: KaxSegment.cpp,v 1.8 2003/05/20 21:42:27 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxSegment.h"
#include "EbmlHead.h"

// sub elements
#include "KaxCluster.h"
#include "KaxSeekHead.h"
#include "KaxCues.h"
#include "KaxTracks.h"
#include "KaxInfo.h"
#include "KaxChapters.h"
#include "KaxAttachements.h"
#include "KaxTags.h"
#include "KaxContexts.h"

START_LIBMATROSKA_NAMESPACE

EbmlSemantic KaxMatroska_ContextList[2] =
{
	EbmlSemantic(true, true,  EbmlHead::ClassInfos),
	EbmlSemantic(true, false, KaxSegment::ClassInfos),
};

EbmlSemantic KaxSegment_ContextList[8] =
{
	EbmlSemantic(false, false, KaxCluster::ClassInfos),    //所有包含有
	EbmlSemantic(false, false, KaxSeekHead::ClassInfos),  //包含有级别为SeekHead;
	EbmlSemantic(false, true,  KaxCues::ClassInfos),      //speed seeking快速seek;
	EbmlSemantic(false, false, KaxTracks::ClassInfos),    //包含有Tracks;
	EbmlSemantic(true,  true,  KaxInfo::ClassInfos),     //segment infomation;
	EbmlSemantic(false, true,  KaxChapters::ClassInfos),
	EbmlSemantic(false, true,  KaxAttachements::ClassInfos),
	EbmlSemantic(false, true,  KaxTags::ClassInfos),
};

const EbmlSemanticContext KaxMatroska_Context = EbmlSemanticContext(countof(KaxMatroska_ContextList), KaxMatroska_ContextList, NULL, *GetKaxGlobal_Context, NULL);
const EbmlSemanticContext KaxSegment_Context = EbmlSemanticContext(countof(KaxSegment_ContextList), KaxSegment_ContextList, NULL, *GetKaxGlobal_Context, &KaxSegment::ClassInfos);

EbmlId KaxSegment_TheId(0x18538067, 4);
const EbmlCallbacks KaxSegment::ClassInfos(KaxSegment::Create, KaxSegment_TheId, "Segment", KaxSegment_Context);
//根元素,涵盖所有最高级别元素1;

KaxSegment::KaxSegment()
	:EbmlMaster(KaxSegment_Context)
{
	SetSizeLength(5); // mandatory min size support (for easier updating) (2^(7*5)-2 = 32Go)
	SetSizeInfinite(); // by default a segment is big and the size is unknown in advance
}

uint64 KaxSegment::GetRelativePosition(uint64 aGlobalPosition) const
{
	return aGlobalPosition - GetElementPosition() - HeadSize();
}

uint64 KaxSegment::GetRelativePosition(const EbmlElement & Elt) const
{
	return GetRelativePosition(Elt.GetElementPosition());
}

uint64 KaxSegment::GetGlobalPosition(uint64 aRelativePosition) const
{
	return aRelativePosition + GetElementPosition() + HeadSize();
}

END_LIBMATROSKA_NAMESPACE
