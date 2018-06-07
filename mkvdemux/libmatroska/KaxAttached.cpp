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
	\version \$Id: KaxAttached.cpp,v 1.5 2003/05/20 21:42:27 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxAttached.h"
#include "KaxContexts.h"

// sub elements

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

EbmlSemantic KaxAttached_ContextList[4] =
{
	EbmlSemantic(true,  true, KaxFileName::ClassInfos),
	EbmlSemantic(true,  true, KaxMimeType::ClassInfos),
	EbmlSemantic(true,  true, KaxFileData::ClassInfos),
	EbmlSemantic(false, true, KaxFileDescription::ClassInfos),
};

EbmlId KaxAttached_TheId       (0x61A7, 2);
EbmlId KaxFileDescription_TheId(0x467E, 2);
EbmlId KaxFileName_TheId       (0x466E, 2);
EbmlId KaxMimeType_TheId       (0x4660, 2);
EbmlId KaxFileData_TheId       (0x465C, 2);

const EbmlCallbacks KaxAttached::ClassInfos(KaxAttached::Create, KaxAttached_TheId, "AttachedFile", KaxAttached_Context);
const EbmlCallbacks KaxFileDescription::ClassInfos(KaxFileDescription::Create, KaxFileDescription_TheId, "FileDescription", KaxFileDescription_Context);
const EbmlCallbacks KaxFileName::ClassInfos(KaxFileName::Create, KaxFileName_TheId, "FileName", KaxFileName_Context);
const EbmlCallbacks KaxMimeType::ClassInfos(KaxMimeType::Create, KaxMimeType_TheId, "FileMimeType", KaxMimeType_Context);
const EbmlCallbacks KaxFileData::ClassInfos(KaxFileData::Create, KaxFileData_TheId, "FileData", KaxFileData_Context);

const EbmlSemanticContext KaxAttached_Context = EbmlSemanticContext(countof(KaxAttached_ContextList), KaxAttached_ContextList, &KaxAttachements_Context, *GetKaxGlobal_Context, &KaxAttached::ClassInfos);
const EbmlSemanticContext KaxFileDescription_Context = EbmlSemanticContext(0, NULL, &KaxAttachements_Context, *GetKaxGlobal_Context, &KaxFileDescription::ClassInfos);
const EbmlSemanticContext KaxFileName_Context        = EbmlSemanticContext(0, NULL, &KaxAttachements_Context, *GetKaxGlobal_Context, &KaxFileName::ClassInfos);
const EbmlSemanticContext KaxMimeType_Context        = EbmlSemanticContext(0, NULL, &KaxAttachements_Context, *GetKaxGlobal_Context, &KaxMimeType::ClassInfos);
const EbmlSemanticContext KaxFileData_Context        = EbmlSemanticContext(0, NULL, &KaxAttachements_Context, *GetKaxGlobal_Context, &KaxFileData::ClassInfos);

KaxAttached::KaxAttached()
 :EbmlMaster(KaxAttached_Context)
{
	SetSizeLength(2); // mandatory min size support (for easier updating) (2^(7*2)-2 = 16Ko)
}

END_LIBMATROSKA_NAMESPACE

