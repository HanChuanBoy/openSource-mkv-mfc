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

	\version \$Id: EbmlMaster.cpp,v 1.12 2003/06/08 23:23:58 robux4 Exp $

	\author Steve Lhomme     <robux4 @ users.sf.net>

*/



#include <cassert>

#include <algorithm>



#include "StdInclude.h"

#include "EbmlMaster.h"

#include "EbmlStream.h"

#include "EbmlContexts.h"



START_LIBEBML_NAMESPACE



EbmlMaster::EbmlMaster(const EbmlSemanticContext & aContext, const bool bSizeIsknown)

 :EbmlElement(0), Context(aContext)

{

	bSizeIsFinite = bSizeIsknown;

	bValueIsSet = true;

	ProcessMandatory();

}

/*

EbmlMaster::EbmlMaster(const EbmlElement & ElementToClone)

	:EbmlElement(0), Context(ElementToClone.Context)

{

	*(static_cast<EbmlElement*>(this)) = ElementToClone;

}

*/

EbmlMaster::~EbmlMaster()

{

	assert(!bLocked); // you're trying to delete a locked element !!!



	size_t Index;

	

	for (Index = 0; Index < ElementList.size(); Index++) {

		if (!(*ElementList[Index]).IsLocked())	{

			delete ElementList[Index];

		}

	}

}



/*!

	\todo handle exception on errors

	\todo write all the Mandatory elements in the Context, otherwise assert

*/

uint32 EbmlMaster::RenderData(IOCallback & output, bool bSaveDefault)

{

	uint32 Result = 0;

	size_t Index;



	assert(CheckMandatory());

	

	for (Index = 0; Index < ElementList.size(); Index++) {

		if (!bSaveDefault && (ElementList[Index])->IsDefaultValue())

			continue;

		Result += (ElementList[Index])->Render(output, bSaveDefault);

	}



	return Result;

}



/*!

	\todo We might be able to forbid elements that don't exist in the context

*/

bool EbmlMaster::PushElement(EbmlElement & element)

{

	ElementList.push_back(&element);

	return true;

}



uint64 EbmlMaster::UpdateSize(bool bSaveDefault)

{

	Size = 0;



	if (!bSizeIsFinite)

		return (0-1);



	assert(CheckMandatory());

	

	size_t Index;

	

	for (Index = 0; Index < ElementList.size(); Index++) {

		if (!bSaveDefault && (ElementList[Index])->IsDefaultValue())

			continue;

		(ElementList[Index])->UpdateSize(bSaveDefault);

		uint64 SizeToAdd = (ElementList[Index])->ElementSize(bSaveDefault);

#if defined(_DEBUG) || defined(DEBUG)

		if (SizeToAdd == (0-1))

			return (0-1);

#endif // DEBUG

		Size += SizeToAdd;

	}



	return Size;

}



uint32 EbmlMaster::WriteHead(IOCallback & output, int SizeLength, bool bSaveDefault)

{

	SetSizeLength(SizeLength);

	return RenderHead(output, bSaveDefault);

}



/*!

	\todo this code is very suspicious !

*/

uint64 EbmlMaster::ReadData(IOCallback & input)

{

	input.setFilePointer(Size, seek_current);

	return Size;

}



/*!

	\note Hopefully no global element is mandatory

	\todo should be called for ALL EbmlMaster element on construction

*/

bool EbmlMaster::ProcessMandatory()

{

	if (Context.Size == 0)

	{

		return true;

	}



	assert(Context.MyTable != NULL);



	unsigned int EltIdx;

	for (EltIdx = 0; EltIdx < Context.Size; EltIdx++) {

		if (Context.MyTable[EltIdx].Mandatory && Context.MyTable[EltIdx].Unique) {

			assert(Context.MyTable[EltIdx].GetCallbacks.Create != NULL);

			PushElement(Context.MyTable[EltIdx].GetCallbacks.Create());

		}

	}

	return true;

}



bool EbmlMaster::CheckMandatory() const

{

	assert(Context.MyTable != NULL);



	unsigned int EltIdx;

	for (EltIdx = 0; EltIdx < Context.Size; EltIdx++) {

		if (Context.MyTable[EltIdx].Mandatory) {

			if (FindElt(Context.MyTable[EltIdx].GetCallbacks) == NULL) {

#if defined(_DEBUG) || defined(DEBUG)

				// you are missing this Mandatory element

				const char * MissingName = Context.MyTable[EltIdx].GetCallbacks.DebugName;

#endif // DEBUG

				return false;

			}

		}

	}

	return true;

}



EbmlElement *EbmlMaster::FindElt(const EbmlCallbacks & Callbacks) const

{

	size_t Index;

	

	for (Index = 0; Index < ElementList.size(); Index++) {

		if (EbmlId(*(ElementList[Index])) == Callbacks.GlobalId)

			return ElementList[Index];

	}



	return NULL;

}



EbmlElement *EbmlMaster::FindFirstElt(const EbmlCallbacks & Callbacks, const bool bCreateIfNull)

{

	size_t Index;

	

	for (Index = 0; Index < ElementList.size(); Index++) {

		if (EbmlId(*(ElementList[Index])) == Callbacks.GlobalId)

			return ElementList[Index];

	}

	

	if (bCreateIfNull && Callbacks.Create != NULL) {

		// add the element

		EbmlElement *NewElt = &(Callbacks.Create());

		if (NewElt == NULL)

			return NULL;



		if (!PushElement(*NewElt)) {

			delete NewElt;

			NewElt = NULL;

		}

		return NewElt;

	}

	

	return NULL;

}



EbmlElement *EbmlMaster::FindFirstElt(const EbmlCallbacks & Callbacks) const

{

	size_t Index;

	

	for (Index = 0; Index < ElementList.size(); Index++) {

		if (EbmlId(*(ElementList[Index])) == Callbacks.GlobalId)

			return ElementList[Index];

	}

	

	return NULL;

}



/*!

	\todo only return elements that are from the same type !

	\todo the element might be the unique in the context !

*/

EbmlElement *EbmlMaster::FindNextElt(const EbmlElement & PastElt, const bool bCreateIfNull)

{

	size_t Index;

	

	for (Index = 0; Index < ElementList.size(); Index++) {

		if ((ElementList[Index]) == &PastElt) {

			// found past element, new one is :

			Index++;

			break;

		}

	}



	while (Index < ElementList.size()) {

		if (PastElt.Generic().GlobalId == ElementList[Index]->Generic().GlobalId)

			break;

		Index++;

	}



	if (Index != ElementList.size())

		return ElementList[Index];



	if (bCreateIfNull && PastElt.Generic().Create != NULL) {

		// add the element

		EbmlElement *NewElt = &(PastElt.Generic().Create());

		if (NewElt == NULL)

			return NULL;



		if (!PushElement(*NewElt)) {

			delete NewElt;

			NewElt = NULL;

		}

		return NewElt;

	}



	return NULL;

}



EbmlElement *EbmlMaster::FindNextElt(const EbmlElement & PastElt) const

{

	size_t Index;

	

	for (Index = 0; Index < ElementList.size(); Index++) {

		if ((ElementList[Index]) == &PastElt) {

			// found past element, new one is :

			Index++;

			break;

		}

	}



	while (Index < ElementList.size()) {

		if (PastElt.Generic().GlobalId == ElementList[Index]->Generic().GlobalId)

			return ElementList[Index];

		Index++;

	}



	return NULL;

}



EbmlElement *EbmlMaster::AddNewElt(const EbmlCallbacks & Callbacks)

{

	// add the element

	EbmlElement *NewElt = &(Callbacks.Create());

	if (NewElt == NULL)

		return NULL;



	if (!PushElement(*NewElt)) {

		delete NewElt;

		NewElt = NULL;

	}

	return NewElt;

}



void EbmlMaster::Sort()

{

	std::sort(ElementList.begin(), ElementList.end(), EbmlElement::CompareElements);

}



/*!

	\brief Method to help reading a Master element and all subsequent children quickly

	\todo add an option to discard even unknown elements

	\todo handle when a mandatory element is not found

*/

void EbmlMaster::Read(EbmlStream & inDataStream, const EbmlSemanticContext & Context, int & UpperEltFound, EbmlElement * & FoundElt, bool AllowDummyElt)

{

	EbmlElement * ElementLevelA;

	// remove all existing elements, including the mandatory ones...

	ElementList.clear();

	uint64 MaxSizeToRead = Size;



	// read blocks and discard the ones we don't care about

	ElementLevelA = inDataStream.FindNextElement(Context, UpperEltFound, MaxSizeToRead, AllowDummyElt);

	while (ElementLevelA != NULL) {

		if (UpperEltFound != 0) {

			break;

		}

		MaxSizeToRead -= ElementLevelA->ElementSize();

		if (ElementLevelA->IsDummy()) {

			ElementLevelA->SkipData(inDataStream, Context);

			delete ElementLevelA; // forget this unknown element



			ElementLevelA = inDataStream.FindNextElement(Context, UpperEltFound, MaxSizeToRead, AllowDummyElt);

		} else {

			ElementList.push_back(ElementLevelA);

			/// \todo read the children data

			ElementLevelA->Read(inDataStream, ElementLevelA->Generic().Context, UpperEltFound, FoundElt, AllowDummyElt);



			if (UpperEltFound > 0) {

				UpperEltFound--;

				if (UpperEltFound > 0)

					return;

				ElementLevelA = FoundElt;

				continue;

			} else {

				ElementLevelA->SkipData(inDataStream, ElementLevelA->Generic().Context);



				ElementLevelA = inDataStream.FindNextElement(Context, UpperEltFound, MaxSizeToRead, AllowDummyElt);

			}

		}

	}

	if (UpperEltFound > 0) {

		FoundElt = ElementLevelA;

	}

}



END_LIBEBML_NAMESPACE

