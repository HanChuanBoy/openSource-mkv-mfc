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
	\version \$Id: KaxCluster.h,v 1.13 2003/06/07 11:36:15 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
	\author Julien Coloos    <suiryc @ users.sf.net>

*/
#ifndef LIBMATROSKA_CLUSTER_H
#define LIBMATROSKA_CLUSTER_H

#include "KaxTypes.h"
#include "EbmlMaster.h"
#include "KaxTracks.h"
#include "KaxBlock.h"
#include "KaxCues.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxSegment;

class KaxCluster : public EbmlMaster {
	public:
		KaxCluster();
		static EbmlElement & Create() {return *(new KaxCluster);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}

		/*!
			\brief Addition of a frame without references

			\param the timecode is expressed in nanoseconds, relative to the beggining of the Segment
		*/
		bool AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, KaxBlockGroup * & MyNewBlock);
		/*!
			\brief Addition of a frame with a backward reference (P frame)
			\param the timecode is expressed in nanoseconds, relative to the beggining of the Segment

		*/
		bool AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, KaxBlockGroup * & MyNewBlock, const KaxBlockGroup & PastBlock);

		/*!
			\brief Addition of a frame with a backward+forward reference (B frame)
			\param the timecode is expressed in nanoseconds, relative to the beggining of the Segment

		*/
		bool AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, KaxBlockGroup * & MyNewBlock, const KaxBlockGroup & PastBlock, const KaxBlockGroup & ForwBlock);

		/*!
			\brief Render the data to the stream and retrieve the position of BlockGroups for later cue entries
		*/
		uint32 Render(IOCallback & output, KaxCues & CueToUpdate, bool bSaveDefault = false);

		/*!
			\return the global timecode of this Cluster
		*/
		uint64 GlobalTimecode() const;

		KaxBlockGroup & GetNewBlock();
		
		/*!
			\brief release all the frames of all Blocks
			\note this is a convenience to be able to keep Clusters+Blocks in memory (for future reference) withouht being a memory hog
		*/
		void ReleaseFrames();

		/*!
			\brief return the position offset compared to the beggining of the Segment
		*/
		uint64 GetPosition() const;

		void SetParent(const KaxSegment & aParentSegment) {ParentSegment = &aParentSegment;}

		void SetPreviousTimecode(uint64 aPreviousTimecode, int64 aTimecodeScale) {
			bPreviousTimecodeIsSet = true; 
			PreviousTimecode = aPreviousTimecode;
			SetGlobalTimecodeScale(aTimecodeScale);
		}

		/*!
			\note dirty hack to get the mandatory data back after reading
			\todo there should be a better way to get mandatory data
		*/
		void InitTimecode(uint64 aTimecode, int64 aTimecodeScale) {
			SetGlobalTimecodeScale(aTimecodeScale);
			MinTimecode = MaxTimecode = PreviousTimecode = aTimecode * TimecodeScale;
			bFirstFrameInside = bPreviousTimecodeIsSet = true;
		}

		int16 GetBlockLocalTimecode(uint64 GlobalTimecode) const;

		uint64 GetBlockGlobalTimecode(int16 LocalTimecode);

		void SetGlobalTimecodeScale(uint64 aGlobalTimecodeScale) {
			TimecodeScale = aGlobalTimecodeScale;
			bTimecodeScaleIsSet = true;
		}
		uint64 GlobalTimecodeScale() const {
			assert(bTimecodeScaleIsSet); 
			return TimecodeScale;
		}

	protected:
		KaxBlockGroup    * currentNewBlock;
		const KaxSegment * ParentSegment;

		uint64 MinTimecode, MaxTimecode, PreviousTimecode;
		int64  TimecodeScale;

		bool bFirstFrameInside; // used to speed research
		bool bPreviousTimecodeIsSet;
		bool bTimecodeScaleIsSet;

		/*!
			\note method used internally
		*/
		bool AddFrameInternal(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, KaxBlockGroup * & MyNewBlock, const KaxBlockGroup * PastBlock, const KaxBlockGroup * ForwBlock);

};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_CLUSTER_H
