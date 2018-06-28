#define ALLOW_OS_CODE 1

#include "os.h"

#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <pthread.h>
#include "rmmkvapi.h"
#define MAX_SIZE 64*1024

#include <iostream>

using namespace std;

int main (int argc, char **argv) {
	ExternalRMmkvClient mkvClient = NULL;
	ExternalRMmkvTrack  videoTrack = NULL;
	ExternalRMmkvTrack  audioTrack = NULL;
	RMuint32 videoTrackID, audioTrackID;
	RMmkvSample videoSample;
	int i;

	if (argc < 2) {
		printf ("Usage: %s file\n", argv[0]);
		return 0;
	}
	
	printf ("Begin to open %s!\n", argv[1]);
	
	if (RMFAILED(RMOpenFileMKV(argv[1], &mkvClient))) {
		printf ("Can't open mkv demux client for %s!\n", argv[1]);
		return 0;
	}
	
	videoTrackID = RMGetMKVVideoTrackID(mkvClient);
//	audioTrackID = RMGetMKVAudioTrackID(mkvClient);
	
	if (videoTrackID)	
		videoTrack = RMOpenMKVTrack(mkvClient, videoTrackID);

//	if (audioTrackID)	
//		audioTrack = RMOpenMKVTrack(mkvClient, audioTrackID);
	
	RMPrintMKV(mkvClient);

    RMuint64 timescale;
    RMGetMKVMovieTimescale(mkvClient, &timescale);

    printf ("Movie timescale %lld!\n", timescale);
	if (TRUE == RMisMKVH264Track(videoTrack)) {
		RMuint32 length_size, profile, level;
		RMGetMKVH264LengthSize (videoTrack, &length_size);
		RMGetMKVH264Profile (videoTrack, &profile);
		RMGetMKVH264Level (videoTrack, &level);
		printf ("length_size = %ld, profile = %ld, level = %ld!\n",
                length_size, profile, level);
	}
	
	RMSeekMKV(mkvClient, 0);
	
	videoSample.size = MAX_SIZE;
	videoSample.buf = (RMuint8*)malloc(MAX_SIZE);
    fprintf (stderr, "\ncmmand> ");
	
    string command;
    int seek = 0;
    do {
        getline(cin, command);
        if (!strcmp (command.c_str (), "start")) {
            RMSeekMKV (mkvClient, 0);
        } else if (!strcmp (command.c_str(), "dump")) {
            //while
        } else if (!strcmp (command.c_str(), "video")) {
            int count;
            RMuint32 vsize;
            scanf("%d", &count);
            for (int i=0; i<count; i++) {
                vsize = MAX_SIZE;
                RMGetNextMKVSample(videoTrack, &videoSample, vsize);
            }
        } else if (!strcmp (command.c_str(), "audio")) {
//            int count;
//            RMuint32 vsize;
//            scanf("%d", &count);
//            for (int i=0; i<count; i++) {
//                vsize = MAX_SIZE;
//                RMGetNextMKVSample(audioTrack, &videoSample, vsize);
//            }

        } else if (!strcmp (command.c_str(), "seek")) {
            scanf("%d", &seek);
            RMSeekMKV (mkvClient, seek);
        } else if (!strcmp (command.c_str(), "quit")) {
            break;
        }
        cout<<endl<<"command>";
    } while (1);
    
	RMCloseMKVTrack (mkvClient, videoTrack);
//	RMCloseMKVTrack (mkvClient, audioTrack);
    
	free (videoSample.buf);
	RMCloseMKV (mkvClient);
	return 1;
}
