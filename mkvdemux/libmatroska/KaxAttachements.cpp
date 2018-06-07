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
	\version \$Id: KaxAttachements.cpp,v 1.4 2003/05/20 21:42:27 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxAttachements.h"
#include "KaxAttached.h"
#include "KaxContexts.h"

using namespace LIBEBML_NAMESPACE;

// sub elements
START_LIBMATROSKA_NAMESPACE

EbmlSemantic KaxAttachements_ContextList[1] =
{
	EbmlSemantic(true, false, KaxAttached::ClassInfos),        ///< EBMLVersion
};

const EbmlSemanticContext KaxAttachements_Context = EbmlSemanticContext(countof(KaxAttachements_ContextList), KaxAttachements_ContextList, &KaxSegment_Context, *GetKaxGlobal_Context, &KaxAttachements::ClassInfos);

EbmlId KaxAttachements_TheId(0x1941A469, 4);
const EbmlCallbacks KaxAttachements::ClassInfos(KaxAttachements::Create, KaxAttachements_TheId, "Attachements", KaxAttachements_Context);

KaxAttachements::KaxAttachements()
 :EbmlMaster(KaxAttachements_Context)
{
	SetSizeLength(2); // mandatory min size support (for easier updating) (2^(7*2)-2 = 16Ko)
}

END_LIBMATROSKA_NAMESPACE
