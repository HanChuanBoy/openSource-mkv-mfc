/****************************************************************************
** libebml : parse EBML files, see http://embl.sourceforge.net/
**
** <file/class description>
**
** Copyright (C) 2002-2003 Ingo Ralf Blum.  All rights reserved.
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
	\version \$Id: EbmlTypes.h,v 1.3 2003/05/27 11:12:23 mosu Exp $
*/
#ifndef LIBEBML_TYPES_H
#define LIBEBML_TYPES_H

#include <clocale>
#include <string>

#include "EbmlConfig.h"

#include "api/c/libebml_t.h"

START_LIBEBML_NAMESPACE

// Define standard types used
typedef uint32 timecode;
typedef uint8  track;
typedef int32  unixtime;

// Unicode converter. Converts UTF16/32 <-> UTF 8
std::string convertUTF(const wchar_t*String);
#if 0 // not supported on Cygwin ?
std::wstring convertUTF(const char*String);
#endif // 0

// Helper function to set the fixed length fields in the library. The second is a convenience
// function for use with the internal structures.
void copyUTF8(const char*Source,const char*Destination,size_t DestinationSize);
template<class BUFFER> void copyUTF8(const char*Source,BUFFER&Buffer){copyUTF8(Source,&Buffer,sizeof(Buffer));}

//#ifdef LIBEBML_WIN32
typedef wchar_t utf16;
typedef uint32 utf32;
//#else
//typedef uint16 utf16;
//typedef wchar_t utf32;
//#endif
typedef char utf8;
typedef char ascii;
typedef char filename;

#ifndef SPEAKER_FRONT_LEFT
enum channel_position {
    SPEAKER_FRONT_LEFT            = 0x00001,
    SPEAKER_FRONT_RIGHT           = 0x00002,
    SPEAKER_FRONT_CENTER          = 0x00004,
    SPEAKER_LOW_FREQUENCY         = 0x00008,
    SPEAKER_BACK_LEFT             = 0x00010,
    SPEAKER_BACK_RIGHT            = 0x00020,
    SPEAKER_FRONT_LEFT_OF_CENTER  = 0x00040,
    SPEAKER_FRONT_RIGHT_OF_CENTER = 0x00080,
    SPEAKER_BACK_CENTER           = 0x00100,
    SPEAKER_SIDE_LEFT             = 0x00200,
    SPEAKER_SIDE_RIGHT            = 0x00400,
    SPEAKER_TOP_CENTER            = 0x00800,
    SPEAKER_TOP_FRONT_LEFT        = 0x01000,
    SPEAKER_TOP_FRONT_CENTER      = 0x02000,
    SPEAKER_TOP_FRONT_RIGHT       = 0x04000,
    SPEAKER_TOP_BACK_LEFT         = 0x08000,
    SPEAKER_TOP_BACK_CENTER       = 0x10000,
    SPEAKER_TOP_BACK_RIGHT        = 0x20000
};
#endif

/* incompatible with cygwin 
// to_type = UTF8 char
// from_type = wchar_t
//
// UTF-8 has the following codes
// 0xxxxxxx => Single byte character (ASCII)
// 10xxxxxx => Byte following a multi byte sequence start
// 1...10x...x, where count(1...1) is between 2 and 6 and count(x...x) is between 5 and 1 => multi byte sequence start
class LIBEBML_EXPORT UTF8CodeConverter:public std::codecvt<utf16,char,mbstate_t>
{
public:
	UTF8CodeConverter(size_t References=0):std::codecvt<wchar_t,char,mbstate_t>(References){}

protected:
	// Conversion from UTF8 -> wchar_t
	virtual result do_in(state_type&State
		,const to_type*SourceBeginning,const to_type*SourceEnd,const to_type*&SourceNext
		,from_type*DestBeginning,from_type*DestEnd,from_type*&DestNext
		)const;

	// Conversion from wchar_t -> UTF8
	virtual result do_out(state_type&State
		,const from_type*SourceBeginning,const from_type*SourceEnd,const from_type*&SourceNext
		,to_type*DestinationBeginning,to_type*DestinationEnd,to_type*&DestinationNext
		)const;

	virtual bool do_always_noconv()const throw(){return false;}
	virtual int do_max_length()const throw(){return 6;}
	virtual int do_endoding()const throw(){return 0;} // The encoding has varying length

	//    virtual result do_unshift(_St& _State,
	//      _To *_F2, _To *_L2, _To *& _Mid2) const
	//    {return (ok); }

	// A better name for do_length would be skip_up_to_count_characters_and_return_the_bytes_skipped.
	//virtual int do_length(state_type&State,const to_type*Beginning,const to_type*End,size_t Count)const throw();
};
*/

END_LIBEBML_NAMESPACE

#include "EbmlEndian.h" // binary needs to be defined

START_LIBEBML_NAMESPACE

typedef binary bits80[10];

typedef Endian<int16,little_endian>  lil_int16;
typedef Endian<int32,little_endian>  lil_int32;
typedef Endian<int64,little_endian>  lil_int64;
typedef Endian<uint16,little_endian> lil_uint16;
typedef Endian<uint32,little_endian> lil_uint32;
typedef Endian<uint64,little_endian> lil_uint64;
typedef Endian<int16,big_endian>     big_int16;
typedef Endian<int32,big_endian>     big_int32;
typedef Endian<int64,big_endian>     big_int64;
typedef Endian<uint16,big_endian>    big_uint16;
typedef Endian<uint32,big_endian>    big_uint32;
typedef Endian<uint64,big_endian>    big_uint64;
typedef Endian<uint32,big_endian>    checksum;
typedef Endian<bits80,big_endian>    big_80bits;

#if 0
// Track type
struct TrackInfo {
    track_type TrackType; // generated on the fly

    bool Enabled;
    bool Preferred;
//    uint32 BlockSize; ///< 0 if block size is not fixed

    binary LanguageCode[4]; ///< \see http://www.matroska.org/language.htm

    std::string Format;
    uint16 Version1; ///< encoding
    uint16 Version2; ///< reading
    uint16 Version3; ///< absolute minimum reading

    std::string CodecName;
    std::string CodecUrl;
    std::string CodecUrl_alt;

    uint64 NanoSecondPerBlock;

    std::string CodecSettings;

    std::string Name;
};

struct TrackInfoAudio {
    uint32 SampleRate;
    uint8  BitDepth;
    bool   DTS;
    bool   SecondaryTrack;
    uint8  SubtrackNumber;
    uint8  ChannelPerSubtrack;
    uint16 AVIFormatTag;
    uint32 ChannelPosition;
};

struct TrackInfoSubtitle {
    bool    myHas_1;
    bool    myControlCodeEnabled;
    uint8   myCharSubset;
};

enum VideoStereoEye {
    eye_unknown = 0,
    eye_right   = 1,
    eye_left    = 2,
    eye_both    = 3
};

struct TrackInfoVideo {
    bool Interlaced;
    bool Stereo3D;
    VideoStereoEye Eyes;
    uint16 xSize; ///< horizontal
    uint16 ySize; ///< vertical
    uint16 AspectRatio; // 1333 means 1:333, 1778 means 16:9 = 1778:1
    binary FourCC[4];
    uint32 ColourSpace;
    binary GUID[16];
    uint8  Gamma;
};
#endif // 0

END_LIBEBML_NAMESPACE

#endif
