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
    \version \$Id: EbmlEndian.h,v 1.1 2003/04/03 16:40:48 robux4 Exp $
    \author Ingo Ralf Blum   <ingoralfblum @ users.sf.net>
    \author Lasse Kärkkäinen <tronic @ users.sf.net>
    \author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBEBML_ENDIAN_H
#define LIBEBML_ENDIAN_H

#include <algorithm>

#include "EbmlConfig.h" // contains _ENDIANESS_

START_LIBEBML_NAMESPACE

enum endianess {
    big_endian,   ///< PowerPC, Alpha, 68000
    little_endian ///< Intel x86 platforms
};

/*!
    \class Endian
    \brief general class to handle endian-specific buffers
    \note don't forget to define/undefine _ENDIANESS_ to BIG_ENDIAN depending on your machine
*/
template<class TYPE, endianess ENDIAN> class Endian
{
    public:
	Endian() {}

	Endian(const TYPE value)
	{
		memcpy(&platform_value, &value, sizeof(TYPE));
		process_endian();
	}

	inline Endian & Eval(const binary *endian_buffer)
	{
	    endian_value = *(TYPE *)(endian_buffer);
	    process_platform();
	    return *this;
	}

	inline void Fill(binary *endian_buffer) const
	{
	    *(TYPE*)endian_buffer = endian_value;
	}

	inline operator const TYPE&() const { return platform_value; }
//	inline TYPE endian() const   { return endian_value; }
	inline const TYPE &endian() const       { return endian_value; }
	inline size_t size() const   { return sizeof(TYPE); }
	inline bool operator!=(const binary *buffer) const {return *((TYPE*)buffer) == platform_value;}

    protected:
	TYPE platform_value;
	TYPE endian_value;

	inline void process_endian()
	{
	    endian_value = platform_value;
#if WORDS_BIGENDIAN
	    if (ENDIAN == little_endian)
		std::reverse(reinterpret_cast<uint8*>(&endian_value),reinterpret_cast<uint8*>(&endian_value+1));
#else  // _ENDIANESS_
	    if (ENDIAN == big_endian)
		std::reverse(reinterpret_cast<uint8*>(&endian_value),reinterpret_cast<uint8*>(&endian_value+1));
#endif // _ENDIANESS_
	}

	inline void process_platform()
	{
	    platform_value = endian_value;
#if WORDS_BIGENDIAN
	    if (ENDIAN == little_endian)
		std::reverse(reinterpret_cast<uint8*>(&platform_value),reinterpret_cast<uint8*>(&platform_value+1));
#else  // _ENDIANESS_
	    if (ENDIAN == big_endian)
		std::reverse(reinterpret_cast<uint8*>(&platform_value),reinterpret_cast<uint8*>(&platform_value+1));
#endif // _ENDIANESS_
	}
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_ENDIAN_H
