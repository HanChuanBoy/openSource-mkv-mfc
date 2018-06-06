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
	\version \$Id: EbmlVoid.cpp,v 1.7 2003/05/11 15:43:13 mosu Exp $
	\author Steve Lhomme     <robux4 @ users.sf.net>
*/
#include "StdInclude.h"
#include "EbmlVoid.h"
#include "EbmlContexts.h"

START_LIBEBML_NAMESPACE

EbmlId EbmlVoid_TheId(0xEC, 1);
const EbmlCallbacks EbmlVoid::ClassInfos(EbmlVoid::Create, EbmlVoid_TheId, "EBMLVoid", EbmlVoid_Context);

EbmlVoid::EbmlVoid()
{
	bValueIsSet = true;
}

uint32 EbmlVoid::RenderData(IOCallback & output, bool bSaveDefault)
{
	// write dummy data by 4KB chunks
	static binary DummyBuf[4*1024];

	uint32 SizeToWrite = Size;
	while (SizeToWrite > 4*1024)
	{
		output.writeFully(DummyBuf, 4*1024);
		SizeToWrite -= 4*1024;
	}
	output.writeFully(DummyBuf, SizeToWrite);
	return Size;
}

uint32 EbmlVoid::ReplaceWith(EbmlElement & EltToReplaceWith, IOCallback & output, bool ComeBackAfterward, bool bSaveDefault)
{
	EltToReplaceWith.UpdateSize(bSaveDefault);
	if (HeadSize() + Size < EltToReplaceWith.GetSize() + EltToReplaceWith.HeadSize()) {
		// the element can't be written here !
		return 0;
	}
	if (HeadSize() + Size - EltToReplaceWith.GetSize() - EltToReplaceWith.HeadSize() == 1) {
		// there is not enough space to put a filling element
		return 0;
	}

	uint64 CurrentPosition = output.getFilePointer();

	output.setFilePointer(GetElementPosition());
	EltToReplaceWith.Render(output, bSaveDefault);

	if (HeadSize() + Size - EltToReplaceWith.GetSize() - EltToReplaceWith.HeadSize() > 1) {
	  // fill the rest with another void element
	  EbmlVoid aTmp;
	  aTmp.SetSize(HeadSize() + Size - EltToReplaceWith.GetSize() - EltToReplaceWith.HeadSize() - 1); // 1 is the length of the Void ID
	  int HeadBefore = aTmp.HeadSize();
	  aTmp.SetSize(aTmp.GetSize() - aTmp.CodedSizeLength());
	  int HeadAfter = aTmp.HeadSize();
	  if (HeadBefore != HeadAfter) {
	    aTmp.SetSize(aTmp.GetSize() + HeadAfter - HeadBefore);
	    aTmp.SetSizeLength(aTmp.CodedSizeLength() - (HeadAfter - HeadBefore));
	  }
	  aTmp.RenderHead(output, bSaveDefault); // the rest of the data is not rewritten
	}

	if (ComeBackAfterward) {
		output.setFilePointer(CurrentPosition);
	}

	return EltToReplaceWith.GetSize() + EltToReplaceWith.HeadSize() + Size + HeadSize();
}

uint32 EbmlVoid::Overwrite(const EbmlElement & EltToVoid, IOCallback & output, bool ComeBackAfterward, bool bSaveDefault)
{
//	EltToVoid.UpdateSize(bSaveDefault);
	if (EltToVoid.GetElementPosition() == 0) {
		// this element has never been written
		return 0;
	}
	if (EltToVoid.GetSize() + EltToVoid.HeadSize() <2) {
		// the element can't be written here !
		return 0;
	}

	uint64 CurrentPosition = output.getFilePointer();

	output.setFilePointer(EltToVoid.GetElementPosition());

	// compute the size of the voided data based on the original one
	Size = EltToVoid.GetSize() + EltToVoid.HeadSize() - 1; // 1 for the ID
	Size -= CodedSizeLength();
	// make sure we handle even the strange cases
	//uint32 A1 = Size + HeadSize();
	//uint32 A2 = EltToVoid.GetSize() + EltToVoid.HeadSize();
	if (Size + HeadSize() != EltToVoid.GetSize() + EltToVoid.HeadSize()) {
		Size--;
		SetSizeLength(CodedSizeLength() + 1);
	}

	if (Size != 0) {
		RenderHead(output, bSaveDefault); // the rest of the data is not rewritten
	}

	if (ComeBackAfterward) {
		output.setFilePointer(CurrentPosition);
	}

	return EltToVoid.GetSize() + EltToVoid.HeadSize();
}

END_LIBEBML_NAMESPACE
