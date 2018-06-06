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

	\version \$Id: EbmlElement.cpp,v 1.29 2003/06/08 23:23:58 robux4 Exp $

	\author Steve Lhomme     <robux4 @ users.sf.net>

*/



#include <cassert>



#include "StdInclude.h"

#include "EbmlElement.h"

#include "EbmlMaster.h"

#include "EbmlStream.h"

#include "EbmlVoid.h"

#include "EbmlContexts.h"



START_LIBEBML_NAMESPACE



/*!

	\brief Use this class at your own risk, it's just a place holder

*/

class DummyRawElement : public EbmlElement {

	public:

		DummyRawElement(const uint64 DefaultSize, const EbmlId & aId) :EbmlElement(DefaultSize), DummyId(aId) {}

		operator const EbmlId &() const {return DummyId;}

		bool SetSizeInfinite(bool aIsInfinite = true) {bSizeIsFinite = !aIsInfinite; return true;}

		bool ValidateSize() const {return true;} // as we don't know anything about the content, we silently agree



		uint64 UpdateSize(bool bSaveDefault = false) { assert(0); return 0;}

		uint64 ReadData(IOCallback & input) {assert(0); return 0;}

		const EbmlCallbacks & Generic() const {return ClassInfos;}

		bool IsDummy() const {return true;}

		static const EbmlCallbacks ClassInfos;

		bool IsDefaultValue() const {return true;}



	protected:

		const EbmlId DummyId;

		static const EbmlId DummyRawId;


		uint32 RenderData(IOCallback & output, bool bSaveDefault) {assert(0); return 0;}

};



const EbmlId DummyRawElement::DummyRawId(0xFF, 1);

const EbmlSemanticContext DummyRawElement_Context = EbmlSemanticContext(0, NULL, NULL, *GetEbmlGlobal_Context, &DummyRawElement::ClassInfos);

const EbmlCallbacks DummyRawElement::ClassInfos(NULL, DummyRawId, "DummyElement", DummyRawElement_Context);



EbmlElement::EbmlElement(const uint64 aDefaultSize, bool bValueSet)

 :DefaultSize(aDefaultSize)

 ,SizeLength(0) ///< write optimal size by default

 ,bSizeIsFinite(true)

 ,ElementPosition(0)

 ,SizePosition(0)

 ,bValueIsSet(bValueSet)

 ,DefaultIsSet(false)

 ,bLocked(false)

{

	Size = DefaultSize;

}



/*!

	\todo handle more than CodedSize of 5

*/

int EbmlElement::CodedSizeLength() const
{

	int CodedSize;

	// prepare the head of the size (000...01xxxxxx)

	// optimal size

	if (Size < 127) // 2^7 - 1

		CodedSize = 1;

	else if (Size < 16383) // 2^14 - 1

		CodedSize = 2;

	else if (Size < 2097151L) // 2^21 - 1

		CodedSize = 3;

	else if (Size < 268435455L) // 2^28 - 1

		CodedSize = 4;

	else CodedSize = 5;


	if (SizeLength > 0 && CodedSize < SizeLength) {

		// defined size

		CodedSize = SizeLength;

	}



	return CodedSize;

}



/*!

	\todo this method is deprecated and should be called FindThisID

	\todo replace the new RawElement with the appropriate class (when known)

*/

EbmlElement * EbmlElement::FindNextID(IOCallback & DataStream, const EbmlCallbacks & ClassInfos, const uint64 MaxDataSize)

{

	binary PossibleId[4];

	int PossibleID_Length = 0;

	binary PossibleSize[8]; // we don't support size stored in more than 64 bits

	unsigned int PossibleSizeLength = 0;

	uint64 SizeUnknown;

	bool bElementFound = false;

	

	binary BitMask;

	uint64 aElementPosition, aSizePosition;

	while (!bElementFound) {

		// read ID

		aElementPosition = DataStream.getFilePointer();

		uint32 ReadSize = 0;

		BitMask = 1 << 7;

		while (1) {

			ReadSize += DataStream.read(&PossibleId[PossibleID_Length], 1);

			if (ReadSize == uint32(PossibleID_Length)) {

				return NULL; // no more data ?

			}

			if (++PossibleID_Length > 4) {

				return NULL; // we don't support element IDs over class D

			}

			if (PossibleId[0] & BitMask) {

				// this is the last octet of the ID

				// check wether that's the one we're looking for

/*			if (PossibleID == ClassInfos.GlobalId) {

					break;

				} else {

					/// \todo This element should be skipped (use a context ?)

				}*/

				bElementFound = true; /// \todo not exactly the one we're looking for

				break;

			}

			BitMask >>= 1;

		}

		

		// read the data size

		aSizePosition = DataStream.getFilePointer();

		SizeUnknown = 0x7F; // the last bit is discarded when computing the size

		BitMask = 1 << 7;

		while (1) {

			ReadSize += DataStream.read(&PossibleSize[PossibleSizeLength], 1);

			if (ReadSize == PossibleSizeLength) {

				return NULL; // no more data ?

			}

			if (++PossibleSizeLength > 8) {

				return NULL; // we don't support element IDs over class D

			}

			if (PossibleSize[0] & BitMask) {

				// this is the last octet of the size
				break;
			}

			BitMask >>= 1;

			SizeUnknown <<= 7;

			SizeUnknown |= 0xFF;

		}

	}

	

	EbmlElement *Result = NULL;

	EbmlId PossibleID(PossibleId, PossibleID_Length);

	if (PossibleID == ClassInfos.GlobalId) {

		// the element is the one expected

		Result = &ClassInfos.Create();

	} else {

		/// \todo find the element in the context

		Result = new DummyRawElement(0, PossibleID);

	}

	

	Result->SetSizeLength(PossibleSizeLength);

	

	Result->Size = 0;

	Result->Size |= PossibleSize[0] & ~BitMask;

	for (unsigned int i = 1; i<PossibleSizeLength; i++) {

		Result->Size <<= 8;

		Result->Size |= PossibleSize[i];

	}



	if (!Result->ValidateSize() || (Result->Size != SizeUnknown && MaxDataSize < Result->Size)) {

			delete Result;

			return NULL;

	}



	// check if the size is not all 1s

	if (Result->Size == SizeUnknown) {

		// Size of this element is unknown

		// only possible for Master elements

		if (!Result->SetSizeInfinite()) {

			/// \todo the element is not allowed to be infinite

			delete Result;

			return NULL;

		}

	} else Result->SetSizeInfinite(false);

	Result->ElementPosition = aElementPosition;

	Result->SizePosition = aSizePosition;

	

	return Result;

}





/*!

	\todo replace the new RawElement with the appropriate class (when known)

	\todo skip data for Dummy elements when they are not allowed

	\todo better check of the size checking for upper elements (using a list of size for each level)

	\param LowLevel Will be returned with the level of the element found compared to the context given

*/

EbmlElement * EbmlElement::FindNextElement(IOCallback & DataStream, const EbmlSemanticContext & Context, int & UpperLevel, 

			uint64 MaxDataSize, bool AllowDummyElt, unsigned int MaxLowerLevel)

{

	binary PossibleId[4];

	int PossibleID_Length;

	binary PossibleSize[8]; // we don't support size stored in more than 64 bits

	binary PossibleIdNSize[16];

	int PossibleSizeLength;

	uint64 SizeUnknown;

	int ReadIndex = 0; // trick for the algo, start index at 0

	uint32 ReadSize = 0;

	int SizeIdx;

	int UpperLevel_original = UpperLevel;

	

	while (1) {

		// read a potential ID

		bool bFound, bValid;

		do {

			assert(ReadIndex < 16);

			// build the ID with the current Read Buffer

			bFound = false;

			binary IdBitMask = 1 << 7;

			for (SizeIdx = 0; SizeIdx < ReadIndex && SizeIdx < 4; SizeIdx++) {

				if (PossibleIdNSize[0] & (IdBitMask >> SizeIdx)) {

					// ID found

					PossibleID_Length = SizeIdx + 1;

					IdBitMask >>= SizeIdx;

					for (SizeIdx = 0; SizeIdx < PossibleID_Length && SizeIdx < 4; SizeIdx++) {

						PossibleId[SizeIdx] = PossibleIdNSize[SizeIdx];

					}

					bFound = true;

					break;

				}

			}

			if (bFound) {

				break;

			}



			if (ReadIndex >= 4) {

				// ID not found

				// shift left the read octets

				memmove(&PossibleIdNSize[0],&PossibleIdNSize[1], --ReadIndex);

			}



			if (DataStream.read(&PossibleIdNSize[ReadIndex++], 1) == 0) {

				return NULL; // no more data ?

			}

			ReadSize++;



		} while (!bFound);



		ReadIndex -= PossibleID_Length;

		memmove(&PossibleIdNSize[0],&PossibleIdNSize[PossibleID_Length], ReadIndex);



		// read the data size

		binary SizeBitMask;

		do {

			// build the ID with the current Read Buffer

			bValid = true;

			bFound = false;

			SizeBitMask = 1 << 7;

			SizeUnknown = 0x7F;

			for (SizeIdx = 0; SizeIdx < ReadIndex && SizeIdx < 8; SizeIdx++) {

				if (PossibleIdNSize[0] & (SizeBitMask >> SizeIdx)) {

					// ID found

					PossibleSizeLength = SizeIdx + 1;

					SizeBitMask >>= SizeIdx;

					for (SizeIdx = 0; SizeIdx < PossibleSizeLength; SizeIdx++) {

						PossibleSize[SizeIdx] = PossibleIdNSize[SizeIdx];

					}

					for (SizeIdx = 0; SizeIdx < PossibleSizeLength - 1; SizeIdx++) {

						SizeUnknown <<= 7;

						SizeUnknown |= 0xFF;

					}

					bFound = true;

					break;

				}

			}



			if (bFound) {

				break;

			}



			if (ReadIndex > 8) {

				// Size not found and it should be next to the ID, so this is a bad ID

				bValid = false;

				break;

			}



			if (DataStream.read(&PossibleIdNSize[ReadIndex++], 1) == 0) {

				return NULL; // no more data ?

			}

			ReadSize++;



		} while (!bFound);



		if (bValid) {

			// find the element in the context and use the correct creator

			EbmlId PossibleID(PossibleId, PossibleID_Length);

			EbmlElement * Result = CreateElementUsingContext(PossibleID, Context, UpperLevel, AllowDummyElt, MaxLowerLevel);

			///< \todo continue is misplaced

			if (Result != NULL) {

				if (AllowDummyElt || !Result->IsDummy()) {

					Result->SetSizeLength(PossibleSizeLength);

					

					Result->Size = 0;

					Result->Size |= PossibleSize[0] & ~SizeBitMask;

					for (unsigned int i = 1; i<(unsigned int)PossibleSizeLength; i++) {

						Result->Size <<= 8;

						Result->Size |= PossibleSize[i];

					}



					if (Result->ValidateSize() && (UpperLevel > 0 || MaxDataSize >= Result->Size)) {

						if (Result->Size == SizeUnknown) {

							Result->SetSizeInfinite();

						}



						if (UpperLevel < 0) {

							// this is a global element

							// no need to change the level

							UpperLevel = 0;

						}



						Result->SizePosition = DataStream.getFilePointer() - ReadIndex;

						Result->ElementPosition = Result->SizePosition - PossibleID.Length;

						// place the file at the beggining of the data

						DataStream.setFilePointer(Result->SizePosition + PossibleSizeLength);

						return Result;

					}

				}

				delete Result;

			}

		}



		// recover all the data in the buffer minus one byte

		memmove(&PossibleIdNSize[PossibleID_Length - 1], &PossibleIdNSize[0], ReadIndex);

		memcpy(&PossibleIdNSize[0], &PossibleId[1], PossibleID_Length - 1);

		ReadIndex += PossibleID_Length - 1;

		UpperLevel = UpperLevel_original;

	}



	return NULL;

}



/*!

	\todo what happens if we are in a upper element with a known size ?

*/

EbmlElement * EbmlElement::SkipData(EbmlStream & DataStream, const EbmlSemanticContext & Context, EbmlElement * TestReadElt, bool AllowDummyElt)

{

	EbmlElement * Result = NULL;

	if (bSizeIsFinite) {

		assert(TestReadElt == NULL);

		assert(ElementPosition < SizePosition);

		DataStream.I_O().setFilePointer(SizePosition + CodedSizeLength() + Size, seek_beginning);

//		DataStream.I_O().setFilePointer(Size, seek_current);

	} else {

		/////////////////////////////////////////////////

		// read elements until an upper element is found

		/////////////////////////////////////////////////

		bool bEndFound = false;

		while (!bEndFound && Result == NULL) {

			// read an element

			/// \todo 0xFF... and true should be configurable

//			EbmlElement * NewElt;

			if (TestReadElt == NULL) {

				int bUpperElement = 0; // trick to call FindNextID correctly

				Result = DataStream.FindNextElement(Context, bUpperElement, 0xFFFFFFFFL, AllowDummyElt);

			} else {

				Result = TestReadElt;

			}

			

			if (Result != NULL) {

				unsigned int EltIndex;

				// data known in this Master's context

				for (EltIndex = 0; EltIndex < Context.Size; EltIndex++) {

					if (EbmlId(*Result) == Context.MyTable[EltIndex].GetCallbacks.GlobalId) {

						// skip the data with its own context

						Result = Result->SkipData(DataStream, Context.MyTable[EltIndex].GetCallbacks.Context, NULL);

						break; // let's go to the next ID

					}

				}



				if (EltIndex >= Context.Size) {

					if (Context.UpTable != NULL) {

						Result = SkipData(DataStream, *Context.UpTable, Result);

					} else {

						assert(Context.GetGlobalContext != NULL);

						if (Context != Context.GetGlobalContext()) {

							Result = SkipData(DataStream, Context.GetGlobalContext(), Result);

						} else {

							bEndFound = true;

						}

					}

				}

			} else {

				bEndFound = true;

			}

		}

	}

	return Result;

}



EbmlElement *EbmlElement::CreateElementUsingContext(const EbmlId & aID, const EbmlSemanticContext & Context,

													int & LowLevel, bool bAllowDummy, unsigned int MaxLowerLevel)

{

	unsigned int ContextIndex;



	if (Context.MasterElt != NULL && aID == Context.MasterElt->GlobalId) {

		LowLevel++; // already one level up (same as context)

		return &Context.MasterElt->Create();

	}



	for (ContextIndex = 0; ContextIndex < Context.Size; ContextIndex++) {

		if (aID == Context.MyTable[ContextIndex].GetCallbacks.GlobalId) {

			return &Context.MyTable[ContextIndex].GetCallbacks.Create();

		}

	}



	// check wether it's not part of an upper context

	if (Context.UpTable != NULL) {

		LowLevel++;

		MaxLowerLevel++;

		return CreateElementUsingContext(aID, *Context.UpTable, LowLevel, bAllowDummy, MaxLowerLevel);

	}

	LowLevel--;

	MaxLowerLevel--;



	EbmlElement *Result = NULL;

//	if (MaxLowerLevel > 1) 

	{

		assert(Context.GetGlobalContext != NULL); // global should always exist, at least the EBML ones

		const EbmlSemanticContext & tstContext = Context.GetGlobalContext();

		if (tstContext != Context) {

			// recursive is good, but be carefull...

			Result = CreateElementUsingContext(aID, Context.GetGlobalContext(), LowLevel, bAllowDummy, MaxLowerLevel);

			if (Result != NULL) {

				return Result;

			}

		}

	}



	if (bAllowDummy) {

		LowLevel = 0;

		Result = new DummyRawElement(0, aID);

	}



	return Result;

}



/*!

	\todo verify that the size written is the same as the data written

*/

uint32 EbmlElement::Render(IOCallback & output, bool bSaveDefault)

{

	assert(bValueIsSet); // an element is been rendered without a value set !!!

		                 // it may be a mandatory element without a default value

	try {

		if (!bSaveDefault && IsDefaultValue()) {

			return 0;

		}

//#if defined(_DEBUG) || defined(DEBUG)

		uint64 SupposedSize = UpdateSize(bSaveDefault);

//#endif // _DEBUG

		uint32 result = RenderHead(output, bSaveDefault);

		uint64 WrittenSize = RenderData(output, bSaveDefault);

#if defined(_DEBUG) || defined(DEBUG)

	if (SupposedSize != (0-1)) assert(WrittenSize == SupposedSize);

#endif // DEBUG

		result += WrittenSize;

		return result;

	} catch (...) {

		assert(false); // we should never be here !

		return 0;

	}

}



/*!

	\todo store the position of the Size writing for elements with unknown size

	\todo handle exceptions on errors

	\todo handle CodeSize bigger than 5 bytes

*/

uint32 EbmlElement::RenderHead(IOCallback & output, bool bSaveDefault)

{

	if (EbmlId(*this).Length <= 0 || EbmlId(*this).Length > 4)

		return 0;

	

	UpdateSize(bSaveDefault);

	

	return MakeRenderHead(output);

}

	

uint32 EbmlElement::MakeRenderHead(IOCallback & output)

{

	binary FinalHead[4+8]; // Class D + 64 bits coded size

	unsigned int FinalHeadSize;

	

	FinalHeadSize = EbmlId(*this).Length;

	EbmlId(*this).Fill(FinalHead);



	int CodedSize = CodedSizeLength();



	// Set the EBML bits

	FinalHead[FinalHeadSize] = 1 << (8 - CodedSize);
	//后面位表达codesize的个数;
	

	uint64 TempSize = GetSize();

	int SizeMask = 0xFF;

	for (int i=1; i<CodedSize; i++) {

		FinalHead[FinalHeadSize + CodedSize - i] = TempSize & 0xFF; //256取余数;

		TempSize >>= 8;  //256取得数;

		SizeMask >>= 1; 
		//如果CodedSize不为1为更大的数，那么需要返回对应大小;
	}

//	SizeMask <<= 1;



	// first one use a OR with the "EBML size head"

	FinalHead[FinalHeadSize] |= TempSize & 0xFF & SizeMask;



	FinalHeadSize += CodedSize;

	

	output.writeFully(FinalHead, FinalHeadSize);

	ElementPosition = output.getFilePointer() - FinalHeadSize;

	SizePosition = ElementPosition + EbmlId(*this).Length;

	

	return FinalHeadSize;

}



uint64 EbmlElement::ElementSize(bool bSaveDefault) const

{

	if (!bSaveDefault && IsDefaultValue())

		return 0; // won't be saved

	return Size + EbmlId(*this).Length + CodedSizeLength();

}



bool EbmlElement::CompareElements(const EbmlElement *A, const EbmlElement *B)

{

	if (EbmlId(*A) == EbmlId(*B))

		return *A < *B;

	else

		return false;

}



void EbmlElement::Read(EbmlStream & inDataStream, const EbmlSemanticContext & Context, int & UpperEltFound, EbmlElement * & FoundElt, bool AllowDummyElt)

{

	ReadData(inDataStream.I_O());

}



bool EbmlElement::ForceSize(uint64 NewSize)

{

	if (bSizeIsFinite) {

		return false;

	}



	int OldSizeLen = CodedSizeLength();

	uint64 OldSize = Size;



	Size = NewSize;



	if (CodedSizeLength() == OldSizeLen) {

		bSizeIsFinite = true;

		return true;

	}

	Size = OldSize;



	return false;

}



uint32 EbmlElement::OverwriteHead(IOCallback & output)

{

	if (ElementPosition == 0) {

		return 0; // the element has not been written

	}



	uint64 CurrentPosition = output.getFilePointer();

	output.setFilePointer(GetElementPosition());

	uint32 Result = MakeRenderHead(output);

	output.setFilePointer(CurrentPosition);

	return Result;

}



uint32 EbmlElement::VoidMe(IOCallback & output, bool bSaveDefault)

{

	if (ElementPosition == 0) {

		return 0; // the element has not been written

	}



	EbmlVoid Dummy;

	return Dummy.Overwrite(*this, output, bSaveDefault);

}



END_LIBEBML_NAMESPACE

