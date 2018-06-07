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
    \file libmatroska.c
    \version \$Id: libmatroska.cpp,v 1.2 2003/04/28 10:06:10 robux4 Exp $
    \author Steve Lhomme     <robux4 @ users.sf.net>

    \brief C wrapper to libmatroska
*/

#include "StdInclude.h"

#include "libmatroska.h"

#include "StdIOCallback.h"
#include "FileKax.h"
//#include "Track.h"
//#include "TrackType.h"
//#include "TrackAudio.h"
//#include "TrackAV.h"
//#include "TrackControl.h"
//#include "TrackCustom.h"
//#include "TrackMultiAudio.h"
//#include "TrackSubtitle.h"
//#include "TrackTitle.h"
//#include "TrackVideo.h"

using namespace LIBMATROSKA_NAMESPACE;

#ifdef OLD
matroska_stream matroska_open_stream_file(c_string string, open_mode mode)
{
    // write the head of the file (with everything already configured)
    StdIOCallback *fileid = new StdIOCallback(string,mode);
    return fileid;
}

matroska_id matroska_open_stream(matroska_stream a_stream)
{
    IOCallback *_stream = (IOCallback *)a_stream;
    // create the Matroska file
    FileKax *aFile = new FileKax(*_stream);

    return aFile;
}

void matroska_close(matroska_id id)
{
    if (id != NULL)
    {
	FileKax * File = (FileKax *) id;
	delete File;
    }
}

void matroska_end(matroska_id id, uint32 totaltime)
{
    if (id != NULL)
    {
	FileKax * File = (FileKax *) id;
	File->Close(totaltime);
	delete File;
    }
}

matroska_track matroska_create_track(matroska_id id, const track_type aType)
{
    FileKax * File = (FileKax *) id;
    Track * binID = File->CreateTrack( aType );

    return binID;
}

void matroska_read_head(matroska_id id)
{
    FileKax * File = (FileKax *) id;
    File->ReadHead( );
}

void matroska_read_tracks(matroska_id id)
{
    FileKax * File = (FileKax *) id;
    File->ReadTracks( );
}

uint8 matroska_get_number_track(matroska_id id)
{
    FileKax * File = (FileKax *) id;
    return File->GetTrackNumber();
}

matroska_track matroska_get_track(matroska_id id, uint8 track_index)
{
    FileKax * File = (FileKax *) id;
    return File->GetTrack(track_index);
}

void matroska_get_track_info(matroska_id id, matroska_track track, track_info * infos)
{
    FileKax * File = (FileKax *) id;
    Track * _Track = (Track *) track;

    TrackInfo _infos;
    File->Track_GetInfo(_Track, _infos);

    infos->Enabled = _infos.Enabled;
    infos->Preferred = _infos.Preferred;
    infos->TrackType = track_type(_infos.TrackType);
    infos->Version1 = _infos.Version1;
    infos->Version2 = _infos.Version2;
    infos->Version3 = _infos.Version3;
    memcpy(infos->LanguageCode, _infos.LanguageCode, 4);
    memcpy(infos->CodecName, _infos.CodecName.c_str(), min(sizeof(infos->CodecName), _infos.CodecName.length()+1));
    memcpy(infos->CodecUrl, _infos.CodecUrl.c_str(), min(sizeof(infos->CodecUrl), _infos.CodecUrl.length()+1));
    memcpy(infos->CodecUrl_alt, _infos.CodecUrl_alt.c_str(), min(sizeof(infos->CodecUrl_alt), _infos.CodecUrl_alt.length()+1));
    memcpy(infos->Format, _infos.Format.c_str(), min(sizeof(infos->Format), _infos.Format.length())+1);
    memcpy(infos->CodecName, _infos.CodecName.c_str(), min(sizeof(infos->CodecName), _infos.CodecName.length()+1));
    memcpy(infos->CodecSettings, _infos.CodecSettings.c_str(), min(sizeof(infos->CodecSettings), _infos.CodecSettings.length()+1));
    memcpy(infos->Name, _infos.Name.c_str(), min(sizeof(infos->Name), _infos.Name.length()+1));
}

#endif // OLD