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
	\version \$Id: EbmlMaster.h,v 1.7 2003/05/20 21:40:50 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBEBML_MASTER_H
#define LIBEBML_MASTER_H

#include <string>
#include <vector>

#include "EbmlTypes.h"
#include "EbmlElement.h"

START_LIBEBML_NAMESPACE

/*!
    \class EbmlMaster
    \brief Handle all operations on an EBML element that contains other EBML elements
*/
class EbmlMaster : public EbmlElement {
	public:
		EbmlMaster(const EbmlSemanticContext & aContext, bool bSizeIsKnown = true);
		EbmlMaster(const EbmlElement & ElementToClone);
		bool ValidateSize() const {return true;}
		/*!
			\warning be carefull to clear the memory allocated in the ElementList elsewhere
		*/
		virtual ~EbmlMaster();
	
		uint32 RenderData(IOCallback & output, bool bSaveDefault = false);
		uint64 ReadData(IOCallback & input);
		uint64 UpdateSize(bool bSaveDefault = false);
		
		/*!
			\brief Set wether the size is finite (size is known in advance when writing, or infinite size is not known on writing)
		*/
		bool SetSizeInfinite(bool aIsInfinite = true) {bSizeIsFinite = !aIsInfinite; return true;}
	
		bool PushElement(EbmlElement & element);
		uint64 GetSize() const { 
			if (bSizeIsFinite)
				return Size;
			else
				return (0-1);
		}

		/*!
			\brief find the element corresponding to the ID of the element, NULL if not found
		*/
		EbmlElement *FindElt(const EbmlCallbacks & Callbacks) const;
		/*!
			\brief find the first element corresponding to the ID of the element
		*/
		EbmlElement *FindFirstElt(const EbmlCallbacks & Callbacks, const bool bCreateIfNull);
		EbmlElement *FindFirstElt(const EbmlCallbacks & Callbacks) const;

		/*!
			\brief find the element of the same type of PasElt following in the list of elements
		*/
		EbmlElement *FindNextElt(const EbmlElement & PastElt, const bool bCreateIfNull);
		EbmlElement *FindNextElt(const EbmlElement & PastElt) const;
		EbmlElement *AddNewElt(const EbmlCallbacks & Callbacks);

		/*!
			\brief Read the data and keep the known children
		*/
		void Read(EbmlStream & inDataStream, const EbmlSemanticContext & Context, int & UpperEltFound, EbmlElement * & FoundElt, bool AllowDummyElt);
		
		/*!
			\brief sort Data when they can
		*/
		void Sort();

		unsigned int ListSize() const {return ElementList.size();}

		EbmlElement * operator[](unsigned int position) {return ElementList[position];}
		const EbmlElement * operator[](unsigned int position) const {return ElementList[position];}

		bool IsDefaultValue() const {
			return false;
		}

		/*!
			\brief verify that all mandatory elements are present
			\note usefull after reading or before writing
		*/
		bool CheckMandatory() const;

		/*!
			\brief facility for Master elements to write only the head and force the size later
			\warning
		*/
		uint32 WriteHead(IOCallback & output, int SizeLength, bool bSaveDefault = false);

	protected:
		std::vector<EbmlElement *> ElementList;
	
		const EbmlSemanticContext & Context;

	private:
		/*!
			\brief Add all the mandatory elements to the list
		*/
		bool ProcessMandatory();
};

template <typename Type>
Type & GetChild(EbmlMaster & Master)
{
	return *(static_cast<Type *>(Master.FindFirstElt(Type::ClassInfos, true)));
}
// call with
// MyDocType = GetChild<EDocType>(TestHead);

template <typename Type>
Type & GetNextChild(EbmlMaster & Master, const Type & PastElt)
{
	return *(static_cast<Type *>(Master.FindNextElt(PastElt, true)));
}

template <typename Type>
Type & AddNewChild(EbmlMaster & Master)
{
	return *(static_cast<Type *>(Master.AddNewElt(Type::ClassInfos)));
}

END_LIBEBML_NAMESPACE

#endif // LIBEBML_MASTER_H
