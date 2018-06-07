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
	\version \$Id: KaxTags.cpp,v 1.9 2003/06/11 20:23:29 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Jory Stone       <jcsston @ toughguy.net>
*/
#include "StdInclude.h"
#include "KaxTags.h"
#include "KaxTag.h"
#include "KaxContexts.h"

using namespace LIBEBML_NAMESPACE;

// sub elements
START_LIBMATROSKA_NAMESPACE

EbmlSemantic KaxTags_ContextList[1] =
{
	EbmlSemantic(false, false, KaxTag::ClassInfos),
};

const EbmlSemanticContext KaxTags_Context = EbmlSemanticContext(countof(KaxTags_ContextList), KaxTags_ContextList, &KaxSegment_Context, *GetKaxGlobal_Context, &KaxTags::ClassInfos);

EbmlId KaxTags_TheId(0x1254C367, 4);

const EbmlCallbacks KaxTags::ClassInfos(KaxTags::Create, KaxTags_TheId, "Tags", KaxTags_Context);

KaxTags::KaxTags()
	:EbmlMaster(KaxTags_Context)
{}

END_LIBMATROSKA_NAMESPACE
