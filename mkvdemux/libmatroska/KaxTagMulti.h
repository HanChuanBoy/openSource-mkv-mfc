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
	\version \$Id: KaxTagMulti.h,v 1.9 2003/05/27 21:22:40 robux4 Exp $
	\author Jory Stone     <jcsston @ toughguy.net>
	\author Steve Lhomme   <robux4 @ users.sf.net>
*/
#ifndef LIBMATROSKA_TAGMULTI_H
#define LIBMATROSKA_TAGMULTI_H

#include "KaxTypes.h"
#include "EbmlMaster.h"
#include "EbmlDate.h"
#include "EbmlFloat.h"
#include "EbmlSInteger.h"
#include "EbmlUInteger.h"
#include "EbmlString.h"
#include "EbmlUnicodeString.h"
#include "EbmlBinary.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE

enum KaxTagMultiCommercialTypes {
	KaxTagMultiCommercialType_FilePurchase = 1, //Information on where to purchase this file. This is akin to the WPAY tag in ID3.
	KaxTagMultiCommercialType_ItemPurchase,	//Information on where to purchase this album. This is akin to the WCOM tag in ID3.
	KaxTagMultiCommercialType_Owner  //Information on the purchase that occurred for this file. This is akin to the OWNE tag in ID3.
};
enum KaxTagMultiDateTypes {
	KaxTagMultiDateType_EncodingDate = 1, //The time that the encoding of this item was completed. This is akin to the TDEN tag in ID3.
	KaxTagMultiDateType_RecordingDate, //The time that the recording began, and finished. This is akin to the TDRC tag in ID3.
	KaxTagMultiDateType_ReleaseDate, //The time that the item was originaly released. This is akin to the TDRL tag in ID3.
	KaxTagMultiDateType_OriginalReleaseDate, //The time that the item was originaly released if it is a remake. This is akin to the TDOR tag in ID3.
	KaxTagMultiDateType_TaggingDate, //The time that the tags were done for this item. This is akin to the TDTG tag in ID3.
	KaxTagMultiDateType_DigitizingDate  //The time that the item was tranfered to a digital medium. This is akin to the IDIT tag in RIFF
};
enum KaxTagMultiEntitiesTypes {
	KaxTagMultiEntitiesType_LyricistTextWriter = 1, //The person that wrote the words/script for this item. This is akin to the TEXT tag in ID3.
	KaxTagMultiEntitiesType_Composer, //The name of the composer of this item. This is akin to the TCOM tag in ID3.
	KaxTagMultiEntitiesType_LeadPerformerSoloist, //This is akin to the TPE1 tag in ID3.
	KaxTagMultiEntitiesType_BandOrchestraAccompaniment, //This is akin to the TPE2 tag in ID3.
	KaxTagMultiEntitiesType_OriginalLyricistTextWriter, //This is akin to the TOLY tag in ID3.
	KaxTagMultiEntitiesType_OriginalArtistPerformer, //This is akin to the TOPE tag in ID3.
	KaxTagMultiEntitiesType_OriginalAlbumMovieShowTitle, //This is akin to the TOAL tag in ID3.
	KaxTagMultiEntitiesType_ConductorPerformerRefinement, //This is akin to the TPE3 tag in ID3.
	KaxTagMultiEntitiesType_InterpretedRemixedBy, //This is akin to the TPE4 tag in ID3.
	KaxTagMultiEntitiesType_Director, //This is akin to the IART tag in RIFF
	KaxTagMultiEntitiesType_ProducedBy, //This is akin to the IPRO tag in Extended RIFF
	KaxTagMultiEntitiesType_Cinematographer, //This is akin to the ICNM tag in Extended RIFF
	KaxTagMultiEntitiesType_ProductionDesigner, //This is akin to the IPDS tag in Extended RIFF
	KaxTagMultiEntitiesType_CostumeDesigner, //This is akin to the ICDS tag in Extended RIFF
	KaxTagMultiEntitiesType_ProductionStudio, //This is akin to the ISTD tag in Extended RIFF
	KaxTagMultiEntitiesType_DistributedBy, //This is akin to the IDST tag in Extended RIFF
	KaxTagMultiEntitiesType_CommissionedBy, //This is akin to the ICMS tag in RIFF
	KaxTagMultiEntitiesType_Engineer, //This is akin to the IENG tag in RIFF
	KaxTagMultiEntitiesType_EditedBy, //This is akin to the IEDT tag in Extended RIFF
	KaxTagMultiEntitiesType_EncodedBy, //This is akin to the TENC tag in ID3.
	KaxTagMultiEntitiesType_RippedBy, //This is akin to the IRIP tag in Extended RIFF
	KaxTagMultiEntitiesType_InvolvedPeopleList, //A very general tag for everyone else that wants to be listed. This is akin to the TMCL tag in ID3.
	KaxTagMultiEntitiesType_InternetRadioStationName, //This is akin to the TSRN tag in ID3.
	KaxTagMultiEntitiesType_Publisher  //This is akin to the TPUB tag in ID3.
};

enum KaxTagMultiIdentifierTypes {
	KaxTagMultiIdentifierType_ISRC = 1, //String, The International Standard Recording Code
	KaxTagMultiIdentifierType_CDIdentifier, //Binary, This is a binary dump of the TOC of the CDROM that this item was taken from. This holds the same information as the MCDI in ID3.
	KaxTagMultiIdentifierType_ISBN, //String, International Standard Book Number
	KaxTagMultiIdentifierType_Catalog, //String, sometimes the EAN/UPC, often some letters followed by some numbers
	KaxTagMultiIdentifierType_EAN, //String, EAN-13 bar code identifier
	KaxTagMultiIdentifierType_UPC, //String, UPC-A bar code identifier
	KaxTagMultiIdentifierType_LabelCode, //String, Typically printed as ________ (LC) xxxx) ~~~~~~~~ or _________ (LC) 0xxxx) ~~~~~~~~~ on CDs medias or covers, where xxxx is a 4-digit number.
	KaxTagMultiIdentifierType_LCCN, //String, Library of Congress Control Number
	KaxTagMultiIdentifierType_UniqueFileIdentifier, //Binary, This used for a dump of the UFID field in ID3. This field would only be used if the item was pulled from an MP3.
	CDROM_CD_TEXT_PACK_TOC_INFO2  //Binary
};

enum KaxTagMultiLegalTypes {
	KaxTagMultiLegalType_Copyright = 1, //The copyright information as per the copyright holder. This is akin to the TCOP tag in ID3.
	KaxTagMultiLegalType_ProductionCopyright, //The copyright information as per the production copyright holder. This is akin to the TPRO tag in ID3.
	KaxTagMultiLegalType_TermsOfUse  //The terms of use for this item. This is akin to the USER tag in ID3.
};

enum KaxTagMultiTitleTypes {
	KaxTagMultiTitleType_TrackTitle = 1,
		//The title of this item. In the case of a track, the Name element should be identical to the Name element.
		//For example, for music you might label this "Canon in D", or for video's audio track you might use "English 5.1" This is akin to the TIT2 tag in ID3.
	KaxTagMultiTitleType_AlbumMovieShowTitle,
		//This is the name given to a grouping of tracks and/or chapters. 
		//For example, all video, audio, and subtitle tracks for a movie would be grouped under this and be given the name of the movie. 
		//All tracks for a particular CD would be grouped together under the title of the CD, or if all tracks for a CD were recorded as a single track, seperated by chapters, the same would apply.
		//You could use this to label episode 3 of The Simpsons. This is akin to the TALB tag in ID3.
	KaxTagMultiTitleType_SetTitle,	//This would be used to label a set of ID 2. For example, season 13 of The Simpsons.
	KaxTagMultiTitleType_Series  //This would be used to label a set of ID 3. For example, The Simpsons.
};

class KaxTagMultiComment : public EbmlMaster {
	public:
		KaxTagMultiComment();
		static EbmlElement & Create() {return *(new KaxTagMultiComment);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiCommentName : public EbmlString {
	public:
		KaxTagMultiCommentName() {}
		static EbmlElement & Create() {return *(new KaxTagMultiCommentName);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiCommentComments : public EbmlUnicodeString {
	public:
		KaxTagMultiCommentComments() {}
		static EbmlElement & Create() {return *(new KaxTagMultiCommentComments);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiCommentLanguage : public EbmlString {
	public:
		KaxTagMultiCommentLanguage() {}
		static EbmlElement & Create() {return *(new KaxTagMultiCommentLanguage);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiCommercial : public EbmlMaster {
	public:
		KaxTagMultiCommercial();
		static EbmlElement & Create() {return *(new KaxTagMultiCommercial);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagCommercial : public EbmlMaster {
	public:
		KaxTagCommercial();
		static EbmlElement & Create() {return *(new KaxTagCommercial);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiCommercialType : public EbmlUInteger {
	public:
		KaxTagMultiCommercialType() {}
		static EbmlElement & Create() {return *(new KaxTagMultiCommercialType);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiCommercialAddress : public EbmlUnicodeString {
	public:
		KaxTagMultiCommercialAddress() {}
		static EbmlElement & Create() {return *(new KaxTagMultiCommercialAddress);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiCommercialURL : public EbmlString {
	public:
		KaxTagMultiCommercialURL() {}
		static EbmlElement & Create() {return *(new KaxTagMultiCommercialURL);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiCommercialEmail : public EbmlString {
	public:
		KaxTagMultiCommercialEmail() {}
		static EbmlElement & Create() {return *(new KaxTagMultiCommercialEmail);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiPrice : public EbmlMaster {
	public:
		KaxTagMultiPrice();
		static EbmlElement & Create() {return *(new KaxTagMultiPrice);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiPriceCurrency : public EbmlString {
	public:
		KaxTagMultiPriceCurrency() {}
		static EbmlElement & Create() {return *(new KaxTagMultiPriceCurrency);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiPriceAmount : public EbmlFloat {
	public:
		KaxTagMultiPriceAmount() :EbmlFloat() {}
		static EbmlElement & Create() {return *(new KaxTagMultiPriceAmount);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiPricePriceDate : public EbmlDate {
	public:
		KaxTagMultiPricePriceDate() :EbmlDate() {}
		static EbmlElement & Create() {return *(new KaxTagMultiPricePriceDate);}		
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiDate : public EbmlMaster {
	public:
		KaxTagMultiDate();
		static EbmlElement & Create() {return *(new KaxTagMultiDate);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagDate : public EbmlMaster {
	public:
		KaxTagDate();
		static EbmlElement & Create() {return *(new KaxTagDate);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiDateType : public EbmlUInteger {
	public:
		KaxTagMultiDateType() {}
		static EbmlElement & Create() {return *(new KaxTagMultiDateType);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiDateDateBegin : public EbmlDate {
	public:
		KaxTagMultiDateDateBegin() :EbmlDate() {}
		static EbmlElement & Create() {return *(new KaxTagMultiDateDateBegin);}		
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiDateDateEnd : public EbmlDate {
	public:
		KaxTagMultiDateDateEnd() :EbmlDate() {}
		static EbmlElement & Create() {return *(new KaxTagMultiDateDateEnd);}		
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiEntity : public EbmlMaster {
	public:
		KaxTagMultiEntity();
		static EbmlElement & Create() {return *(new KaxTagMultiEntity);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagEntity : public EbmlMaster {
	public:
		KaxTagEntity();
		static EbmlElement & Create() {return *(new KaxTagEntity);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiEntityType : public EbmlUInteger {
	public:
		KaxTagMultiEntityType() {}
		static EbmlElement & Create() {return *(new KaxTagMultiEntityType);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiEntityName : public EbmlUnicodeString {
	public:
		KaxTagMultiEntityName() {}
		static EbmlElement & Create() {return *(new KaxTagMultiEntityName);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiEntityAddress : public EbmlUnicodeString {
	public:
		KaxTagMultiEntityAddress() {}
		static EbmlElement & Create() {return *(new KaxTagMultiEntityAddress);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiEntityURL : public EbmlString {
	public:
		KaxTagMultiEntityURL() {}
		static EbmlElement & Create() {return *(new KaxTagMultiEntityURL);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiEntityEmail : public EbmlString {
	public:
		KaxTagMultiEntityEmail() {}
		static EbmlElement & Create() {return *(new KaxTagMultiEntityEmail);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiIdentifier : public EbmlMaster {
	public:
		KaxTagMultiIdentifier();
		static EbmlElement & Create() {return *(new KaxTagMultiIdentifier);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagIdentifier : public EbmlMaster {
	public:
		KaxTagIdentifier();
		static EbmlElement & Create() {return *(new KaxTagIdentifier);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiIdentifierType : public EbmlUInteger {
	public:
		KaxTagMultiIdentifierType() {}
		static EbmlElement & Create() {return *(new KaxTagMultiIdentifierType);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiIdentifierBinary : public EbmlBinary {
	public:
		KaxTagMultiIdentifierBinary() {}
		static EbmlElement & Create() {return *(new KaxTagMultiIdentifierBinary);}
		bool ValidateSize() const {return true;} // we don't mind about what's inside
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiIdentifierString : public EbmlUnicodeString {
	public:
		KaxTagMultiIdentifierString() {}
		static EbmlElement & Create() {return *(new KaxTagMultiIdentifierString);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiLegal : public EbmlMaster {
	public:
		KaxTagMultiLegal();
		static EbmlElement & Create() {return *(new KaxTagMultiLegal);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagLegal : public EbmlMaster {
	public:
		KaxTagLegal();
		static EbmlElement & Create() {return *(new KaxTagLegal);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiLegalType : public EbmlUInteger {
	public:
		KaxTagMultiLegalType() {}
		static EbmlElement & Create() {return *(new KaxTagMultiLegalType);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiLegalURL : public EbmlString {
	public:
		KaxTagMultiLegalURL() {}
		static EbmlElement & Create() {return *(new KaxTagMultiLegalURL);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiLegalAddress : public EbmlUnicodeString {
	public:
		KaxTagMultiLegalAddress() {}
		static EbmlElement & Create() {return *(new KaxTagMultiLegalAddress);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiTitle : public EbmlMaster {
	public:
		KaxTagMultiTitle();
		static EbmlElement & Create() {return *(new KaxTagMultiTitle);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagTitle : public EbmlMaster {
	public:
		KaxTagTitle();
		static EbmlElement & Create() {return *(new KaxTagTitle);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiTitleType : public EbmlUInteger {
	public:
		KaxTagMultiTitleType() {}
		static EbmlElement & Create() {return *(new KaxTagMultiTitleType);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};


class KaxTagMultiTitleName : public EbmlUnicodeString {
	public:
		KaxTagMultiTitleName() {}
		static EbmlElement & Create() {return *(new KaxTagMultiTitleName);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiTitleSubTitle : public EbmlUnicodeString {
	public:
		KaxTagMultiTitleSubTitle() {}
		static EbmlElement & Create() {return *(new KaxTagMultiTitleSubTitle);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiTitleEdition : public EbmlUnicodeString {
	public:
		KaxTagMultiTitleEdition() {}
		static EbmlElement & Create() {return *(new KaxTagMultiTitleEdition);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiTitleAddress : public EbmlUnicodeString {
	public:
		KaxTagMultiTitleAddress() {}
		static EbmlElement & Create() {return *(new KaxTagMultiTitleAddress);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiTitleURL : public EbmlString {
	public:
		KaxTagMultiTitleURL() {}
		static EbmlElement & Create() {return *(new KaxTagMultiTitleURL);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiTitleEmail : public EbmlString {
	public:
		KaxTagMultiTitleEmail() {}
		static EbmlElement & Create() {return *(new KaxTagMultiTitleEmail);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagMultiTitleLanguage : public EbmlString {
	public:
		KaxTagMultiTitleLanguage() {}
		static EbmlElement & Create() {return *(new KaxTagMultiTitleLanguage);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

/*
class KaxTagBPM : public EbmlFloat {
	public:
		KaxTagBPM() :EbmlFloat() {}
		static EbmlElement & Create() {return *(new KaxTagBPM);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagPopularimeter : public EbmlSInteger {
	public:
		KaxTagPopularimeter() {}
		static EbmlElement & Create() {return *(new KaxTagPopularimeter);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagRating : public EbmlBinary {
	public:
		KaxTagRating() {}
		static EbmlElement & Create() {return *(new KaxTagRating);}
		bool ValidateSize() const {return true;} // we don't mind about what's inside
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagSetPart : public EbmlUInteger {
	public:
		KaxTagSetPart() {}
		static EbmlElement & Create() {return *(new KaxTagSetPart);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagUserDefinedURL : public EbmlString {
	public:
		KaxTagUserDefinedURL() {}
		static EbmlElement & Create() {return *(new KaxTagUserDefinedURL);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxTagVideoSecondaryGenre : public EbmlBinary {
	public:
		KaxTagVideoSecondaryGenre() {}
		static EbmlElement & Create() {return *(new KaxTagVideoSecondaryGenre);}
		bool ValidateSize() const {return (Size >= 4);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};

class KaxWritingApp : public EbmlUnicodeString {
	public:
		KaxWritingApp() {}
		static EbmlElement & Create() {return *(new KaxWritingApp);}
		const EbmlCallbacks & Generic() const {return ClassInfos;}
		static const EbmlCallbacks ClassInfos;
		operator const EbmlId &() const {return ClassInfos.GlobalId;}
};*/

END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_TAGMULTI_H
