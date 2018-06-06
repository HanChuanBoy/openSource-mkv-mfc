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
	\version \$Id: EbmlSubHead.cpp,v 1.2 2003/04/14 20:30:25 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "EbmlSubHead.h"
#include "EbmlContexts.h"

START_LIBEBML_NAMESPACE

EbmlId EVersion_TheId            (0x4286, 2);
EbmlId EReadVersion_TheId        (0x42F7, 2);
EbmlId EMaxIdLength_TheId        (0x42F2, 2);
EbmlId EMaxSizeLength_TheId      (0x42F3, 2);
EbmlId EDocType_TheId            (0x4282, 2);
EbmlId EDocTypeVersion_TheId     (0x4287, 2);
EbmlId EDocTypeReadVersion_TheId (0x4285, 2);

const EbmlCallbacks EVersion::ClassInfos(EVersion::Create,               EVersion_TheId,        "EBMLVersion",                        EVersion_Context);
const EbmlCallbacks EReadVersion::ClassInfos(EReadVersion::Create,       EReadVersion_TheId,    "EBMLReadVersion",                    EReadVersion_Context);
const EbmlCallbacks EMaxIdLength::ClassInfos(EMaxIdLength::Create,       EMaxIdLength_TheId,    "EBMLMaxIdLength",                    EMaxIdLength_Context);
const EbmlCallbacks EMaxSizeLength::ClassInfos(EMaxSizeLength::Create,   EMaxSizeLength_TheId,  "EBMLMaxSizeLength",                  EMaxSizeLength_Context);
const EbmlCallbacks EDocType::ClassInfos(EDocType::Create,               EDocType_TheId,        "EBMLDocType",                        EDocType_Context);
const EbmlCallbacks EDocTypeVersion::ClassInfos(EDocTypeVersion::Create, EDocTypeVersion_TheId, "EBMLDocTypeVersion",                 EDocTypeVersion_Context);
const EbmlCallbacks EDocTypeReadVersion::ClassInfos(EDocTypeReadVersion::Create, EDocTypeReadVersion_TheId, "EBMLDocTypeReadVersion", EDocTypeReadVersion_Context);

const EbmlSemanticContext EVersion_Context        = EbmlSemanticContext(0, NULL, &EbmlHead_Context, *GetEbmlGlobal_Context, &EVersion::ClassInfos);
const EbmlSemanticContext EReadVersion_Context    = EbmlSemanticContext(0, NULL, &EbmlHead_Context, *GetEbmlGlobal_Context, &EReadVersion::ClassInfos);
const EbmlSemanticContext EMaxIdLength_Context    = EbmlSemanticContext(0, NULL, &EbmlHead_Context, *GetEbmlGlobal_Context, &EMaxIdLength::ClassInfos);
const EbmlSemanticContext EMaxSizeLength_Context  = EbmlSemanticContext(0, NULL, &EbmlHead_Context, *GetEbmlGlobal_Context, &EMaxSizeLength::ClassInfos);
const EbmlSemanticContext EDocType_Context        = EbmlSemanticContext(0, NULL, &EbmlHead_Context, *GetEbmlGlobal_Context, &EDocType::ClassInfos);
const EbmlSemanticContext EDocTypeVersion_Context = EbmlSemanticContext(0, NULL, &EbmlHead_Context, *GetEbmlGlobal_Context, &EDocTypeVersion::ClassInfos);
const EbmlSemanticContext EDocTypeReadVersion_Context = EbmlSemanticContext(0, NULL, &EbmlHead_Context, *GetEbmlGlobal_Context, &EDocTypeReadVersion::ClassInfos);

END_LIBEBML_NAMESPACE
