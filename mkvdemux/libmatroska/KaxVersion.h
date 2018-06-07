/****************************************************************************
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
	\version \$Id: KaxVersion.h,v 1.7 2003/05/22 21:32:07 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_VERSION_H
#define LIBMATROSKA_VERSION_H

#include <string>

#include "KaxConfig.h"

START_LIBMATROSKA_NAMESPACE

#define LIBMATROSKA_VERSION 000404

static const std::string KaxCodeVersion = "0.4.4";
static const std::string KaxCodeDate    = __TIMESTAMP__;

/*!
	\todo Take the libmcf API used in VDubMod and map it to the new matroska/ebml objects.
	\todo Improve the CRC/ECC system (backward and forward possible ?) to fit streaming/live writing/simple reading
	\todo Signed element should be opt-out, not opt-in (only add a list of elements not signed)
*/

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_VERSION_H
