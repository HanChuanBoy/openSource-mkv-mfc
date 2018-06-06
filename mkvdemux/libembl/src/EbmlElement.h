/****************************************************************************
** libebml : parse EBML files, see http://embl.sourceforge.net/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Steve Lhomme.  All rights reserved.
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
	\version \$Id: EbmlElement.h,v 1.14 2003/05/30 10:15:38 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBEBML_ELEMENT_H
#define LIBEBML_ELEMENT_H

#include <cassert>

#include "EbmlConfig.h"
#include "EbmlTypes.h"
#include "EbmlId.h"
#include "IOCallback.h"

START_LIBEBML_NAMESPACE

class EbmlStream;
class EbmlSemanticContext;
class EbmlElement;

// functions for generic handling of data (should be static to all classes)
/*!
	\todo Handle default value
*/
class EbmlCallbacks {
	public:
		//´«µÝ;
		EbmlCallbacks(EbmlElement & (*Creator)(), const EbmlId & aGlobalId, const char * aDebugName, const EbmlSemanticContext & aContext)
			:Create(Creator)
			,GlobalId(aGlobalId)
			,DebugName(aDebugName)
			,Context(aContext)
		{}

		EbmlElement & (*Create)();
		const EbmlId & GlobalId;
		const char * DebugName;
		const EbmlSemanticContext & Context;
};

/*!
	\brief contains the semantic informations for a given level and all sublevels
	\todo move the ID in the element class
*/
class EbmlSemantic {
	public:
		EbmlSemantic(bool aMandatory, bool aUnique, const EbmlCallbacks & aGetCallbacks)
			:Mandatory(aMandatory), Unique(aUnique), GetCallbacks(aGetCallbacks) {}

		bool Mandatory; ///< wether the element is mandatory in the context or not
		bool Unique;
		const EbmlCallbacks & GetCallbacks;
};

typedef const class EbmlSemanticContext & (*_GetSemanticContext)();

/*!
	Context of the element
	\todo allow more than one parent ?
*/
class EbmlSemanticContext {
	public:
		EbmlSemanticContext(unsigned int aSize,
			const EbmlSemantic *aMyTable,
			const EbmlSemanticContext *aUpTable,
			const _GetSemanticContext aGetGlobalContext,
			const EbmlCallbacks *aMasterElt)
			:Size(aSize), MyTable(aMyTable), UpTable(aUpTable),
			 GetGlobalContext(aGetGlobalContext), MasterElt(aMasterElt) {}

		bool operator!=(const EbmlSemanticContext & aElt) const {
			return ((Size != aElt.Size) || (MyTable != aElt.MyTable) ||
				(UpTable != aElt.UpTable) || (GetGlobalContext != aElt.GetGlobalContext) |
				(MasterElt != aElt.MasterElt));
		}
		unsigned int Size;          ///< number of elements in the table
		const EbmlSemantic *MyTable; ///< First element in the table
		const EbmlSemanticContext *UpTable; ///< Parent element
		/// \todo replace with the global context directly
		const _GetSemanticContext GetGlobalContext; ///< global elements supported at this level
		const EbmlCallbacks *MasterElt;
};

/*!
	\class EbmlElement
	\brief Hold basic informations about an EBML element (ID + length)
*/
class EbmlElement {
	public:
		EbmlElement(const uint64 aDefaultSize, bool bValueSet = false);
		virtual ~EbmlElement() {assert(!bLocked);}
	
		/// Set the minimum length that will be used to write the element size (-1 = optimal)
		void SetSizeLength(const int NewSizeLength) {SizeLength = NewSizeLength;}
		int GetSizeLength() const {return SizeLength;}
		
		static EbmlElement * FindNextElement(IOCallback & DataStream, const EbmlSemanticContext & Context, int & UpperLevel, uint64 MaxDataSize, bool AllowDummyElt, unsigned int MaxLowerLevel = 1);
		static EbmlElement * FindNextID(IOCallback & DataStream, const EbmlCallbacks & ClassInfos, const uint64 MaxDataSize);

		/*!
			\brief find the next element with the same ID
		*/
		EbmlElement * FindNext(IOCallback & DataStream, const uint64 MaxDataSize);

		EbmlElement * SkipData(EbmlStream & DataStream, const EbmlSemanticContext & Context, EbmlElement * TestReadElt = NULL, bool AllowDummyElt = false);

		virtual operator const EbmlId &() const = 0;

		// by default only allow to set element as finite (override when needed)
		virtual bool SetSizeInfinite(bool bIsInfinite = true) {return !bIsInfinite;}

		virtual bool ValidateSize() const = 0;

		uint64 GetElementPosition() const {return ElementPosition;}

		uint64 ElementSize(bool bSaveDefault = false) const; /// return the size of the header+data
		
		uint32 Render(IOCallback & output, bool bSaveDefault = false);

		virtual uint64 UpdateSize(bool bSaveDefault = false) = 0; /// update the Size of the Data stored
		virtual uint64 GetSize() const {return Size;}

		virtual uint64 ReadData(IOCallback & input) = 0;
		virtual void Read(EbmlStream & inDataStream, const EbmlSemanticContext & Context, int & UpperEltFound, EbmlElement * & FoundElt, bool AllowDummyElt = false);
		
		/// return the generic callback to monitor a derived class
		virtual const EbmlCallbacks & Generic() const = 0;

		bool IsLocked() const {return bLocked;}
		void Lock(bool bLock = true) { bLocked = bLock;}

		/*!
			\brief default comparison for elements that can't be compared
		*/
		virtual bool operator<(const EbmlElement & EltB) const {
			return true;
		}

		static bool CompareElements(const EbmlElement *A, const EbmlElement *B);

		virtual bool IsDummy() const {return false;}

		uint8 HeadSize() const {return EbmlId(*this).Length + CodedSizeLength();}
		
		/*!
			\brief Force the size of an element
			\warning only possible if the size is "undefined"
		*/
		bool ForceSize(uint64 NewSize);

		uint32 OverwriteHead(IOCallback & output);

		/*!
			\brief void the content of the element (replace by EbmlVoid)
		*/
		uint32 VoidMe(IOCallback & output, bool bSaveDefault = false);

		bool DefaultISset() const {return DefaultIsSet;}
		virtual bool IsDefaultValue() const = 0;
		bool IsFiniteSize() const {return bSizeIsFinite;}

		/*!
			\brief set the default size of an element
		*/
		virtual void SetDefaultSize(const uint64 aDefaultSize) {DefaultSize = aDefaultSize;}
		
	protected:
		uint64 Size;        ///< the size of the data to write
		uint64 DefaultSize; ///< Minimum data size to fill on rendering (0 = optimal)
		int SizeLength; /// the minimum size on which the size will be written (0 = optimal)
		int CodedSizeLength() const;
		bool bSizeIsFinite;
		uint64 ElementPosition;
		uint64 SizePosition;
		bool bValueIsSet;
		bool DefaultIsSet;
		bool bLocked;

		/*!
			\brief find any element in the stream
			\return a DummyRawElement if the element is unknown or NULL if the element dummy is not allowed
		*/
		static EbmlElement *CreateElementUsingContext(const EbmlId & aID, const EbmlSemanticContext & Context, int & LowLevel, bool bAllowDummy = false, unsigned int MaxLowerLevel = 1);

		uint32 RenderHead(IOCallback & output, bool bSaveDefault = false);
		uint32 MakeRenderHead(IOCallback & output);
	
		/*!
			\brief prepare the data before writing them (in case it's not already done by default)
		*/
		virtual uint32 RenderData(IOCallback & output, bool bSaveDefault = false) = 0;
};

END_LIBEBML_NAMESPACE

#endif // LIBEBML_ELEMENT_H
