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
	\version \$Id: EbmlUnicodeString.h,v 1.19 2003/06/14 16:20:19 mosu Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Moritz Bunkus <moritz @ bunkus.org>
*/
#ifndef LIBEBML_UNICODE_STRING_H
#define LIBEBML_UNICODE_STRING_H

#include <string>

#include "EbmlConfig.h"
#include "EbmlTypes.h"
#include "EbmlElement.h"

START_LIBEBML_NAMESPACE

/*!
  \class UTFstring
  A class storing strings in a wchar_t (ie, in UCS-2 or UCS-4)
  \note inspired by wstring which is not available everywhere
*/
class UTFstring {
public:
	typedef wchar_t value_type;

	UTFstring();
	UTFstring(const wchar_t *); // should be NULL terminated
	UTFstring(const UTFstring &);
	virtual ~UTFstring();
	bool operator==(const UTFstring&) const;
	UTFstring & operator=(const wchar_t *);
	UTFstring & operator=(const UTFstring &);
	UTFstring & operator=(wchar_t);
	size_t length() const {return _Length;}
	operator const wchar_t*() const {return _Data;}
	const wchar_t* c_str() const {return _Data;}
	const std::string & GetUTF8() const {return UTF8string;}
	void SetUTF8(const std::string &);
protected:
	size_t _Length; ///< length of the UCS string excluding the \0
	wchar_t* _Data; ///< internal UCS representation
	std::string UTF8string;
	static bool wcscmp(const wchar_t *str1, const wchar_t *str2);
	void UpdateFromUTF8();
	void UpdateFromUCS2();
};


/*!
    \class EbmlUnicodeString
    \brief Handle all operations on a Unicode string EBML element
	\note internally treated as a string made of wide characters (ie UCS-2 or UCS-4 depending on the machine)
*/
class EbmlUnicodeString : public EbmlElement {
	public:
		EbmlUnicodeString();
		EbmlUnicodeString(const UTFstring & DefaultValue);
		EbmlUnicodeString(const EbmlElement & ElementToClone);
		EbmlUnicodeString(const EbmlElement & ElementToClone, const UTFstring & aDefaultValue);
	
		virtual ~EbmlUnicodeString() {}
	
		bool ValidateSize() const {return true;} // any size is possible
		uint32 RenderData(IOCallback & output, bool bSaveDefault = false);
		uint64 ReadData(IOCallback & input);
		uint64 UpdateSize(bool bSaveDefault = false);
	
		EbmlUnicodeString & operator=(const UTFstring &); ///< platform dependant code
		operator const UTFstring &() const {return Value;}
	
		UTFstring DefaultVal() const {assert(DefaultIsSet); return DefaultValue;}

		bool IsDefaultValue() const {
			return (DefaultISset() && Value == DefaultValue);
		}

	protected:
		UTFstring Value; /// The actual value of the element
		UTFstring DefaultValue;
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_UNICODE_STRING_H
