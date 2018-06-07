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
	\version \$Id: KaxBlockData.cpp,v 1.8 2003/06/11 21:04:22 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include <cassert>

#include "StdInclude.h"
#include "KaxBlockData.h"
#include "KaxContexts.h"
#include "KaxBlock.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

const EbmlSemantic KaxSlices_ContextList[1] =
{
	EbmlSemantic(false, false,  KaxTimeSlice::ClassInfos),
};

const EbmlSemantic KaxTimeSlice_ContextList[4] =
{
	EbmlSemantic(false, true,  KaxSliceLaceNumber::ClassInfos),
	EbmlSemantic(false, true,  KaxSliceFrameNumber::ClassInfos),
	EbmlSemantic(false, true,  KaxSliceDelay::ClassInfos),
	EbmlSemantic(false, true,  KaxSliceDuration::ClassInfos),
};

EbmlId KaxReferencePriority_TheId(0xFA, 1);
EbmlId KaxReferenceBlock_TheId   (0xFB, 1);
EbmlId KaxReferenceVirtual_TheId (0xFD, 1);
EbmlId KaxSlices_TheId           (0x8E, 1);
EbmlId KaxTimeSlice_TheId        (0xE8, 1);
EbmlId KaxSliceLaceNumber_TheId  (0xCC, 1);
EbmlId KaxSliceFrameNumber_TheId (0xCD, 1);
EbmlId KaxSliceDelay_TheId       (0xCE, 1);
EbmlId KaxSliceDuration_TheId    (0xCF, 1);

const EbmlSemanticContext KaxReferencePriority_Context = EbmlSemanticContext(0, NULL, &KaxBlockGroup_Context, *GetKaxGlobal_Context, &KaxReferencePriority::ClassInfos);
const EbmlSemanticContext KaxReferenceBlock_Context = EbmlSemanticContext(0, NULL, &KaxBlockGroup_Context, *GetKaxGlobal_Context, &KaxReferenceBlock::ClassInfos);
const EbmlSemanticContext KaxReferenceVirtual_Context = EbmlSemanticContext(0, NULL, &KaxBlockGroup_Context, *GetKaxGlobal_Context, &KaxReferenceVirtual::ClassInfos);
const EbmlSemanticContext KaxSlices_Context = EbmlSemanticContext(countof(KaxSlices_ContextList), KaxSlices_ContextList, &KaxBlockGroup_Context, *GetKaxGlobal_Context, &KaxSlices::ClassInfos);
const EbmlSemanticContext KaxTimeSlice_Context = EbmlSemanticContext(countof(KaxTimeSlice_ContextList), KaxTimeSlice_ContextList, &KaxSlices_Context, *GetKaxGlobal_Context, &KaxTimeSlice::ClassInfos);
const EbmlSemanticContext KaxSliceLaceNumber_Context = EbmlSemanticContext(0, NULL, &KaxTimeSlice_Context, *GetKaxGlobal_Context, &KaxSliceLaceNumber::ClassInfos);
const EbmlSemanticContext KaxSliceFrameNumber_Context = EbmlSemanticContext(0, NULL, &KaxTimeSlice_Context, *GetKaxGlobal_Context, &KaxSliceFrameNumber::ClassInfos);
const EbmlSemanticContext KaxSliceDelay_Context = EbmlSemanticContext(0, NULL, &KaxTimeSlice_Context, *GetKaxGlobal_Context, &KaxSliceDelay::ClassInfos);
const EbmlSemanticContext KaxSliceDuration_Context = EbmlSemanticContext(0, NULL, &KaxTimeSlice_Context, *GetKaxGlobal_Context, &KaxSliceDuration::ClassInfos);

const EbmlCallbacks KaxReferencePriority::ClassInfos(KaxReferencePriority::Create, KaxReferencePriority_TheId, "FlagReferenced", KaxReferencePriority_Context);
const EbmlCallbacks KaxReferenceBlock::ClassInfos(KaxReferenceBlock::Create, KaxReferenceBlock_TheId, "ReferenceBlock", KaxReferenceBlock_Context);
const EbmlCallbacks KaxReferenceVirtual::ClassInfos(KaxReferenceVirtual::Create, KaxReferenceVirtual_TheId, "ReferenceVirtual", KaxReferenceVirtual_Context);
const EbmlCallbacks KaxSlices::ClassInfos(KaxSlices::Create, KaxSlices_TheId, "Slices", KaxSlices_Context);
const EbmlCallbacks KaxTimeSlice::ClassInfos(KaxTimeSlice::Create, KaxTimeSlice_TheId, "TimeSlice", KaxTimeSlice_Context);
const EbmlCallbacks KaxSliceLaceNumber::ClassInfos(KaxSliceLaceNumber::Create, KaxSliceLaceNumber_TheId, "SliceLaceNumber", KaxSliceLaceNumber_Context);
const EbmlCallbacks KaxSliceFrameNumber::ClassInfos(KaxSliceFrameNumber::Create, KaxSliceFrameNumber_TheId, "SliceFrameNumber", KaxSliceFrameNumber_Context);
const EbmlCallbacks KaxSliceDelay::ClassInfos(KaxSliceDelay::Create, KaxSliceDelay_TheId, "SliceDelay", KaxSliceDelay_Context);
const EbmlCallbacks KaxSliceDuration::ClassInfos(KaxSliceDuration::Create, KaxSliceDuration_TheId, "SliceDuration", KaxSliceDuration_Context);

KaxSlices::KaxSlices()
 :EbmlMaster(KaxSlices_Context)
{}

KaxTimeSlice::KaxTimeSlice()
 :EbmlMaster(KaxTimeSlice_Context)
{}

const KaxBlockGroup & KaxReferenceBlock::RefBlock() const
{
	assert(RefdBlock != NULL);
	return *RefdBlock;
}

uint64 KaxReferenceBlock::UpdateSize(bool bSaveDefault)
{
	assert(RefdBlock != NULL);
	assert(ParentBlock != NULL);

	Value = (int64(RefdBlock->GlobalTimecode()) - int64(ParentBlock->GlobalTimecode())) / int64(ParentBlock->GlobalTimecodeScale());
	return EbmlSInteger::UpdateSize(bSaveDefault);
}

END_LIBMATROSKA_NAMESPACE
