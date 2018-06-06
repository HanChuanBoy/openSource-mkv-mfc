/****************************************************************************
** libebml : parse EBML files, see http://embl.sourceforge.net/
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
	\version \$Id: EbmlHead.cpp,v 1.3 2003/05/20 21:40:50 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "EbmlHead.h"
#include "EbmlSubHead.h"
#include "EbmlContexts.h"

START_LIBEBML_NAMESPACE

const EbmlSemantic EbmlHead_ContextList[] =
{
	EbmlSemantic(true, true, EVersion::ClassInfos),        ///< EBMLVersion
	EbmlSemantic(true, true, EReadVersion::ClassInfos),    ///< EBMLReadVersion
	EbmlSemantic(true, true, EMaxIdLength::ClassInfos),    ///< EBMLMaxIdLength
	EbmlSemantic(true, true, EMaxSizeLength::ClassInfos),  ///< EBMLMaxSizeLength
	EbmlSemantic(true, true, EDocType::ClassInfos),        ///< DocType
	EbmlSemantic(true, true, EDocTypeVersion::ClassInfos), ///< DocTypeVersion
	EbmlSemantic(true, true, EDocTypeReadVersion::ClassInfos), ///< DocTypeReadVersion
};

//EbmlSemanticContext上下文,需要
const EbmlSemanticContext EbmlHead_Context = EbmlSemanticContext(countof(EbmlHead_ContextList), EbmlHead_ContextList, NULL, *GetEbmlGlobal_Context, &EbmlHead::ClassInfos);

EbmlId EbmlHead_TheId(0x1A45DFA3, 4);  //容器格式的可辨识行信息;
const EbmlCallbacks EbmlHead::ClassInfos(EbmlHead::Create, EbmlHead_TheId, "EBMLHead", EbmlHead_Context);

EbmlHead::EbmlHead()
 :EbmlMaster(EbmlHead_Context)
{}

END_LIBEBML_NAMESPACE
