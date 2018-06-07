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
	\version \$Id: KaxChapters.cpp,v 1.4 2003/05/20 21:42:27 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxChapters.h"
#include "KaxContexts.h"

// sub elements
START_LIBMATROSKA_NAMESPACE

EbmlSemantic *KaxChapters_ContextList;

const EbmlSemanticContext KaxChapters_Context = EbmlSemanticContext(0, KaxChapters_ContextList, &KaxSegment_Context, *GetKaxGlobal_Context, &KaxChapters::ClassInfos);

EbmlId KaxChapters_TheId(0x1043A770, 4);
const EbmlCallbacks KaxChapters::ClassInfos(KaxChapters::Create, KaxChapters_TheId, "Chapters", KaxChapters_Context);

KaxChapters::KaxChapters()
 :EbmlMaster(KaxChapters_Context)
{}


END_LIBMATROSKA_NAMESPACE
