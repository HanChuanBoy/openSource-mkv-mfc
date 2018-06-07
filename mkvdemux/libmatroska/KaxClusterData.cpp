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
	\version \$Id: KaxClusterData.cpp,v 1.4 2003/04/29 19:34:32 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "KaxClusterData.h"
#include "KaxContexts.h"

START_LIBMATROSKA_NAMESPACE

EbmlId KaxClusterTimecode_TheId(0xE7, 1);
EbmlId KaxClusterPosition_TheId(0xA7, 1);
EbmlId KaxClusterPrevSize_TheId(0xAB, 1);

const EbmlSemanticContext KaxClusterTimecode_Context = EbmlSemanticContext(0, NULL, &KaxCluster_Context, *GetKaxGlobal_Context, &KaxClusterTimecode::ClassInfos);
const EbmlSemanticContext KaxClusterPosition_Context = EbmlSemanticContext(0, NULL, &KaxCluster_Context, *GetKaxGlobal_Context, &KaxClusterPosition::ClassInfos);
const EbmlSemanticContext KaxClusterPrevSize_Context = EbmlSemanticContext(0, NULL, &KaxCluster_Context, *GetKaxGlobal_Context, &KaxClusterPrevSize::ClassInfos);

const EbmlCallbacks KaxClusterTimecode::ClassInfos(KaxClusterTimecode::Create, KaxClusterTimecode_TheId, "ClusterTimecode", KaxClusterTimecode_Context);
const EbmlCallbacks KaxClusterPosition::ClassInfos(KaxClusterPosition::Create, KaxClusterPosition_TheId, "ClusterPosition", KaxClusterPosition_Context);
const EbmlCallbacks KaxClusterPrevSize::ClassInfos(KaxClusterPrevSize::Create, KaxClusterPrevSize_TheId, "ClusterPrevSize", KaxClusterPrevSize_Context);

END_LIBMATROSKA_NAMESPACE
