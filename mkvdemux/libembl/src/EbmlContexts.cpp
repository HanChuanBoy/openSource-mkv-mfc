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
	\version \$Id: EbmlContexts.cpp,v 1.1 2003/04/03 16:40:48 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "EbmlContexts.h"
#include "EbmlCrc32.h"
#include "EbmlVoid.h"

START_LIBEBML_NAMESPACE

const EbmlSemantic EbmlGlobal_ContextList[2] =
{
	EbmlSemantic(false, false, EbmlCrc32::ClassInfos),   ///< EbmlCrc32
	EbmlSemantic(false, false, EbmlVoid::ClassInfos),    ///< EbmlVoid
};

const EbmlSemanticContext EbmlVoid_Context = EbmlSemanticContext(0, NULL, NULL, *GetEbmlGlobal_Context, NULL);

static const EbmlSemanticContext EbmlGlobal_Context = EbmlSemanticContext(countof(EbmlGlobal_ContextList), EbmlGlobal_ContextList, NULL, *GetEbmlGlobal_Context, NULL);

//返回左值引用，不能被修改;
const EbmlSemanticContext & GetEbmlGlobal_Context()
{
	return EbmlGlobal_Context;
}

END_LIBEBML_NAMESPACE
