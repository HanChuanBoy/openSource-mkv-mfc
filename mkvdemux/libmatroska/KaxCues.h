/****************************************************************************
** libmatroska : parse Matroska files, see http://www.matroska.org/
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
	\version \$Id: KaxCues.h,v 1.8 2003/06/06 23:31:30 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_CUES_H
#define LIBMATROSKA_CUES_H

#include <vector>

#include "KaxTypes.h"
#include "EbmlMaster.h"
#include "KaxBlock.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxCuePoint;

class KaxCues : public EbmlMaster {
	public:
		KaxCues();
		~KaxCues();
		static EbmlElement & Create() {return *(new KaxCues);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}

		bool AddBlockGroup(const KaxBlockGroup & BlockReference);

		/*!
			\brief Indicate that the position for this Block is set
		*/
		void PositionSet(const KaxBlockGroup & BlockReference);

		/*!
			\brief override to sort by timecode/track
		*/
		uint32 Render(IOCallback & output, bool bSaveDefault = false) {
			Sort();
			return EbmlMaster::Render(output, bSaveDefault);
		}

		uint64 GetTimecodePosition(uint64 aTimecode) const;
		const KaxCuePoint * GetTimecodePoint(uint64 aTimecode) const;

		void SetGlobalTimecodeScale(uint64 aGlobalTimecodeScale) {
			mGlobalTimecodeScale = aGlobalTimecodeScale;
			bGlobalTimecodeScaleIsSet = true;
		}
		uint64 GlobalTimecodeScale() const {
			assert(bGlobalTimecodeScaleIsSet); 
			return mGlobalTimecodeScale;
		}

	protected:
		std::vector<const KaxBlockGroup *> myTempReferences;
		bool   bGlobalTimecodeScaleIsSet;
		uint64 mGlobalTimecodeScale;
};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_CUES_H
