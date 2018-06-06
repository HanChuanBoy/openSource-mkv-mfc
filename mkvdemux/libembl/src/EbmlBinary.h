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
	\version \$Id: EbmlBinary.h,v 1.3 2003/04/28 19:48:37 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Julien Coloos	<suiryc @ users.sf.net>
*/
#ifndef LIBEBML_BINARY_H
#define LIBEBML_BINARY_H

#include <string>

#include "EbmlConfig.h"
#include "EbmlTypes.h"
#include "EbmlElement.h"

START_LIBEBML_NAMESPACE

/*!
    \class EbmlBinary
    \brief Handle all operations on an EBML element that contains "unknown" binary data

	\todo handle fix sized elements (like UID of CodecID)
*/
class EbmlBinary : public EbmlElement {
	public:
		EbmlBinary();
		EbmlBinary(const EbmlElement & ElementToClone);
		virtual ~EbmlBinary(void);
	
		uint32 RenderData(IOCallback & output, bool bSaveDefault = false);
		uint64 ReadData(IOCallback & input);
		uint64 UpdateSize(bool bSaveDefault = false);
	
		void SetBuffer(const binary *Buffer, const uint32 BufferSize) {
			Data = (binary *) Buffer;
			Size = BufferSize;
			bValueIsSet = true;
		}

		binary *GetBuffer() const {return Data;}
		
		void CopyBuffer(const binary *Buffer, const uint32 BufferSize) {
			if (Data != NULL)
				delete Data;
			Data = new binary[BufferSize];
			memcpy(Data, Buffer, BufferSize);
			Size = BufferSize;
			bValueIsSet = true;
		}
		
		uint64 GetSize() const {return Size;}
		operator const binary &() const {return *Data;}
	
		bool IsDefaultValue() const {
			return false;
		}

	protected:
		binary *Data; // the binary data inside the element
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_BINARY_H
