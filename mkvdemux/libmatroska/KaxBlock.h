/****************************************************************************

** libmatroska : parse Matroska files, see http://www.matroska.org/

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

	\todo add a PureBlock class to group functionalities between Block and BlockVirtual

	\version \$Id: KaxBlock.h,v 1.17 2003/06/08 22:56:32 robux4 Exp $

	\author Steve Lhomme     <robux4 @ users.sf.net>

	\author Julien Coloos    <suiryc @ users.sf.net>

*/

#ifndef LIBMATROSKA_BLOCK_H

#define LIBMATROSKA_BLOCK_H



#include <vector>



#include "KaxTypes.h"

#include "EbmlBinary.h"

#include "EbmlMaster.h"

#include "KaxTracks.h"



using namespace LIBEBML_NAMESPACE;



START_LIBMATROSKA_NAMESPACE



class KaxCluster;

class KaxReferenceBlock;



class DataBuffer {

	protected:

		binary * myBuffer;

		uint32   mySize;

		bool     bValidValue;

		bool     (*myFreeBuffer)(const DataBuffer & aBuffer); // method to free the internal buffer



	public:

		DataBuffer(binary * aBuffer, uint32 aSize, bool (*aFreeBuffer)(const DataBuffer & aBuffer) = NULL)

			:myBuffer(aBuffer)

			,mySize(aSize)

			,bValidValue(true)	

      ,myFreeBuffer(aFreeBuffer)

		{}

		virtual binary * Buffer() {return myBuffer;}

		virtual uint32   & Size() {return mySize;};

		virtual const binary * Buffer() const {return myBuffer;}

		virtual const uint32   Size()   const {return mySize;};

		bool    FreeBuffer(const DataBuffer & aBuffer) {

			bool bResult = true;

			if (myBuffer != NULL && myFreeBuffer != NULL && bValidValue) {

				bResult = myFreeBuffer(aBuffer);

				myBuffer = NULL;

				bValidValue = false;

			}

			return bResult;

		}

};



/*!

	\warning the binary buffer should be allocated with the new binary* operator : "new binary[your_size]"

*/

class SimpleDataBuffer : public DataBuffer {

	public:

		SimpleDataBuffer(binary * aBuffer, uint32 aSize, uint32 aOffset, bool (*aFreeBuffer)(const DataBuffer & aBuffer) = myFreeBuffer)

			:DataBuffer(aBuffer + aOffset, aSize, aFreeBuffer)

			,Offset(aOffset)

			,BaseBuffer(aBuffer)

		{}



		uint32   & Size()   {return mySize;};

		binary * Buffer() {return myBuffer;}

		const binary * Buffer() const {return myBuffer;}

		virtual const uint32   Size()   const {return mySize;};



	protected:

		uint32 Offset;

		binary * BaseBuffer;



		static bool myFreeBuffer(const DataBuffer & aBuffer)

		{

			delete[] static_cast<const SimpleDataBuffer*>(&aBuffer)->BaseBuffer;

			return true;

		}

};



/*!

	\note the data is copied locally, it can be freed right away

* /

class NotSoSimpleDataBuffer : public SimpleDataBuffer {

	public:

		NotSoSimpleDataBuffer(binary * aBuffer, uint32 aSize, uint32 aOffset)

			:SimpleDataBuffer(new binary[aSize - aOffset], aSize, 0)

		{

			memcpy(BaseBuffer, aBuffer + aOffset, aSize - aOffset);

		}

};

*/



class KaxBlockGroup : public EbmlMaster {

	public:

		KaxBlockGroup();

		static EbmlElement & Create() {return *(new KaxBlockGroup);}

		const EbmlCallbacks & Generic() const {return ClassInfos;}

		static const EbmlCallbacks ClassInfos;

		operator const EbmlId &() const {return ClassInfos.GlobalId;}



		/*!

			\brief Addition of a frame without references

		*/

		bool AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer);

		/*!

			\brief Addition of a frame with a backward reference (P frame)

		*/

		bool AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, const KaxBlockGroup & PastBlock);



		/*!

			\brief Addition of a frame with a backward+forward reference (B frame)

		*/

		bool AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, const KaxBlockGroup & PastBlock, const KaxBlockGroup & ForwBlock);



		void SetParent(KaxCluster & aParentCluster) {

			ParentCluster = &aParentCluster;

		}



		void SetParentTrack(const KaxTrackEntry & aParentTrack) {

			ParentTrack = &aParentTrack;

		}



		/*!

			\brief Set the duration of the contained frame(s) (for the total number of frames)

		*/

		void SetBlockDuration(uint64 TimeLength);

		bool GetBlockDuration(uint64 &TheTimecode) const;



		/*!

			\return the global timecode of this Block (not just the delta to the Cluster)

		*/

		uint64 GlobalTimecode() const;

		uint64 GlobalTimecodeScale() const {

			assert(ParentTrack != NULL);

			return ParentTrack->GlobalTimecodeScale();

		}



		uint16 TrackNumber() const;



		uint64 ClusterPosition() const;

		

		/*!

			\return the number of references to other frames

		*/

		unsigned int ReferenceCount() const;

		const KaxReferenceBlock & Reference(unsigned int Index) const;



		/*!

			\brief release all the frames of all Blocks

		*/

		void ReleaseFrames();



	protected:

		KaxCluster * ParentCluster;

		const KaxTrackEntry * ParentTrack;

};



class KaxBlock : public EbmlBinary {

	public:

		KaxBlock() :bLocalTimecodeUsed(false), bGap(false), ParentCluster(NULL) {}

		~KaxBlock();

		static EbmlElement & Create() {return *(new KaxBlock);}

		const EbmlCallbacks & Generic() const {return ClassInfos;}

		static const EbmlCallbacks ClassInfos;

		operator const EbmlId &() const {return ClassInfos.GlobalId;}

		bool ValidateSize() const;



		uint16 TrackNum() const {return TrackNumber;}

		/*!

			\todo !!!! This method needs to be changes !

		*/

		uint64 GlobalTimecode() const {return Timecode;}



		/*!

			\note override this function to generate the Data/Size on the fly, unlike the usual binary elements

		*/

		uint64 UpdateSize(bool bSaveDefault = false);

		uint64 ReadData(IOCallback & input);

		

		/*!

			\brief Only read the head of the Block (not internal data)

			\note convenient when you are parsing the file quickly

		*/

		uint64 ReadInternalHead(IOCallback & input);

		

		unsigned int NumberFrames() const { return myBuffers.size();}

		DataBuffer & GetBuffer(unsigned int iIndex) {return *myBuffers[iIndex];}



		bool AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer);



		/*!

			\brief release all the frames of all Blocks

		*/

		void ReleaseFrames();



		void SetParent(KaxCluster & aParentCluster);



	protected:

		std::vector<DataBuffer *> myBuffers;

		uint64 Timecode; // temporary timecode of the first frame, non scaled

		int16  LocalTimecode;

		bool   bLocalTimecodeUsed;

		uint16 TrackNumber;

		bool   bGap;



		uint32 RenderData(IOCallback & output, bool bSaveDefault = false);



		KaxCluster * ParentCluster;

};



class KaxBlockVirtual : public EbmlBinary {

	public:

		KaxBlockVirtual() :ParentCluster(NULL) {Data = DataBlock; Size = countof(DataBlock);}

		static EbmlElement & Create() {return *(new KaxBlockVirtual);}

		const EbmlCallbacks & Generic() const {return ClassInfos;}

		static const EbmlCallbacks ClassInfos;

		operator const EbmlId &() const {return ClassInfos.GlobalId;}

		bool ValidateSize() const {return true;}



		/*!

			\note override this function to generate the Data/Size on the fly, unlike the usual binary elements

		*/

		uint64 UpdateSize(bool bSaveDefault = false);



		void SetParent(const KaxCluster & aParentCluster) {ParentCluster = &aParentCluster;}



	protected:

		uint64 Timecode; // temporary timecode of the first frame if there are more than one

		uint16 TrackNumber;

		binary DataBlock[5];



		const KaxCluster * ParentCluster;

};



class KaxBlockDuration : public EbmlUInteger {

	public:

		KaxBlockDuration() {}

		static EbmlElement & Create() {return *(new KaxBlockDuration);}

		const EbmlCallbacks & Generic() const {return ClassInfos;}

		static const EbmlCallbacks ClassInfos;

		operator const EbmlId &() const {return ClassInfos.GlobalId;}

};



END_LIBMATROSKA_NAMESPACE



#endif // LIBMATROSKA_BLOCK_H

