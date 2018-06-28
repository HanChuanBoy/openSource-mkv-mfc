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
	\version \$Id: EbmlVersion.h,v 1.7 2003/05/22 21:32:35 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBEBML_VERSION_H
#define LIBEBML_VERSION_H

#include <string>

#include "EbmlConfig.h"

START_LIBEBML_NAMESPACE

#define LIBEBML_VERSION 000404

static const std::string EbmlCodeVersion = "0.4.4";
static const std::string EbmlCodeDate    = __TIMESTAMP__;

/*!
	\todo Implement "global" elements (can be used at all levels) for EBML (Void + CRC) and matroska (Signature)
	\todo Closer relation between an element and the context it comes from (context is an element attribute ?)
	\todo Ensure all elements written have a value set (default or by the user)
	\todo Better separation of what is specific to EbmlMaster (infinite size, special skipping, finding, context, etc)
*/

END_LIBEBML_NAMESPACE

#endif // LIBEBML_VERSION_H
