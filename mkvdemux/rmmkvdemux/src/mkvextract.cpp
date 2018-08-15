/*
   mkvextract -- extract tracks from Matroska files into other files

   Distributed under the GPL
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   $Id: mkvextract.cpp 3455 2007-01-10 13:13:23Z mosu $

   command line parsing, setup

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#include "os.h"

#include <errno.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#if defined(COMP_MSC)
#include <cassert>
#else
#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <vector>

#include <ebml/EbmlHead.h>
#include <ebml/EbmlSubHead.h>
#include <ebml/EbmlStream.h>
#include <ebml/EbmlVoid.h>
#include <matroska/FileKax.h>

#include <matroska/KaxAttached.h>
#include <matroska/KaxAttachments.h>
#include <matroska/KaxBlock.h>
#include <matroska/KaxBlockData.h>
#include <matroska/KaxChapters.h>
#include <matroska/KaxCluster.h>
#include <matroska/KaxClusterData.h>
#include <matroska/KaxCues.h>
#include <matroska/KaxCuesData.h>
#include <matroska/KaxInfo.h>
#include <matroska/KaxInfoData.h>
#include <matroska/KaxSeekHead.h>
#include <matroska/KaxSegment.h>
#include <matroska/KaxTags.h>
#include <matroska/KaxTracks.h>
#include <matroska/KaxTrackEntryData.h>
#include <matroska/KaxTrackAudio.h>
#include <matroska/KaxTrackVideo.h>

#include "common.h"
#include "matroska.h"
#include "mkvextract.h"
#include "mm_io.h"
#include "matroska_demux.h"

using namespace libmatroska;
using namespace std;

bool no_variable_data = false;
static bool demux_running = false;

typedef enum {
	PLAY = 0,
	STOP
} STATE;

static STATE state = STOP;

void *demux_func (void *pargs) {
    matroska_demux_c *p_demux = (matroska_demux_c *)pargs;
    assert (p_demux != NULL);

    fprintf (stderr, "%s %d\n", __FUNCTION__, __LINE__);
    while (demux_running) {
		switch (state) {
			case PLAY:
				p_demux->Demux();
				break;
			case STOP:
				break;
			default:
				break;
		}
    }
    fprintf (stderr, "%s %d\n", __FUNCTION__, __LINE__);
    
    return NULL;
}
 
#define VIDEO_BUFFER_SIZE (32 *1024)
static uint8_t g_video_buffer[VIDEO_BUFFER_SIZE];

int main(int argc, char **argv) {
    string input_file;
    matroska_demux_c *pdemux = NULL;
    uint32_t videoTrackID, audioTrackID;

    mm_file_io_c::setup();
    srand(time(NULL));

    pdemux = new matroska_demux_c ();

    input_file = argv[1];
    pdemux->Open (input_file);


    printf ("%s has %d video tracks, %d audio tracks, %d subtitle tracks!\n", input_file.c_str (),
            pdemux->NumberOfVideoTracks (), pdemux->NumberOfAudioTracks(), pdemux->NumberOfSubtitleTracks ());

    printf ("Set video track ID = %d!\n", (videoTrackID = pdemux->GetVideoTrackID ()));
    printf ("Set audio track ID = %d!\n", (audioTrackID = pdemux->GetAudioTrackID ()));

    matroska_track_c *videoTrack = pdemux->GetTrackByID (videoTrackID);
    matroska_track_c *audioTrack = pdemux->GetTrackByID (audioTrackID);

    size_t dsiSize = 0;
    uint8_t *dsiBuffer;
    dsiBuffer = videoTrack->TrackDSI (dsiSize);
    printf ("VideoDSI:");
    for (int i=0; i<dsiSize; i++)
        printf ("%02x ", dsiBuffer[i]);
    printf ("\n");

    dsiBuffer = audioTrack->TrackDSI (dsiSize);
    printf ("AudioDSI:");
    for (int i=0; i<dsiSize; i++)
        printf ("%02x ", dsiBuffer[i]);
    printf ("\n");

    pdemux->PreparePlayback ();

    pdemux->DumpTracks ();

    pthread_t demux_pthread;
    pthread_attr_t demux_attr;

    demux_running = true;

    pthread_attr_init (&demux_attr);
    pthread_create (&demux_pthread, &demux_attr, &demux_func, (void *)pdemux);

    // Got the demux thread running
    //if you want dump video track you should  close seek;
    //pdemux->Seek (0);
    string command;
    int seek = 0;
	size_t vsize;
    cout<<endl<<"command>";

    do {
        getline(cin, command);
        if (!strcmp (command.c_str (), "start")) {
			state = PLAY;
			pdemux->Seek (0);
        } else if (!strcmp (command.c_str(), "get")) {
            int count;
            scanf("%d", &count);
            for (int i=0; i<count; i++) {
                vsize = VIDEO_BUFFER_SIZE;
                pdemux->track_get_next_frame (videoTrack, g_video_buffer, vsize);
            }
        } else if (!strcmp (command.c_str(), "seek")) {
            scanf("%d", &seek);
			state = STOP;
			pdemux->Seek (seek);
			state = PLAY;
        } else if (!strcmp (command.c_str(), "quit")) {
            demux_running = false;
            pdemux->Seek (0);
            pthread_join (demux_pthread, NULL);
            break;
        }
        cout<<endl<<"command>";
    } while (1);

    pdemux->CloseTrack (videoTrack);
    pdemux->CloseTrack (audioTrack);

    pdemux->Close ();
    
    delete pdemux;

    return 0;
}
