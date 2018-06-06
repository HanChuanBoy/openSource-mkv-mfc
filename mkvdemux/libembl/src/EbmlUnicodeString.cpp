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
	\version \$Id: EbmlUnicodeString.cpp,v 1.26 2003/06/14 14:25:29 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/

#include <cassert>

#if __GNUC__ == 2
#include <wchar.h>
#endif

#include "StdInclude.h"
#include "EbmlUnicodeString.h"

START_LIBEBML_NAMESPACE

// ===================== UTFstring class ===================

UTFstring::UTFstring()
	:_Length(0)
	 ,_Data(NULL)
{}

UTFstring::UTFstring(const wchar_t * _aBuf)
	:_Length(0)
	 ,_Data(NULL)
{
	*this = _aBuf;
}

UTFstring::~UTFstring()
{
	delete [] _Data;
}

UTFstring::UTFstring(const UTFstring & _aBuf)
	:_Length(0)
	 ,_Data(NULL)
{
	*this = _aBuf.c_str();
}

UTFstring & UTFstring::operator=(const wchar_t * _aBuf)
{
	delete [] _Data;
	size_t aLen;
	for (aLen=0; _aBuf[aLen] != 0; aLen++);
	_Length = aLen;
	_Data = new wchar_t[_Length+1];
	for (aLen=0; _aBuf[aLen] != 0; aLen++) {
	    _Data[aLen] = _aBuf[aLen];
	}
	_Data[aLen] = 0;
	UpdateFromUCS2();
	return *this;
}

UTFstring & UTFstring::operator=(const UTFstring & _aBuf)
{
	*this = _aBuf.c_str();
	return *this;
}

bool UTFstring::operator==(const UTFstring& _aStr) const
{
	if ((_Data == NULL) && (_aStr._Data == NULL))
		return true;
	if ((_Data == NULL) || (_aStr._Data == NULL))
		return false;
	return (wcscmp(_Data, _aStr._Data) == 0);
}

UTFstring & UTFstring::operator=(wchar_t _aChar)
{
	delete [] _Data;
	_Data = new wchar_t[2];
	_Length = 1;
	_Data[0] = _aChar;
	_Data[1] = 0;
	UpdateFromUCS2();
	return *this;
}

void UTFstring::SetUTF8(const std::string & _aStr)
{
    UTF8string = _aStr;
    UpdateFromUTF8();
}

void UTFstring::UpdateFromUTF8()
{
    delete [] _Data;
    // find the size of the final UCS-2 string
    size_t i;
    for (_Length=0, i=0; i<UTF8string.length(); _Length++) {
	if ((UTF8string[i] & 0x80) == 0) {
	    i++;
	} else if ((UTF8string[i] & 0x20) == 0) {
	    i += 2;
	} else if ((UTF8string[i] & 0x08) == 0) {
	    i += 3;
	}
    }
    _Data = new wchar_t[_Length+1];
    size_t j;
    for (j=0, i=0; i<UTF8string.length(); j++) {
	if ((UTF8string[i] & 0x80) == 0) {
	    _Data[j] = UTF8string[i];
	    i++;
	} else if ((UTF8string[i] & 0x20) == 0) {
	    _Data[j] = ((UTF8string[i] & 0x1F) << 6) + (UTF8string[i+1] & 0x3F);
	    i += 2;
	} else if ((UTF8string[i] & 0x08) == 0) {
	    _Data[j] = ((UTF8string[i] & 0x0F) << 12) + ((UTF8string[i+1] & 0x3F) << 6) + (UTF8string[i+2] & 0x3F);
	    i += 3;
	}
    }
    _Data[j] = 0;
}

void UTFstring::UpdateFromUCS2()
{
    // find the size of the final UTF-8 string
    size_t i,Size=0;
    for (i=0; i<_Length; i++)
    {
	if (_Data[i] < 0x80) {
	    Size++;
	} else if (_Data[i] < 0x800) {
	    Size += 2;
	} else if (_Data[i] < 0x10000) {
	    Size += 3;
	}
    }
    std::string::value_type *tmpStr = new std::string::value_type[Size+1];
    for (i=0, Size=0; i<_Length; i++)
    {
	if (_Data[i] < 0x80) {
	    tmpStr[Size++] = _Data[i];
	} else if (_Data[i] < 0x800) {
	    tmpStr[Size++] = 0xC0 | (_Data[i] >> 6);
	    tmpStr[Size++] = 0x80 | (_Data[i] & 0x3F);
	} else if (_Data[i] < 0x10000) {
	    tmpStr[Size++] = 0xE0 | (_Data[i] >> 12);
	    tmpStr[Size++] = 0x80 | ((_Data[i] >> 6) & 0x3F);
	    tmpStr[Size++] = 0x80 | (_Data[i] & 0x3F);
	}
    }
    tmpStr[Size] = 0;
    UTF8string = tmpStr; // implicit conversion
    delete [] tmpStr;
}

bool UTFstring::wcscmp(const wchar_t *str1, const wchar_t *str2)
{
    size_t Index=0;
    while (str1[Index] == str2[Index] && str1[Index] != 0) {
	Index++;
    }
    return (str1[Index] == str2[Index]);
}

// ===================== EbmlUnicodeString class ===================

EbmlUnicodeString::EbmlUnicodeString()
 :EbmlElement(0, false)
{
	DefaultSize = 0;
}

EbmlUnicodeString::EbmlUnicodeString(const UTFstring & aDefaultValue)
 :EbmlElement(0, true), Value(aDefaultValue), DefaultValue(aDefaultValue)
{
	DefaultSize = 0;
	DefaultIsSet = true;
}

EbmlUnicodeString::EbmlUnicodeString(const EbmlElement & ElementToClone)
 :EbmlElement(0, false)
{
	*(reinterpret_cast<EbmlElement*>(this)) = ElementToClone;
}

EbmlUnicodeString::EbmlUnicodeString(const EbmlElement & ElementToClone, const UTFstring & aDefaultValue)
 :EbmlElement(0, true), Value(aDefaultValue), DefaultValue(aDefaultValue)
{
	*(reinterpret_cast<EbmlElement*>(this)) = ElementToClone;
	DefaultIsSet = true;
}

/*!
	\note limited to UCS-2
	\todo handle exception on errors
*/
uint32 EbmlUnicodeString::RenderData(IOCallback & output, bool bSaveDefault)
{
	uint32 Result = Value.GetUTF8().length();

	if (Result != 0) {
	    output.writeFully(Value.GetUTF8().c_str(), Result);
	}

	if (Result < DefaultSize) {
		// pad the rest with 0
		binary *Pad = new binary[DefaultSize - Result];
		if (Pad != NULL) {
			memset(Pad, 0x00, DefaultSize - Result);
			output.writeFully(Pad, DefaultSize - Result);

			Result = DefaultSize;
			delete [] Pad;
		}
	}

	return Result;
}

EbmlUnicodeString & EbmlUnicodeString::operator=(const UTFstring & NewString)
{
	Value = NewString;
	bValueIsSet = true;
	return *this;
}

/*!
	\note limited to UCS-2
*/
uint64 EbmlUnicodeString::UpdateSize(bool bSaveDefault)
{
    if (!bSaveDefault && IsDefaultValue())
	return 0;

    Size = Value.GetUTF8().length();
    if (Size < DefaultSize)
	Size = DefaultSize;
	
    return Size;
}

/*!
	\note limited to UCS-2
*/
uint64 EbmlUnicodeString::ReadData(IOCallback & input)
{
	if (Size == 0) {
		Value = UTFstring::value_type(0);
	} else {
		char *Buffer = new char[Size+1];
		if (Buffer == NULL) {
			// impossible to read, skip it
			input.setFilePointer(Size, seek_current);
		} else {
			input.readFully(Buffer, Size);
			if (Buffer[Size-1] != 0) {
				Buffer[Size] = 0;
			}

			Value.SetUTF8(Buffer); // implicit conversion to std::string
			delete [] Buffer;
		}
	}

	return Size;
}

END_LIBEBML_NAMESPACE
