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
	\version \$Id: EbmlStream.cpp,v 1.4 2003/04/29 18:57:03 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "EbmlStream.h"

START_LIBEBML_NAMESPACE

EbmlStream::EbmlStream(IOCallback & DataStream)
 :Stream(DataStream)
{}

EbmlStream::~EbmlStream()
{}

EbmlElement * EbmlStream::FindNextID(const EbmlCallbacks & ClassInfos, const uint64 MaxDataSize)
{
	return EbmlElement::FindNextID(Stream, ClassInfos, MaxDataSize);
}

EbmlElement * EbmlStream::FindNextElement(const EbmlSemanticContext & Context, int & UpperLevel, const uint64 MaxDataSize, bool AllowDummyElt, unsigned int MaxLowerLevel)
{
	return EbmlElement::FindNextElement(Stream, Context, UpperLevel, MaxDataSize, AllowDummyElt, MaxLowerLevel);
}

END_LIBEBML_NAMESPACE
