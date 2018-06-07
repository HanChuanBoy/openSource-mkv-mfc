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
	\version \$Id: KaxBlockData.h,v 1.5 2003/06/11 21:04:22 robux4 Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_BLOCK_ADDITIONAL_H
#define LIBMATROSKA_BLOCK_ADDITIONAL_H

#include "KaxTypes.h"
#include "EbmlMaster.h"
#include "EbmlUInteger.h"
#include "EbmlSInteger.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

class KaxReferenceBlock;
class KaxBlockGroup;

class KaxReferencePriority : public EbmlUInteger {
	public:
		KaxReferencePriority() :EbmlUInteger(0) {}
		static EbmlElement & Create() {return *(new KaxReferencePriority);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

/*!
	\brief element used for B frame-likes
*/
class KaxReferenceBlock : public EbmlSInteger {
	public:
		KaxReferenceBlock() :RefdBlock(NULL), ParentBlock(NULL) {}
		static EbmlElement & Create() {return *(new KaxReferenceBlock);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
		
		/*!
			\brief override this method to compute the timecode value
		*/
		uint64 UpdateSize(bool bSaveDefault = false);

		const KaxBlockGroup & RefBlock() const;
		void SetReferencedBlock(const KaxBlockGroup & aRefdBlock) {RefdBlock = &aRefdBlock; bValueIsSet = true;}
		void SetParentBlock(const KaxBlockGroup & aParentBlock) {ParentBlock = &aParentBlock;}
		
	protected:
		const KaxBlockGroup * RefdBlock;
		const KaxBlockGroup * ParentBlock;
};

class KaxReferenceVirtual : public EbmlSInteger {
	public:
		KaxReferenceVirtual() {}
		static EbmlElement & Create() {return *(new KaxReferenceVirtual);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTimeSlice : public EbmlMaster {
	public:
		KaxTimeSlice();
		static EbmlElement & Create() {return *(new KaxTimeSlice);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxSlices : public EbmlMaster {
	public:
		KaxSlices();
		static EbmlElement & Create() {return *(new KaxSlices);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxSliceLaceNumber : public EbmlUInteger {
	public:
		KaxSliceLaceNumber() :EbmlUInteger(0) {}
		static EbmlElement & Create() {return *(new KaxSliceLaceNumber);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxSliceFrameNumber : public EbmlUInteger {
	public:
		KaxSliceFrameNumber() :EbmlUInteger(0) {}
		static EbmlElement & Create() {return *(new KaxSliceFrameNumber);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxSliceDelay : public EbmlUInteger {
	public:
		KaxSliceDelay() :EbmlUInteger(0) {}
		static EbmlElement & Create() {return *(new KaxSliceDelay);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxSliceDuration : public EbmlUInteger {
	public:
		KaxSliceDuration() {}
		static EbmlElement & Create() {return *(new KaxSliceDuration);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_BLOCK_ADDITIONAL_H
