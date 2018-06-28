/*
 *
 * Copyright (c) Sigma Designs, Inc. 2005. All rights reserved.
 *
 */

/**
	@file play_mp4.c
	@brief sample application to access the Mambo chip and test DMA transfers
	
	@author Sebastian Frias Feltrer
   	@ingroup dccsamplecode
*/

/*
  ******************************************************************************************
  This file is part of libsamples library, therefore *NO* static variables should be defined
  ******************************************************************************************
*/

#include "../../samples/sample_os.h"

#define ALLOW_OS_CODE 1

#include "../../dcc/include/dcc.h"

#include "../include/rmmkvapi.h"
#include "../include/rmmkvdemux.h"

#include "../../samples/common.h"
#include "../../samples/parsemp4dsi.h"
#include <unistd.h>
#include <stdlib.h>

#define MAX_SAMPLE_SIZE 2048

#define COMMON_TIMEOUT_US    (TIMEOUT_10MS)	  // 10 ms
#define DMA_BUFFER_SIZE_LOG2 16
#define DMA_BUFFER_COUNT     64

#define AUDIO_BASE 90000

#define VIDEO_FIFO_SIZE (2*1024*1024)
#define AUDIO_FIFO_SIZE (256*1024)

#define VIDEO_XFER_FIFO_COUNT 1024
#define AUDIO_XFER_FIFO_COUNT 1024

#define VIDEO_PREBUFFER_SIZE (2048) // in KB
#define AUDIO_PREBUFFER_SIZE ( 128) // in KB

#define KEYFLAGS (SET_KEY_DISPLAY | SET_KEY_PLAYBACK | SET_KEY_AUDIO | SET_KEY_SPI)

#define RM_DEVICES_STC 0x1
#define RM_DEVICES_VIDEO 0x2
#define RM_DEVICES_AUDIO 0x4

#define SEND_VIDEO_TRACK 1
#define SEND_AUDIO_TRACK 2

#if 0
  #define SENDDBG ENABLE
#else
  #define SENDDBG DISABLE
#endif

#if 0
  #define DBG ENABLE
#else
  #define DBG DISABLE
#endif

#if 0
  #define INTERLEAVE_DBG ENABLE
#else
  #define INTERLEAVE_DBG DISABLE
#endif

#if 0
  #define H264_SC_DBG ENABLE
#else
  #define H264_SC_DBG DISABLE
#endif

#define DMABUFFER_UNUSED_BLOCK_SIZE 256

#define PLAY_TIMERS 1

enum goto_label {
	LABEL_ERROR = 1,
	LABEL_NONE,
	LABEL_CLEANUP,
	LABEL_MAINLOOP,
	LABEL_MAINLOOP_NOSEEK,
	LABEL_START_SENDING_DATA,
	LABEL_SIGNAL_EOS_AND_EXIT
};

struct priv_cmdline {
	RMuint32 audio_track;
	RMuint32 internal_file_open;	// should we open the file internally
	RMbool monitorFIFOs;
};


struct SendMKVDataContext {
	struct RUA *pRUA;
	struct DCC *pDCC;
	struct RUABufferPool *pDMA;
	RMbool FirstSystemTimeStamp;
	RMbool WaitAtExit;


	RMfile mkv_file;
	ExternalRMmkvClient mkvc;
	ExternalRMmkvTrack mkvtV, mkvtA;
	RMmkvSample audiosample, videosample;

	RMbool SendVideoData;
	RMbool SendAudioData;

	RMbool SendVideoPts;
	RMbool SendAudioPts;

	RMuint32 videoDataSent;
	RMuint32 audioDataSent;

	RMbool  isIFrameMode;
	RMint32 IFrameDirection;
	RMbool  isTrickMode;

	RMuint32 video_vop_tir, audio_vop_tir;	// is AudioCTSTimeScale
	RMuint32 VideoCTSTimeScale;
	RMuint32 AudioCTSTimeScale;

	RMuint32 audioSampleRate;

	RMuint32 AAC_SampleRateFromDSI;
	RMuint32 AAC_ChannelsFromDSI;
	RMuint32 AAC_ObjectIDFromDSI;

	RMbool isAudioOnly;

	RMuint64 lastSTC;
	RMuint64 Duration;
	RMint64 fileSize;

	struct emhwlib_info video_Info;
	struct emhwlib_info audio_Info;

	struct dcc_context *dcc_info;
	struct RM_PSM_Context *PSMcontext;

	RMbool video_ptsScalingMsg;
	RMbool audio_ptsScalingMsg;
	RMbool forceTimersToZero;

	RMbool playbackAllowed;

	RMuint32 videoTracks;
	RMuint32 audioTracks;

	RMuint32 currentVideoTrack;
	RMuint32 currentAudioTrack;

	RMuint8 *videodsiBuf; 
	RMuint32 videodsiSize;
	RMbool sendVideoDSI;

	RMuint32 videoWidth;
	RMuint32 videoHeight;

	RMbool initTerminal;

	// for prebuffering
	RMuint32 videoPrebufferSize;
	RMuint32 audioPrebufferSize;
	RMbool prebufferingDone;

	struct RMFileStreamInfo streamInfo;

	RMbool isAACaudioPayload;

	RMint32 avRatio;

	RMuint32 dmaBufferCount;
	RMuint32 dmaBufferSizeLog2;
	RMuint32 videoBitstreamFIFOSize;
	RMuint32 videoFIFOXFERCount;
	RMuint32 audioBitstreamFIFOSize[MAX_AUDIO_DECODER_INSTANCES];
	RMuint32 audioFIFOXFERCount[MAX_AUDIO_DECODER_INSTANCES];

	RMuint64 lastVideoPTS;
	RMuint64 lastAudioPTS;

	RMbool   videoEOS;
	RMbool   audioEOS;

	RMuint8 *startCodeDMABuffer;
	RMuint32 startCodeDMABufferOffset;

	RMuint8 *videoDMABuffer;
	RMuint32 videoDMABufferOffset;
	RMbool   videoTransferPending;

	RMuint8 *audioDMABuffer;
	RMuint32 audioDMABufferOffset;
	RMbool   audioTransferPending;

	RMuint8  h264DummyBuffer[4];
	RMuint8 *h264PktLenBuf;
	RMuint32 h264PktLen;
	RMuint32 h264LengthSize;
	RMuint32 h264BytesLeft;
	RMbool   h264sendStartCode;
	RMbool   h264readPacketSize;

	RMuint32 videoTrackSize;
	RMuint32 audioTrackSize;
	RMuint32 videoMeanSampleSize;
	RMuint32 audioMeanSampleSize;

	RMbool monitorFIFOs;

	RMbool linearInterleaving;

	void **dmabuffer_array;
	RMuint32 dmabuffer_index;

	RMbool isH264;

	struct RM_PSM_Actions actions;
	enum RM_PSM_State ACTUAL_PlaybackStatus;
	enum RM_PSM_State OLD_PlaybackStatus;
	RMuint32 Ntimes;

	struct playback_cmdline *play_opt;
	struct video_cmdline *video_opt;
	struct display_cmdline *disp_opt;
	struct audio_cmdline *audio_opt;
	struct priv_cmdline priv_opt;

	RMuint32 audioInstances;
	
	struct timeval last;
	RMbool first;

	RMuint32 videoSampleSize;
	RMuint32 videoFrameCount;  // equivalent to AUs

	// the result of local_process_key are put here
	enum goto_label processKeyResult;
	RMbool processKeyResultValid;
	
	RMint64 stc_offset_ms;

	RMint32 meanCounter;
	RMint64 videoBitrateCounter;
	RMint64 meanVideoBitrate;
	RMint64 pseudoInstantaneousVideoBitrate;
	RMint64 maxPseudoInstantaneousVideoBitrate;

	RMint64 lastVideoDataSentValue;
	RMint64 meanVideoReadSpeed;
	RMint64 meanVideoReadSpeedCounter;
	RMint64 maxVideoReadSpeed;

};

#define GET_DATA_FIFO_INFO(pRUA, ModuleId)				\
	{								\
		struct DataFIFOInfo DataFIFOInfo;			\
		RMuint32 fullness;					\
		RUAGetProperty(pRUA, ModuleId, RMGenericPropertyID_DataFIFOInfo, &DataFIFOInfo, sizeof(DataFIFOInfo)); \
		fullness = (100*DataFIFOInfo.Readable)/DataFIFOInfo.Size; \
		fprintf(stderr, "Data 0x%lx: st=0x%08lx sz=%ld wr=%ld rd=%ld --> f : %lu/100\n", ModuleId, DataFIFOInfo.StartAddress,	\
			DataFIFOInfo.Size, DataFIFOInfo.Writable,  DataFIFOInfo.Readable, fullness); \
	}								\


#define GET_XFER_FIFO_INFO(pRUA, ModuleId)				\
	{								\
		struct XferFIFOInfo_type XferFIFOInfo;			\
		RMuint32 fullness;					\
		RUAGetProperty(pRUA, ModuleId, RMGenericPropertyID_XferFIFOInfo, &XferFIFOInfo, sizeof(XferFIFOInfo)); \
		fullness = (100*XferFIFOInfo.Readable)/XferFIFOInfo.Size; \
		fprintf(stderr, "XFER 0x%lx: st=0x%08lx sz=%ld wr=%ld rd=%ld er=%lx --> f : %lu/100\n", ModuleId, XferFIFOInfo.StartAddress, \
			XferFIFOInfo.Size, XferFIFOInfo.Writable,  XferFIFOInfo.Readable, XferFIFOInfo.Erasable, fullness); \
	}

#define GET_XFER_FIFO_BYTES_QUEUED(pRUA, ModuleId)			\
	{								\
		RMuint32 bytes_queued;					\
		RMstatus err;						\
		err = RUAGetProperty(pRUA, ModuleId, RMGenericPropertyID_XferFIFOBytesQueued, &bytes_queued, sizeof(bytes_queued)); \
		if (err == RM_OK)					\
			fprintf(stderr, "XFER 0x%lx: %lu\n", ModuleId, bytes_queued); \
	}


#define MONITOR_FIFO_INTERVAL_US 250000

static RMstatus SyncTimerWithDecoderPTS(struct SendMKVDataContext *pSendContext);

static void monitorFIFO(struct SendMKVDataContext *context, RMbool alwaysShow)
{	
	RMstatus status;
	struct timeval now;
	RMuint64 elapsed;
	RMuint64 ptime;
	struct dcc_context *dcc_info = context->dcc_info;

	gettimeofday(&now, NULL);
	elapsed = (now.tv_sec - context->last.tv_sec) * 1000000;
	elapsed += (now.tv_usec - context->last.tv_usec);

	if (elapsed > MONITOR_FIFO_INTERVAL_US || alwaysShow) {

		DCCSTCGetTime(dcc_info->pStcSource, &ptime, context->video_vop_tir);
		fprintf(stderr, "\n*****************************\n");
		fprintf(stderr, "STC = %lld (%lld secs)\n", ptime, (ptime/context->video_vop_tir));
		/* sample code to get fifo info */
		if (dcc_info->pVideoSource) {
			RMuint32 dummy32;
			RMuint64 dummy64;
			RMint64 readSpeed;

			readSpeed = context->videoDataSent - context->lastVideoDataSentValue;
			readSpeed = (readSpeed * 8 * 1000000) / elapsed;

			if (readSpeed > context->maxVideoReadSpeed)
				context->maxVideoReadSpeed = readSpeed;

			context->lastVideoDataSentValue = context->videoDataSent;

			context->meanVideoReadSpeed += readSpeed;
			context->meanVideoReadSpeedCounter++;

			fprintf(stderr, "Video :\n");
			
			if (context->meanCounter) {
				RMint64 meanVideoBitrate, meanReadSpeed;

				meanVideoBitrate = context->meanVideoBitrate / context->meanCounter;
				meanReadSpeed = context->meanVideoReadSpeed / context->meanVideoReadSpeedCounter;
				

				fprintf(stderr, "bitrate:   mean %lld bit/sec; pseudo-instantaneus %lld bit/sec (max %lld) (%llu bytes since last PTS)\n", 
					meanVideoBitrate,
					context->pseudoInstantaneousVideoBitrate, 
					context->maxPseudoInstantaneousVideoBitrate,
					context->videoBitrateCounter >> 3);
				fprintf(stderr, "readspeed: mean %lld bit/sec; pseudo-instantaneus %lld bit/sec (max %lld) (%lld bytes/%llu usec)\n",
					meanReadSpeed,
					readSpeed,
					context->maxVideoReadSpeed,
					readSpeed >> 3,
					elapsed);

				if (meanVideoBitrate > meanReadSpeed)
					fprintf(stderr, ">> read speed is lower than bitstream rate!\n");
				
			}
					
			
			GET_DATA_FIFO_INFO(dcc_info->pRUA, dcc_info->video_decoder);
			GET_XFER_FIFO_INFO(dcc_info->pRUA, dcc_info->video_decoder);
			GET_XFER_FIFO_BYTES_QUEUED(dcc_info->pRUA, dcc_info->video_decoder);

			status = RUAGetProperty(dcc_info->pRUA, dcc_info->video_decoder, RMVideoDecoderPropertyID_DecodedPictureCount, &dummy32, sizeof(dummy32));
			if (status == RM_OK)
				fprintf(stderr, "module 0x%lx: DecodedPictureCount %lu\n", dcc_info->video_decoder, dummy32);
			
			status = RUAGetProperty(dcc_info->pRUA, dcc_info->video_decoder, RMVideoDecoderPropertyID_SkippedPictureCount, &dummy32, sizeof(dummy32));
			if (status == RM_OK)
				fprintf(stderr, "module 0x%lx: SkippedPictureCount %lu\n", dcc_info->video_decoder, dummy32);

			status = RUAGetProperty(dcc_info->pRUA, dcc_info->video_decoder, RMVideoDecoderPropertyID_Error, &dummy32, sizeof(dummy32));
			if (status == RM_OK)
				fprintf(stderr, "module 0x%lx: Error 0x%lx\n", dcc_info->video_decoder, dummy32);

			status = RUAGetProperty(dcc_info->pRUA, dcc_info->video_decoder, RMVideoDecoderPropertyID_LastDecodedPTS, &dummy64, sizeof(dummy64));
			if (status == RM_OK)
				fprintf(stderr, "module 0x%lx: LastDecodedPTS %lld\n", dcc_info->video_decoder, dummy64);


		}
		if (dcc_info->pMultipleAudioSource) {
			struct DCCAudioSourceHandle audioHandle;
			RMuint32 i;

			for (i = 0; i < context->audioInstances; i++) {
				fprintf(stderr, "Audio[%lu] :\n", i);
				
				DCCMultipleAudioSourceGetSingleDecoderHandleForInstance(dcc_info->pMultipleAudioSource, i, &audioHandle);
						
				GET_DATA_FIFO_INFO(dcc_info->pRUA, audioHandle.moduleID);
				GET_XFER_FIFO_INFO(dcc_info->pRUA, audioHandle.moduleID);
				GET_XFER_FIFO_BYTES_QUEUED(dcc_info->pRUA, audioHandle.moduleID);
			}
		}
		fprintf(stderr, "*****************************\n");
		gettimeofday(&(context->last), NULL);

		fflush(stderr);
		
	}

	return;
}

/* prototypes */
static RMuint64 round_int_div(RMuint64 numerator, RMuint32 divisor);
static RMstatus mkv_select_audio_track(struct SendMKVDataContext *pSendContext, RMuint32 index);
static void get_buffer(struct SendMKVDataContext *pSendContext, RMuint8 **buf);


RMuint64 round_int_div(RMuint64 numerator, RMuint32 divisor) {
	RMuint64 temp;
	temp = numerator / divisor;
	if ((numerator % divisor) * 2 > divisor)
		temp++;
	return temp;
}


/* highspeed iframe mode works only when using STC timers */

#define MEAN_DEPTH 7
#define DIVERGENCE_TRIGGER 10
static RMint64 shiftReg1[MEAN_DEPTH];
static RMint64 shiftReg2[MEAN_DEPTH];
static RMint32 count1 = 0, count2 = 0;

static RMstatus computeSpeed(struct SendMKVDataContext *pSendContext)
{
	RMuint32 samplingT = 200; // 1/5 sec
	RMuint64 stc;
	RMstatus err;
	enum RM_PSM_State PlaybackStatus = RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info));

	if (!pSendContext->dcc_info->seek_supported)
		return RM_OK;

	if ((PlaybackStatus == RM_PSM_IForward) || (PlaybackStatus == RM_PSM_IRewind)) {

		RMint32 N;
		RMuint32 M;
		RMuint32 speedX, speedX2;
		RMint64 currentPTS, diff1, diff2, currentDecoded, delta = 0;
		RMint64 speed1 = 0, speed2 = 0, sum1, sum2;
		RMuint32 i;
		RMbool skip = TRUE;

		DCCSTCGetTime(pSendContext->dcc_info->pStcSource, &stc, 1000);
		DCCSTCGetSpeed(pSendContext->dcc_info->pStcSource, &N, &M);

		if (N > 0) {

			samplingT *= N;
			samplingT = (RMuint32) round_int_div((RMuint64)samplingT, M);

			if (stc > pSendContext->lastSTC + samplingT) {
		
				delta = stc - pSendContext->lastSTC;
				pSendContext->lastSTC = stc;
				err = RUAGetProperty(pSendContext->dcc_info->pRUA, pSendContext->dcc_info->SurfaceID, RMGenericPropertyID_CurrentDisplayPTS, &currentPTS, sizeof(currentPTS));
				err = RUAGetProperty(pSendContext->dcc_info->pRUA, pSendContext->dcc_info->video_decoder, RMVideoDecoderPropertyID_LastDecodedPTS, &currentDecoded, sizeof(currentDecoded));
				diff1 = (RMint64)stc - (RMint64)(currentPTS / 45);
				diff2 = (RMint64)stc - (RMint64)(currentDecoded / 45);
				speed1 = diff1;
				speed2 = diff2;

				skip = FALSE;
			}
		}
		else {
			N *= -1;
			samplingT *= N;
			samplingT = (RMuint32) round_int_div((RMuint64)samplingT, M);

			if (stc < pSendContext->lastSTC - samplingT) {
				

				delta = pSendContext->lastSTC - stc;
				pSendContext->lastSTC = stc;
				err = RUAGetProperty(pSendContext->dcc_info->pRUA, pSendContext->dcc_info->SurfaceID, RMGenericPropertyID_CurrentDisplayPTS, &currentPTS, sizeof(currentPTS));
				err = RUAGetProperty(pSendContext->dcc_info->pRUA, pSendContext->dcc_info->video_decoder, RMVideoDecoderPropertyID_LastDecodedPTS, &currentDecoded, sizeof(currentDecoded));
				diff1 = (RMint64)(currentPTS / 45) - (RMint64)stc;
				diff2 = (RMint64)(currentDecoded / 45) - (RMint64)stc;
				speed1 = diff1;
				speed2 = diff2;
				
				skip = FALSE;
			}
		}

		if (skip == FALSE) {
			sum1 = sum2 = 0;
			for (i = 0; i < MEAN_DEPTH; i++) {
				if (shiftReg1[0] < shiftReg1[i])
					sum1++;
				if (shiftReg2[0] < shiftReg2[i])
					sum2++;
			}
		
			if (sum1 > MEAN_DEPTH/2)
				count1++;
			else
				count1 = 0;
		
			if (sum2 > MEAN_DEPTH/2)
				count2++;
			else
				count2 = 0;
		
			speedX = (RMuint32)round_int_div((RMuint64)N, M);
			speedX2 = (RMuint32)round_int_div((RMuint64)delta, 200);
		
			RMDBGLOG((ENABLE, "time %llu (%ld/%lu=%lux, %lu), sampling %lu, delta %4llu, sum1 %lld, sum2 %lld, diff3 %lld(%ld), diff4 %lld(%ld) %s\n", 
				  stc,
				  N,
				  M,
				  speedX,
				  speedX2,
				  samplingT,
				  delta,
				  sum1,
				  sum2,
				  speed1,
				  count1,
				  speed2,
				  count2,
				  ((count1 > DIVERGENCE_TRIGGER) && (count2 > DIVERGENCE_TRIGGER)) ? "DIVERGE!":""));
		
			pSendContext->lastVideoPTS = currentPTS;
            //	pSendContext->lastDecoded = currentDecoded;
		
			for (i = 0 ; i < MEAN_DEPTH-1 ; i++)
				shiftReg1[i] = shiftReg1[i+1];
			shiftReg1[MEAN_DEPTH-1] = speed1;
		
			for (i = 0 ; i < MEAN_DEPTH-1 ; i++)
				shiftReg2[i] = shiftReg2[i+1];
			shiftReg2[MEAN_DEPTH-1] = speed2;
		
            //	if ((count1 > DIVERGENCE_TRIGGER) && 
            //  (count2 > DIVERGENCE_TRIGGER) &&
            //  (!pSendContext->highSpeedIFrameMode)) {
            //RMDBGLOG((ENABLE, "enable high speed iframe mode\n"));
				//pSendContext->highSpeedIFrameMode = TRUE;
			//}
			//pSendContext->highSpeedIFrameSpeed = speedX;
		}
	}
	
	return RM_OK;
}

#define PROCESS_KEY(release, getkey)					\
do {								        \
	local_process_key(&SendContext, getkey, release);		\
	if (SendContext.ACTUAL_PlaybackStatus != SendContext.OLD_PlaybackStatus) {		\
		RMDBGLOG((ENABLE, "old status %lu new status %lu\n", (RMuint32)SendContext.OLD_PlaybackStatus, (RMuint32)SendContext.ACTUAL_PlaybackStatus)); \
		SendContext.OLD_PlaybackStatus = SendContext.ACTUAL_PlaybackStatus;		\
	}								\
	if (SendContext.processKeyResultValid) {			\
		SendContext.processKeyResultValid = FALSE;		\
		switch(SendContext.processKeyResult) {			\
		case LABEL_ERROR:					\
		case LABEL_CLEANUP:					\
			goto cleanup;					\
		case LABEL_MAINLOOP:					\
			goto mainloop;					\
		case LABEL_MAINLOOP_NOSEEK:				\
			goto mainloop_noseek;				\
		case LABEL_START_SENDING_DATA:				\
			goto start_sending_data;			\
		case LABEL_SIGNAL_EOS_AND_EXIT:				\
			goto signal_EOS_and_exit_loop;			\
		case LABEL_NONE:					\
			RMDBGLOG((ENABLE, "warning unrecognised key pressed\n"));\
			break;						\
		}							\
	}								\
	computeSpeed(&SendContext);			\
} while(0)





#define ADTS_HEADER_SIZE 7

static RMuint32 FindAdtsSRIndex(RMuint32 sr)
{
	const RMuint32 adts_sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350,0,0,0};
	RMuint32 i;
	
	for (i = 0; i < 16; i++) {
		if (sr == adts_sample_rates[i])
			return i;
	}

	return 0xf;
}

static void FillAdtsHeader(RMuint8 *buf, RMuint32 framesize, RMuint32 samplerate, RMuint32 channelcount, RMuint32 objectID)
{
	RMuint32 profile = (objectID - 1) & 0x3;
	RMuint32 sr_index = FindAdtsSRIndex(samplerate);

	framesize += ADTS_HEADER_SIZE;  /* add adts header size */

	buf[0] = 0xFF; /* 8b: syncword */
	buf[1] = 0xF1; /* 4b: syncword */
	               /* 1b: mpeg id = 0 */
	               /* 2b: layer = 0 */
	               /* 1b: protection absent = 1 */
	
	buf[2] = ((profile << 6) & 0xC0);  /* 2b: profile */
	buf[2] += ((sr_index << 2) & 0x3C); /* 4b: sampling_frequency_index */
	                                    /* 1b: private = 0 */
	buf[2] += ((channelcount >> 2) & 0x1); /* 1b: channel_configuration */
	
	buf[3] = ((channelcount << 6) & 0xC0); /* 2b: channel_configuration */
	/* 1b: original = 0 */
	/* 1b: home = 0 */
	/* 1b: copyright_id = 0 */
	/* 1b: copyright_id_start = 0 */
	buf[3] += ((framesize >> 11) & 0x3); /* 2b: aac_frame_length */
	
	buf[4] = ((framesize >> 3) & 0xFF); /* 8b: aac_frame_length */
	
	buf[5] = ((framesize << 5) & 0xE0); /* 3b: aac_frame_length */
	buf[5] += ((0x7FF >> 6) & 0x1F); /* 5b: adts_buffer_fullness */
	
	buf[6] = ((0x7FF << 2) & 0xFC); /* 6b: adts_buffer_fullness */
	/* 2b: num_raw_buf_blocks  = 0 */
}

static RMstatus init_private_options(struct priv_cmdline *options)
{
	options->audio_track = 0;
	options->internal_file_open = 0;
	options->monitorFIFOs = FALSE;	

	return RM_OK;
}

#ifndef WITH_MONO
static RMstatus parse_private_cmdline(struct SendMKVDataContext *pSendContext, int argc, char **argv, int *index, struct priv_cmdline *options)
{
	RMstatus err = RM_PENDING;
	int i = *index;

	if (! strcmp(argv[i], "-at")) {
		if (argc > i+1) {
			options->audio_track = strtol(argv[i+1], NULL, 10);
			i+=2;
			err = RM_OK;
		}
		else
			err = RM_ERROR;
	} else if ( ! strcmp(argv[i], "-fopen")) {
		if (argc > i+1) {
			options->internal_file_open = strtol(argv[i+1], NULL, 10);
			i+=2;
			err = RM_OK;
		}
		else
			err = RM_ERROR;
	}
	else if ( ! strcmp(argv[i], "-monitor")) {
		options->monitorFIFOs = TRUE;
		err = RM_OK;
		i++;
	}
	
	*index = i;
	
	return err;
}


static void show_private_options(void)
{
	fprintf(stderr, 
		"MKV OPTIONS (default values inside brackets)\n"
		"\t-at track: Selects audio track number track [0]\n"
		"\t-fopen <0|1>: use internal file open [0]\n");
}

static void show_usage(char *progname)
{
	show_private_options();
	show_playback_options();
	show_display_options();
	show_video_options();
	show_audio_options();
	fprintf(stderr, 
		"--------------------------------\n"
		"Minimum cmd line: %s <file name>\n"
		"--------------------------------\n", 
		progname);

	exit(1);
}

static void parse_cmdline(struct SendMKVDataContext *pSendContext, int argc, char *argv[], RMuint32 *currentAudioInstance)
{
	int i;
	RMstatus err;

	if (argc < 2) 
		show_usage(argv[0]);
	
	i = 1;
	while ((argc > i)) {
		if (argv[i][0] != '-') {
			if (pSendContext->play_opt->filename == NULL) {
				pSendContext->play_opt->filename = argv[i];
				i++;
			}
			else
				show_usage(argv[0]);
		}
		else {
			err = parse_private_cmdline(pSendContext, argc, argv, &i, &(pSendContext->priv_opt));
			if (err == RM_ERROR)
				show_usage(argv[0]);
			if (err != RM_PENDING)
				continue;
			err = parse_playback_cmdline(argc, argv, &i, pSendContext->play_opt);
			if (err == RM_ERROR) 
				show_usage(argv[0]);
			if (err != RM_PENDING)
				continue;
			err = parse_display_cmdline(argc, argv, &i, pSendContext->disp_opt);
			if (err == RM_ERROR) 
				show_usage(argv[0]);
			if (err != RM_PENDING)
				continue;
			err = parse_video_cmdline(argc, argv, &i, pSendContext->video_opt);
			if (err == RM_ERROR) 
				show_usage(argv[0]);
			if (err != RM_PENDING)
				continue;

			RMDBGLOG((ENABLE, "option[%lu] %s\n", i, argv[i]));
			err = parse_audio_cmdline2(argc, argv, &i, pSendContext->audio_opt, MAX_AUDIO_DECODER_INSTANCES, currentAudioInstance);
			if (RMFAILED(err))
				show_usage(argv[0]);
		}
	}

	if (pSendContext->play_opt->filename == NULL)
		show_usage(argv[0]);
}
#endif //WITH_MONO

static void check_prebuf_state(struct SendMKVDataContext * pSendContext, RMuint32 buffersize);

static RMstatus WaitForEOS(struct SendMKVDataContext *pSendContext, struct RM_PSM_Actions *pActions)
{
	RMuint32 eos_bit_field = 0;
	enum RM_PSM_State PlaybackStatus = RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info));

	if (pSendContext == NULL || pActions == NULL)
		return RM_ERROR;
	
	if (pSendContext->SendVideoData) 
		eos_bit_field |= EOS_BIT_FIELD_VIDEO;

	if (pSendContext->SendAudioData && 
	    ((PlaybackStatus == RM_PSM_Playing) ||
	     (PlaybackStatus == RM_PSM_Prebuffering)))
		eos_bit_field |= EOS_BIT_FIELD_AUDIO;
	
	return WaitForEOSWithCommand(pSendContext->PSMcontext, &(pSendContext->dcc_info), pActions, eos_bit_field);
}



static RMstatus init_MKV_tracks(struct SendMKVDataContext *pSendContext)
{
	RMuint32 videotrackID = 0;
	RMstatus status;
    RMbool deinterlaced = FALSE;

	RMDBGLOG((ENABLE, "initTracks\n"));

	pSendContext->videoTracks = RMGetMKVNumberOfVideoTracks(pSendContext->mkvc);
	RMDBGLOG((ENABLE, "found %lu video tracks\n", pSendContext->videoTracks));

	if (pSendContext->videoTracks > 0) {
		/* so far, all files have only one video track, init the first video track found */
		RMDBGLOG((ENABLE, "init mkv video track\n"));
		pSendContext->currentVideoTrack = 1;
		pSendContext->mkvtV = (ExternalRMmkvTrack) NULL;
		videotrackID = RMGetMKVVideoTrackID(pSendContext->mkvc);
		pSendContext->videodsiBuf = (RMuint8 *) NULL;
		if (videotrackID > 0) {
			RMmkvTrackType type;

			pSendContext->mkvtV = RMOpenMKVTrack(pSendContext->mkvc, videotrackID);
			if (!pSendContext->mkvtV) {
				RMDBGLOG((ENABLE, "couldnt open track!\n"));
				goto disable_video;
			}
			RMDBGLOG((ENABLE, "video track ID is %lu\n", videotrackID));

			RMGetMKVTrackWidth(pSendContext->mkvtV, &(pSendContext->videoWidth));
			RMGetMKVTrackHeight(pSendContext->mkvtV, &(pSendContext->videoHeight));
            RMGetMKVTrackDeinterlaced (pSendContext->mkvtV, &deinterlaced);

			RMDBGLOG((ENABLE, ">> video size %lu x %lu, %s\n", pSendContext->videoWidth, pSendContext->videoHeight, deinterlaced?"deinterlaced":"not deinterlaced"));

			RMGetMKVTrackType(pSendContext->mkvtV, &type);

			if (type == RM_MKV_VIDEO_H264_TRACK)
				pSendContext->isH264 = TRUE;
			else if (type == RM_MKV_VIDEO_MPEG4_TRACK)
				pSendContext->isH264 = FALSE;
			else {
				RMDBGLOG((ENABLE, "unknown video!!\n"));
				goto disable_video;
			}

			if (pSendContext->isH264) {
				RMuint32 level, profile, lengthSize;

				RMDBGLOG((ENABLE, ">> video track is H264\n"));

				RMGetMKVH264Level(pSendContext->mkvtV, &level);
				RMGetMKVH264Profile(pSendContext->mkvtV, &profile);
				RMGetMKVH264LengthSize(pSendContext->mkvtV, &lengthSize);

				pSendContext->h264LengthSize = lengthSize;

				RMDBGLOG((ENABLE, "level %lu, profile %lu, lengthSize %lu\n", level, profile, lengthSize));


				if (level > 51) {
					fprintf(stderr, "unsupported level, level %lu > max 51\n", level);
					goto disable_video;
				}

				if (profile > 100) {
					fprintf(stderr, "unsupported profile, profile %lu > max 100\n", profile);
					goto disable_video;
				}

//#ifdef WITH_MONO
				RMDBGLOG((ENABLE, ">> set codec to H264 HD\n"));
                if (deinterlaced) {
                    pSendContext->video_opt->MPEGProfile = Profile_H264_HD_DeInt;
                    pSendContext->video_opt->Codec = VideoDecoder_Codec_H264_HD_DeInt;
                } else {
                    pSendContext->video_opt->MPEGProfile = Profile_H264_HD;
                    pSendContext->video_opt->Codec = VideoDecoder_Codec_H264_HD;
                }
//#endif
			}
			else {
				RMDBGLOG((ENABLE, ">> video track is MPEG4\n"));
//#ifdef WITH_MONO
				RMDBGLOG((ENABLE, ">> set codec to MPEG4\n"));
                if (deinterlaced) {
                    pSendContext->video_opt->MPEGProfile = Profile_MPEG4_HD_DeInt;
                    pSendContext->video_opt->Codec = VideoDecoder_Codec_MPEG4_HD_DeInt;
                } else {
                    pSendContext->video_opt->MPEGProfile = Profile_MPEG4_HD;
                    pSendContext->video_opt->Codec = VideoDecoder_Codec_MPEG4_HD;

                }
//#endif
			}
			

			pSendContext->videodsiBuf = RMGetMKVTrackDSI(pSendContext->mkvtV, &(pSendContext->videodsiSize));
			RMDBGLOG((ENABLE, "got %lu bytes of video DSI, parsing...\n", pSendContext->videodsiSize));

			if (pSendContext->videodsiSize > (RMuint32) (1<<pSendContext->dmaBufferSizeLog2)) {
				RMDBGLOG((ENABLE, "video DSI too big!!\n"));
				goto disable_video;
			}
			else if (pSendContext->videodsiSize == 0) {
				RMDBGLOG((ENABLE, "video DSI size is zero!!\n"));
				goto disable_video;
			}
			
			pSendContext->VideoCTSTimeScale = RMGetMKVTrackTimeScale(pSendContext->mkvtV);
			RMDBGLOG((ENABLE, "video cts timescale = %lu\n", pSendContext->VideoCTSTimeScale));

			switch (pSendContext->video_opt->MPEGProfile){
			case Profile_FIRST_:
			case Profile_LAST_:
				break;
				
			case Profile_MPEG2_SD:
			case Profile_MPEG2_DVD:
			case Profile_MPEG2_HD:
			case Profile_MPEG2_SD_Packed:
			case Profile_MPEG2_HD_Packed:
			case Profile_MPEG2_DVD_Packed:
			case Profile_MPEG2_SD_DeInt:
			case Profile_MPEG2_DVD_DeInt:
			case Profile_MPEG2_HD_DeInt:
			case Profile_MPEG2_SD_Packed_DeInt:
			case Profile_MPEG2_DVD_Packed_DeInt:
			case Profile_MPEG2_HD_Packed_DeInt:
				RMDBGLOG((ENABLE, "MPEG 2 video\n"));
				pSendContext->video_vop_tir = 90000;
				break;
			
			case Profile_DIVX3_SD:
			case Profile_DIVX3_HD:
			case Profile_DIVX3_SD_Packed:
			case Profile_DIVX3_HD_Packed:
				RMDBGLOG((ENABLE, "DIVX3 video\n"));
				pSendContext->video_vop_tir = pSendContext->VideoCTSTimeScale;
				break;

			case Profile_WMV_SD:
			case Profile_WMV_816P:
			case Profile_WMV_HD:
				RMDBGLOG((ENABLE, "WMV9 video\n"));
				pSendContext->video_vop_tir = 1000;
				break;

			case Profile_MPEG4_SD:
			case Profile_MPEG4_HD:
			case Profile_MPEG4_SD_Packed:
			case Profile_MPEG4_HD_Packed:
			case Profile_MPEG4_SD_DeInt:
			case Profile_MPEG4_HD_DeInt:
			case Profile_MPEG4_SD_Packed_DeInt:
			case Profile_MPEG4_HD_Packed_DeInt:
			case Profile_MPEG4_SD_Padding:
			case Profile_MPEG4_HD_Padding:
			case Profile_MPEG4_SD_DeInt_Padding:
			case Profile_MPEG4_HD_DeInt_Padding:
				RMDBGLOG((ENABLE, "MPEG4 video\n"));
				//ParseMKVVideoDSI(pSendContext->videodsiBuf, pSendContext->videodsiSize, (void *) &pSendContext->video_vop_tir, sizeof(RMuint32));
				break;

			case Profile_VC1_SD:
			case Profile_VC1_HD:
				RMDBGLOG((ENABLE, "VC1 video\n"));
				pSendContext->video_vop_tir = pSendContext->VideoCTSTimeScale;
				break;
			
			case Profile_H264_SD:
			case Profile_H264_HD:
			case Profile_H264_SD_DeInt:
			case Profile_H264_HD_DeInt:
				RMDBGLOG((ENABLE, "H264 video\n"));
				pSendContext->video_vop_tir = 90000; //pSendContext->VideoCTSTimeScale;
				break;
			}
		
			RMDBGLOG((ENABLE,"********** video VOP TIR %lu, VideoCTSTimeScale %lu\n", pSendContext->video_vop_tir, pSendContext->VideoCTSTimeScale));

			if (!pSendContext->video_vop_tir) {
				RMDBGLOG((ENABLE, "video_vop_tir = 0!!, reset to VideoCTSTimeScale\n"));
				pSendContext->video_vop_tir = pSendContext->VideoCTSTimeScale;
			}

		}
		else
			goto disable_video;

	}
	else {

	disable_video:
		RMDBGLOG((ENABLE, ">> no video\n"));
		pSendContext->SendVideoData = FALSE;
		pSendContext->isAudioOnly = TRUE;
		pSendContext->currentVideoTrack = 0;

		if (pSendContext->mkvtV) {
			RMDBGLOG((ENABLE, "closing video track\n"));
			RMCloseMKVTrack(pSendContext->mkvc, pSendContext->mkvtV);
			pSendContext->mkvtV = (ExternalRMmkvTrack) NULL;
		}
	}
	
	// continue with other tracks
	RMDBGLOG((ENABLE, "init mkv audio track(s)\n"));
	pSendContext->mkvtA = (ExternalRMmkvTrack) NULL;

	pSendContext->audioTracks = RMGetMKVNumberOfAudioTracks(pSendContext->mkvc);
	RMDBGLOG((ENABLE, "found %lu audio tracks\n", pSendContext->audioTracks));

	if (pSendContext->audioTracks > 0) {
		RMuint32 i;

		pSendContext->currentAudioTrack = 0;

		if (pSendContext->priv_opt.audio_track) {
			RMDBGLOG((ENABLE, "open audio track %lu from cmdline\n", pSendContext->priv_opt.audio_track));
			status = mkv_select_audio_track(pSendContext, pSendContext->priv_opt.audio_track);
			if (status == RM_OK) {
				pSendContext->currentAudioTrack = pSendContext->priv_opt.audio_track;
			}
			else {
				RMDBGLOG((ENABLE, "cant select audio track %lu\n", pSendContext->priv_opt.audio_track));
				goto disable_audio;
			}
		}
			
		for (i = 1; i <= pSendContext->audioTracks; i++) {
			// init the first working audio track
			status = mkv_select_audio_track(pSendContext, i);
			if (status != RM_OK)
				RMDBGLOG((ENABLE, "cant select audio track %lu, skipping!\n", i));
			else {
				pSendContext->currentAudioTrack = i;
				break;
			}
		}

		if (pSendContext->currentAudioTrack != i) {

		disable_audio:
			// all audio tracks failed init, disable audio
			RMDBGLOG((ENABLE, "all audio tracks failed init, disabling audio\n"));
			pSendContext->SendAudioData = FALSE;
			pSendContext->isAudioOnly = FALSE;
			pSendContext->currentAudioTrack = 0;
			if (pSendContext->mkvtA) {
				RMDBGLOG((ENABLE, "closing audio track\n"));
				RMCloseMKVTrack(pSendContext->mkvc, pSendContext->mkvtA);
				pSendContext->mkvtA = (ExternalRMmkvTrack) NULL;
			}
		}
	}
	else {
		RMDBGLOG((ENABLE, ">> no audio\n"));
		pSendContext->SendAudioData = FALSE;
		pSendContext->isAudioOnly = FALSE;
		pSendContext->currentAudioTrack = 0;
	}

	/* exit */
	if ((pSendContext->audioTracks == 0) && (pSendContext->videoTracks == 0))
		return RM_ERROR;

	RMDBGLOG((ENABLE, "initTracks done, video %lu(of %lu), audio %lu(of %lu)\n",
              pSendContext->currentVideoTrack,
              pSendContext->videoTracks,
              pSendContext->currentAudioTrack,
              pSendContext->audioTracks));
	return RM_OK;

}

/* for audio and spu (spu has a dsisize=64 always, is the palette) */
#define MAX_DSI_SIZE 64 

static RMstatus mkv_select_audio_track(struct SendMKVDataContext *pSendContext, RMuint32 index)
{
	RMuint32 audiotrackID = 0;
	RMstatus status;
	ExternalRMmkvTrack newTrack;

	if (index > pSendContext->audioTracks)
		return RM_ERROR;


	if (index == pSendContext->currentAudioTrack)
		return RM_OK;


	status = RMGetMKVAudioTrackIDByIndex(pSendContext->mkvc, index, &audiotrackID);
	if (status != RM_OK) {
		RMDBGLOG((ENABLE, "cant get id for audio track %lu!\n", index));
		return status;
	}

	if (audiotrackID > 0) {
 		RMuint8 *dsibuf;
		RMuint32 dsisize;
		RMuint8 tempdsi[MAX_DSI_SIZE];
		RMuint32 samplerate, channels;
		RMuint8 objID;
		RMmkvTrackType type;

		RMDBGLOG((ENABLE, "selecting audio track %lu, id %lu\n", index, audiotrackID));

		newTrack = RMOpenMKVTrack(pSendContext->mkvc, audiotrackID);
		if (!newTrack) {
			RMDBGLOG((ENABLE, "couldnt open track!\n"));
			return RM_ERROR;
		}

		RMGetMKVTrackType(newTrack, &type);
		
		if (type == RM_MKV_AUDIO_AAC_TRACK) {
		
			RMDBGLOG((ENABLE, ">> AAC audio\n"));
			dsibuf = RMGetMKVTrackDSI(newTrack, &dsisize);
			
			RMDBGLOG((ENABLE, "got %lu bytes of audio DSI, parsing...\n", dsisize));
			
			if (dsibuf == NULL) {
				RMDBGLOG((ENABLE, "audio DSI not present!\n"));
#ifdef WITH_MONO
				return RM_ERROR;
#else
				return RM_OK;
#endif
			}
			else if (dsisize > MAX_DSI_SIZE) {
				RMDBGLOG((ENABLE, "audio DSI too big!\n"));
				return RM_ERROR;
			}
			else if (dsisize == 0) {
				RMDBGLOG((ENABLE, "audio DSI has size zero!!\n"));
				return RM_ERROR;
			}

			/* copy the DSI in case we overwite it during parsing */
			memcpy(tempdsi, dsibuf, dsisize);
			
			//ParseMKVAudioDSI(tempdsi, &dsisize, &samplerate, &channels, &objID);
			pSendContext->audioSampleRate = samplerate;

			pSendContext->AAC_SampleRateFromDSI = samplerate;
			pSendContext->AAC_ChannelsFromDSI = channels;
			pSendContext->AAC_ObjectIDFromDSI = objID;
			pSendContext->isAACaudioPayload = FALSE;
			RMGetMKVTrackSampleRate(newTrack, &(pSendContext->audioSampleRate));
		}
		else if (type == RM_MKV_AUDIO_AC3_TRACK) {
			RMDBGLOG((ENABLE, ">> AC3 audio\n"));
			pSendContext->isAACaudioPayload = FALSE;
			RMGetMKVTrackSampleRate(newTrack, &(pSendContext->audioSampleRate));
			RMDBGLOG((ENABLE, "sample rate %lu\n", pSendContext->audioSampleRate));

		}
		else if (type == RM_MKV_AUDIO_DTS_TRACK) {
			RMDBGLOG((ENABLE, ">> DTS audio\n"));
			pSendContext->isAACaudioPayload = FALSE;
			RMGetMKVTrackSampleRate(newTrack, &(pSendContext->audioSampleRate));
			RMDBGLOG((ENABLE, "sample rate %lu\n", pSendContext->audioSampleRate));
		}
		else {
			fprintf(stderr, "unknown audio!\n");
			
			if (newTrack) {
				RMDBGLOG((ENABLE, "closing audio track\n"));
				RMCloseMKVTrack(pSendContext->mkvc, newTrack);
			}
			return RM_ERROR;
		}


			
		if (pSendContext->mkvtA) {
			RMDBGLOG((ENABLE, "audio track already open, closing\n"));
			RMCloseMKVTrack(pSendContext->mkvc, pSendContext->mkvtA);
		}

		pSendContext->mkvtA = newTrack;

		pSendContext->audio_vop_tir = AUDIO_BASE;
		pSendContext->AudioCTSTimeScale = RMGetMKVTrackTimeScale(pSendContext->mkvtA);

		RMDBGLOG((ENABLE, "********** audio VOP TIR %ld, AudioCTSTimeScale %ld\n", pSendContext->audio_vop_tir, pSendContext->AudioCTSTimeScale));
	} 
	else
		return RM_ERROR;


	
#ifdef WITH_MONO
	{
		RMuint32 i;
		RMDBGLOG((ENABLE, "setup audio parameters for this track\n"));
		for (i = 0; i < pSendContext->audioInstances; i++) {
			RMmkvTrackType type;
			
			RMGetMKVTrackType(pSendContext->mkvtA, &type);
			
			if (type == RM_MKV_AUDIO_AAC_TRACK) {
				RMDBGLOG((ENABLE, ">> set audio codec[%lu] to AAC\n", i));
				pSendContext->audio_opt[i].Codec = AudioDecoder_Codec_AAC;
				pSendContext->audio_opt[i].AACParams.InputFormat = (enum AACInputFormat)1;
				pSendContext->audio_opt[i].AACParams.OutputChannels = Aac_LR;
			}
			else if (type == RM_MKV_AUDIO_AC3_TRACK) {
				RMDBGLOG((ENABLE, ">> set audio codec[%lu] to AC3\n", i));
				pSendContext->audio_opt[i].Codec = AudioDecoder_Codec_AC3;
				
			}
			else if (type == RM_MKV_AUDIO_DTS_TRACK) {
				RMDBGLOG((ENABLE, ">> set audio codec[%lu] to DTS\n", i));
				pSendContext->audio_opt[i].Codec = AudioDecoder_Codec_DTS;
				
			}

			
			if (!pSendContext->audio_opt[i].OutputChannelsExplicitAssign)
				pSendContext->audio_opt[i].OutputChannels = Audio_Out_Ch_LR;
			
		}
	}
#endif


	return RM_OK;
}

static RMstatus send_videoDSI(struct SendMKVDataContext * pSendContext)
{
	RMuint32 i;
	RMstatus status;

	if (!pSendContext->sendVideoDSI) {
		return RM_OK;
	}

	while (RUAGetBuffer(pSendContext->pDMA, &(pSendContext->videosample.buf),  COMMON_TIMEOUT_US) != RM_OK)
		RMDBGLOG((ENABLE, "there are no free buffers for video DSI right now, retry later...\n"));


	memcpy(pSendContext->videosample.buf, pSendContext->videodsiBuf, pSendContext->videodsiSize);
	
	pSendContext->videosample.size = pSendContext->videodsiSize;
	pSendContext->videosample.flags = 0;
	pSendContext->video_Info.ValidFields = 0;
	pSendContext->video_Info.TimeStamp = 0;

	for (i = 0; i < pSendContext->videodsiSize; i++) {
		RMDBGLOG((DISABLE, "videoDSI[%lu]=%02X\n", i, pSendContext->videodsiBuf[i]));
	}

	if (pSendContext->SendVideoData) {
		while( RUASendData(pSendContext->pRUA, pSendContext->dcc_info->video_decoder, pSendContext->pDMA, 
				   pSendContext->videosample.buf, 
				   pSendContext->videosample.size, 
				   &pSendContext->video_Info, sizeof(pSendContext->video_Info)) != RM_OK) {
			RMDBGLOG((ENABLE, "waiting to send video DSI\n"));
		}
	}

	RMDBGLOG((ENABLE, "video DSI sent, size %ld\n", pSendContext->videodsiSize));

	if (pSendContext->videosample.buf != NULL) {
		RUAReleaseBuffer(pSendContext->pDMA, pSendContext->videosample.buf);
		pSendContext->videosample.buf = NULL;
	}

	pSendContext->sendVideoDSI = FALSE;

	return RM_OK;
}

static RMstatus Stop(struct SendMKVDataContext * pSendContext, RMuint32 devices)
{
	RMstatus err = RM_OK;
	struct dcc_context *dcc_info = pSendContext->dcc_info;
	
	if (devices & RM_DEVICES_STC) {
		RMDBGLOG((ENABLE, "STOP: stc\n"));
		DCCSTCStop(dcc_info->pStcSource);
	}

	if (devices & RM_DEVICES_VIDEO) {
		if (pSendContext->SendVideoData) {
			RMDBGLOG((ENABLE, "STOP: video decoder\n"));
			err = DCCStopVideoSource(dcc_info->pVideoSource, DCCStopMode_LastFrame);
			if (RMFAILED(err)){
				RMDBGLOG((ENABLE, "Error stopping video source %d\n", err));
				return err;
			}
			
			pSendContext->lastVideoPTS = 0;
			pSendContext->sendVideoDSI = TRUE;
		}
	}

	if (devices & RM_DEVICES_AUDIO) {
		if ((pSendContext->SendAudioData) && (dcc_info->pMultipleAudioSource)) {
			RMDBGLOG((ENABLE, "STOP: multiple audio decoders\n"));
			err = DCCStopMultipleAudioSource(dcc_info->pMultipleAudioSource);
			if (RMFAILED(err)){
				RMDBGLOG((ENABLE,"Error stopping multiple audio source %d\n", err));
				return err;
			}

			pSendContext->lastAudioPTS = 0;
		}
	}

	if ((devices & RM_DEVICES_AUDIO) && (devices & RM_DEVICES_VIDEO)) {
		pSendContext->FirstSystemTimeStamp = TRUE;
	}

	
	if (pSendContext->monitorFIFOs) {
		RMDBGLOG((ENABLE, "FIFO STATUS after stop\n"));
		monitorFIFO(pSendContext, TRUE);
		RMDBGLOG((ENABLE, "**********************\n"));
	}

	return err;

}

static RMstatus Play(struct SendMKVDataContext * pSendContext, RMuint32 devices, enum DCCVideoPlayCommand mode)
{

	RMstatus err = RM_OK;
	struct dcc_context *dcc_info = pSendContext->dcc_info;

	if (pSendContext->monitorFIFOs) {
		RMDBGLOG((ENABLE, "FIFO STATUS before play\n"));
		monitorFIFO(pSendContext, TRUE);
		RMDBGLOG((ENABLE, "**********************\n"));
	}
	
	if (devices & RM_DEVICES_STC) {
		RMDBGLOG((ENABLE, "PLAY: stc\n"));
		DCCSTCPlay(dcc_info->pStcSource);
	}

	if (devices & RM_DEVICES_VIDEO) {
		if (pSendContext->SendVideoData) {
			RMDBGLOG((ENABLE, "PLAY: video decoder\n"));
			err = DCCPlayVideoSource(dcc_info->pVideoSource, mode);
			if (RMFAILED(err)) {
				RMDBGLOG((ENABLE, "Cannot play video decoder %d\n", err));
				return err;
			}
		}
		send_videoDSI(pSendContext);
	}

	if (devices & RM_DEVICES_AUDIO) {
		if ((pSendContext->SendAudioData) && (dcc_info->pMultipleAudioSource)) {
			RMDBGLOG((ENABLE, "PLAY: multiple audio decoders\n"));
			err = DCCPlayMultipleAudioSource(dcc_info->pMultipleAudioSource);
			if (RMFAILED(err)) {
				RMDBGLOG((ENABLE, "Cannot play video decoder %d\n", err));
				return err;
			}
		}
	}


	return err;

}

// used for prebuffering
static RMstatus Pause(struct SendMKVDataContext * pSendContext, RMuint32 devices)
{

	RMstatus err = RM_OK;
	
	if (devices & RM_DEVICES_STC) {
		RMDBGLOG((ENABLE, "PAUSE: stc\n"));
		DCCSTCStop(pSendContext->dcc_info->pStcSource);
	}

	if (devices & RM_DEVICES_VIDEO) {
		if (pSendContext->SendVideoData) {
			RMDBGLOG((ENABLE, "PAUSE: video decoder\n"));
			err = DCCPauseVideoSource(pSendContext->dcc_info->pVideoSource);
			if (RMFAILED(err)) {
				RMDBGLOG((ENABLE, "Cannot pause video decoder %d\n", err));
				return err;
			}
		}
	}

	if (devices & RM_DEVICES_AUDIO) {
		if ((pSendContext->SendAudioData) && (pSendContext->dcc_info->pMultipleAudioSource)) {
			RMDBGLOG((ENABLE, "PAUSE: audio decoder\n"));
			err = DCCPauseMultipleAudioSource(pSendContext->dcc_info->pMultipleAudioSource);
			if (RMFAILED(err)) {
				RMDBGLOG((ENABLE, "Cannot pause video decoder %d\n", err));
				return err;
			}
		}
	}


	return err;

}

static RMstatus send_MKV_audio(struct SendMKVDataContext * pSendContext)
{
	RMstatus status;
	RMuint64 pts;
	RMuint8 *buf = NULL;
	enum RM_PSM_State PlaybackStatus = RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info));
	RMuint32 nextSampleSize = 0;

	if (PlaybackStatus == RM_PSM_Stopped) {
		return RM_OK;
	}

	if ((PlaybackStatus != RM_PSM_Playing) && (PlaybackStatus != RM_PSM_Paused) && (PlaybackStatus != RM_PSM_Prebuffering))
		return RM_OK;

	if (pSendContext->audioTransferPending)
		goto send_data;

	if (pSendContext->play_opt->disk_ctrl_state == DISK_CONTROL_STATE_SLEEPING)
		RMDBGLOG((ENABLE, "trying to read from a suspended drive!!\n"));

	pSendContext->audiosample.buf = pSendContext->audioDMABuffer + pSendContext->audioDMABufferOffset;
	
	pSendContext->audiosample.size = (1<<pSendContext->dmaBufferSizeLog2) - pSendContext->audioDMABufferOffset;

	if (pSendContext->isAACaudioPayload) {
		buf = pSendContext->audiosample.buf;
		
		pSendContext->audiosample.size -= ADTS_HEADER_SIZE;
		pSendContext->audiosample.buf += ADTS_HEADER_SIZE;
	}
	
	
	if (!RMGetNextMKVSample(pSendContext->mkvtA, &(pSendContext->audiosample), pSendContext->audiosample.size)) {
		RMDBGLOG((ENABLE,"Failed to get Audio sample -> end of audio stream\n"));
		
		pSendContext->audioEOS = TRUE;

		return RM_EOS;
	}
	
	if (!pSendContext->audiosample.size)
		RMDBGLOG((DISABLE, ">>> got an audio sample of size 0!\n"));
	
	RMGetMKVTrackNextSampleSize(pSendContext->mkvtA, &nextSampleSize);
	
	RMDBGLOG((DISABLE, "got audio sample of size %ld, nextSampleSize %lu, last audioDataSent %lu\n", pSendContext->audiosample.size, nextSampleSize, pSendContext->audioDataSent));
	
	if (pSendContext->isAACaudioPayload) {
		FillAdtsHeader(buf, pSendContext->audiosample.size, pSendContext->AAC_SampleRateFromDSI,
                       pSendContext->AAC_ChannelsFromDSI, pSendContext->AAC_ObjectIDFromDSI);
		
		pSendContext->audiosample.buf = buf;
		pSendContext->audiosample.size += ADTS_HEADER_SIZE;
	}
	
	
	if (pSendContext->audiosample.flags & MKV_CTS_VALID) {
		pSendContext->audio_Info.ValidFields = TIME_STAMP_INFO;
		pts = RMuint64from2RMuint32(pSendContext->audiosample.CTS_MSB, pSendContext->audiosample.CTS_LSB);
		pSendContext->audio_Info.TimeStamp = pts;
		
		if (pSendContext->audio_vop_tir != pSendContext->AudioCTSTimeScale) {
			pSendContext->audio_Info.TimeStamp = round_int_div(pts * pSendContext->audio_vop_tir, pSendContext->AudioCTSTimeScale);
			RMDBGLOG((DISABLE, "audio pts scaling! old %lld => %lld thru a factor %ld/%ld\n",
                      pts,
                      pSendContext->audio_Info.TimeStamp,
                      pSendContext->audio_vop_tir,
                      pSendContext->AudioCTSTimeScale));
		} 
		else
			RMDBGLOG((DISABLE, "setting pts to %llu\n", pts));
		
		pSendContext->lastAudioPTS = (1000 * pSendContext->audio_Info.TimeStamp) / pSendContext->audio_vop_tir;
	}
	
	if (!pSendContext->SendAudioPts) {
		pSendContext->audio_Info.TimeStamp = 0;
		pSendContext->audio_Info.ValidFields = 0;
	}
	
	// happens only once
	if (!pSendContext->audio_ptsScalingMsg) {
		if (pSendContext->audio_vop_tir != pSendContext->AudioCTSTimeScale) {
			RMDBGLOG((ENABLE, ">>> audio pts scaling! thru a factor %ld/%ld\n",
                      pSendContext->audio_vop_tir,
                      pSendContext->AudioCTSTimeScale));
		}
		else
			RMDBGLOG((ENABLE, ">>> no audio pts scaling required\n"));
		pSendContext->audio_ptsScalingMsg = TRUE;
	}
	
	
	if (pSendContext->FirstSystemTimeStamp && (pSendContext->audio_Info.ValidFields & TIME_STAMP_INFO)) {
		RMuint64 dummyPTS = pSendContext->audio_Info.TimeStamp;
			
		RMDBGLOG((ENABLE, "FirstSystemTimeStamp from audio = %lld(0x%llX) at %ld/sec = %llu s\n",
                  dummyPTS,
                  dummyPTS,
                  pSendContext->audio_vop_tir,
                  dummyPTS / pSendContext->audio_vop_tir));
			
		pSendContext->FirstSystemTimeStamp = FALSE;
			
		RMDBGLOG((ENABLE, "setting timers\n"));
		DCCSTCSetTime(pSendContext->dcc_info->pStcSource, dummyPTS+pSendContext->stc_offset_ms*((RMint64)(pSendContext->audio_vop_tir/1000)), pSendContext->audio_vop_tir);
#if PLAY_TIMERS
		if ((PlaybackStatus != RM_PSM_Prebuffering) && (PlaybackStatus != RM_PSM_Paused) && (PlaybackStatus != RM_PSM_Stopped))
			DCCSTCPlay(pSendContext->dcc_info->pStcSource);
#endif
			
			
	} 
	else if (pSendContext->FirstSystemTimeStamp) {
		if (pSendContext->forceTimersToZero) {
			RMDBGLOG((ENABLE, ">>> no pts, force timers to zero\n"));
			DCCSTCSetTime(pSendContext->dcc_info->pStcSource, pSendContext->stc_offset_ms*((RMint64)(pSendContext->audio_vop_tir/1000)), pSendContext->audio_vop_tir);
#if PLAY_TIMERS
			if ((PlaybackStatus != RM_PSM_Prebuffering) && (PlaybackStatus != RM_PSM_Paused) && (PlaybackStatus != RM_PSM_Stopped))
				DCCSTCPlay(pSendContext->dcc_info->pStcSource);
#endif
			pSendContext->FirstSystemTimeStamp = FALSE;
		}
		else {
			RMDBGLOG((ENABLE, "waiting for first time stamp to be detected, timer not set!!!, audio skipped\n"));
				
			return RM_OK;
		}
	}
		
	RMDBGLOG((DISABLE, "about to send a packet audio dma 0x%08X, sample 0x%08X, size %ld, pts %lld(0x%llx) %s\n", 
              pSendContext->pDMA, 
              pSendContext->audiosample.buf, 
              pSendContext->audiosample.size, 
              pSendContext->audio_Info.TimeStamp,
              pSendContext->audio_Info.TimeStamp,
              (pSendContext->audio_Info.ValidFields & TIME_STAMP_INFO) ? "valid":""));
		
	pSendContext->audioDMABufferOffset += pSendContext->audiosample.size;

 send_data:

	if (pSendContext->audiosample.size != 0) {
		
		if (pSendContext->SendAudioData) {
			RMint32 lastOKinstance;

			status = DCCMultipleAudioSendData(pSendContext->dcc_info->pMultipleAudioSource, 
                                              pSendContext->pDMA, 
                                              pSendContext->audiosample.buf, 
                                              pSendContext->audiosample.size, 
                                              &pSendContext->audio_Info, 
                                              sizeof(pSendContext->audio_Info), 
                                              &lastOKinstance);
			
			if (status != RM_OK) {
				if (status == RM_PENDING) {
					RMDBGLOG((DISABLE, "audio transfer pending\n"));
					pSendContext->audioTransferPending = TRUE;
				}
				return status;
			}
			pSendContext->audioTransferPending = FALSE;
		}
		
				
		if (pSendContext->SendAudioData) {
			RMDBGLOG((SENDDBG, "sent a packet audio dma 0x%08X, sample 0x%08X, size %ld, total %ld, pts %lld(0x%llx) %s\n", 
                      pSendContext->pDMA, 
                      pSendContext->audiosample.buf, 
                      pSendContext->audiosample.size,
                      pSendContext->audioDataSent,
                      pSendContext->audio_Info.TimeStamp,
                      pSendContext->audio_Info.TimeStamp,
                      (pSendContext->audio_Info.ValidFields & TIME_STAMP_INFO) ? "valid":""));
		}	
		// required for prebuffering routine
		pSendContext->audioDataSent += pSendContext->audiosample.size;
		
		
	}
	else {
		RMDBGLOG((DISABLE, "got audio sample of size 0!\n"));
	}
	
	
	return RM_OK;
}	



static RMstatus send_video_payload(struct SendMKVDataContext *pSendContext)
{
	RMstatus status;
	RMuint32 sizeBuffer = (1 << pSendContext->dmaBufferSizeLog2);
	RMmkvSample *sample = &(pSendContext->videosample);

	RMuint8 *videoDMAStartCodeBuffer;

	
	if (0 /*pSendContext->isH264*/) {
		// h264 streams packed into MKV system files require startcode reinsertion.
		
		RMDBGLOG((H264_SC_DBG, "send_video_payload (size %lu)\n", sample->size));

		if (sample->size < 3)
			RMDBGLOG((ENABLE, "warning read a sampleSize < 3\n"));
		else {
			RMuint32 nextSampleSize;

			RMGetMKVTrackNextSampleSize(pSendContext->mkvtV, &nextSampleSize);
		
			
			RMDBGLOG((H264_SC_DBG, "%02x %02x %02x %02x this sample size %6lu next %6lu flags %2lu\n", 
				  sample->buf[0],
				  sample->buf[1],
				  sample->buf[2],
				  sample->buf[3],
				  sample->size,
				  nextSampleSize,
				  sample->flags));
		}
			
		if (sample->flags & MKV_AU_START) {
			
			pSendContext->h264sendStartCode = TRUE;
			pSendContext->h264readPacketSize = TRUE;

			/* 
			   clear the flag because if sending part of the sample returns pending, we'll get called again
			   and we shouldn't send the startcode again
			*/
			sample->flags &= ~MKV_AU_START;
		}
		
		while (sample->size) {
			RMuint32 sizeToSend;

			RMDBGLOG((H264_SC_DBG, "available buffer count: %lu\n", RUAGetAvailableBufferCount(pSendContext->pDMA)));
			
			// send startcode
			
			if (pSendContext->h264sendStartCode) {
				// get a DMA buffer
				if (!pSendContext->startCodeDMABuffer) {
					get_buffer(pSendContext, &(pSendContext->startCodeDMABuffer));
					if (pSendContext->processKeyResultValid) {
						RMDBGLOG((ENABLE, "a key was pressed during H264 startcode buffer wait"));
						// a key was pressed, return to main_loop to process it
						return RM_PENDING;
					}
					pSendContext->startCodeDMABufferOffset = 0;
					RMDBGLOG((DBG, "setting new buffer for video startcodes 0x%lx\n", (RMuint32)pSendContext->startCodeDMABuffer));
				}

				videoDMAStartCodeBuffer = pSendContext->startCodeDMABuffer + pSendContext->startCodeDMABufferOffset;

				pSendContext->videoTransferPending = FALSE;
				
				// set StartCode (4bytes)
				RMuint32ToBeBuf(0x00000001, videoDMAStartCodeBuffer);
				
				if (pSendContext->SendVideoData) {
					// send startcode with PTS from the sample
					while (1) {
						status = RUASendData(pSendContext->pRUA, 
								     pSendContext->dcc_info->video_decoder, 
								     pSendContext->pDMA, 
								     videoDMAStartCodeBuffer, 
								     4, 
								     &pSendContext->video_Info, 
								     sizeof(pSendContext->video_Info));

						if (status == RM_OK) 
							break;
						/* handle end of prebufferisation here */
						else if (status == RM_PENDING) {
							struct RUAEvent e;
							
							check_prebuf_state(pSendContext, 0);
							
							e.ModuleID = pSendContext->dcc_info->video_decoder;
							e.Mask = RUAEVENT_XFER_FIFO_READY;
							RUAWaitForMultipleEvents(pSendContext->pRUA, &e, 1, COMMON_TIMEOUT_US, NULL);
							RMDBGLOG((ENABLE, "waiting to send video startcode\n"));
						}
						else {
							RMDBGLOG((ENABLE, "waiting to send video startcode\n"));
							return status;
						}
					}
				}
								
				pSendContext->h264sendStartCode = FALSE;
				
				// required for prebuffering routine
				pSendContext->videoDataSent += 4;

				pSendContext->videoBitrateCounter += 4 * 8;

				pSendContext->startCodeDMABufferOffset += 4;

				if (pSendContext->startCodeDMABufferOffset + DMABUFFER_UNUSED_BLOCK_SIZE >= sizeBuffer) {
					RMDBGLOG((DBG, "release buffer 0x%lx startcode %lu\n", (RMuint32)pSendContext->startCodeDMABuffer, pSendContext->startCodeDMABufferOffset));
					RUAReleaseBuffer(pSendContext->pDMA, pSendContext->startCodeDMABuffer);
					pSendContext->startCodeDMABuffer = NULL;
					pSendContext->startCodeDMABufferOffset = 0;
				}

				RMDBGLOG((H264_SC_DBG, "sent startcode\n"));

				if (pSendContext->SendVideoData) {
					RMDBGLOG((SENDDBG, "sent H264 video startcode : frameCount %lu totalBytes %lu pts %lld ( 0x%llx ) %s\n",
						  pSendContext->videoFrameCount,
						  pSendContext->videoDataSent,
						  pSendContext->video_Info.TimeStamp,
						  pSendContext->video_Info.TimeStamp,
						  (pSendContext->video_Info.ValidFields & TIME_STAMP_INFO) ? "valid":""));
					
				}

				// dont send pts with subsequent packets
				pSendContext->video_Info.ValidFields = 0;
				pSendContext->video_Info.TimeStamp = 0;

			}
		
			// read avcPkt size
			if (pSendContext->h264readPacketSize) {
				
				while (sample->size) {
					*(pSendContext->h264PktLenBuf) = *(sample->buf);
					
					pSendContext->h264PktLenBuf++;
					
					sample->size--;
					sample->buf++;
					
					pSendContext->h264LengthSize--;
					if (!pSendContext->h264LengthSize)
						break;
				}
				
				if (!pSendContext->h264LengthSize) {
					pSendContext->h264readPacketSize = FALSE;
					
					RMGetMKVH264LengthSize(pSendContext->mkvtV, &(pSendContext->h264LengthSize));
					
					pSendContext->h264PktLenBuf -= pSendContext->h264LengthSize;
					
					switch (pSendContext->h264LengthSize) {
					case 2:
						pSendContext->h264PktLen = RMbeBufToUint16(pSendContext->h264PktLenBuf);
						break;
					case 3:
						pSendContext->h264PktLen = RMbeBufToUint24(pSendContext->h264PktLenBuf);
						break;
					case 4:
						pSendContext->h264PktLen = RMbeBufToUint32(pSendContext->h264PktLenBuf);
						break;
					default:
						RMDBGLOG((ENABLE, "ERROR! unhandled h264LengthSize %lu\n", pSendContext->h264LengthSize));
						return RM_ERROR;
					}

					RMDBGLOG((H264_SC_DBG, "read h264 packet size %lu (0x%lx)\n", pSendContext->h264PktLen, pSendContext->h264PktLen));
					pSendContext->h264BytesLeft = pSendContext->h264PktLen;
				}

				if (!sample->size) {
					RMDBGLOG((ENABLE, "get new sample\n"));
					return RM_OK; 
				}
			}
		
		
			// send the rest of the buffer

			sizeToSend = RMmin(pSendContext->h264BytesLeft, sample->size);
		
			if (pSendContext->SendVideoData) {
				status = RUASendData(pSendContext->pRUA, 
						     pSendContext->dcc_info->video_decoder, 
						     pSendContext->pDMA, 
						     sample->buf, 
						     sizeToSend, 
						     &pSendContext->video_Info, 
						     sizeof(pSendContext->video_Info));
				
				if (status != RM_OK) {
					if (status == RM_PENDING) {
						RMDBGLOG((DISABLE, "video transfer pending\n"));
						pSendContext->videoTransferPending = TRUE;
					}
					return status;
				}
				pSendContext->videoTransferPending = FALSE;
			}
			
			pSendContext->h264BytesLeft -= sizeToSend;

			sample->size -= sizeToSend;
			sample->buf  += sizeToSend;
			
			// required for prebuffering routine
			pSendContext->videoDataSent += sizeToSend;

			pSendContext->videoBitrateCounter += sizeToSend * 8;


			RMDBGLOG((H264_SC_DBG, "send payload (%5lu/%5lu), sampleSize %5lu, bytesLeft %5lu\n",
				  sizeToSend,
				  pSendContext->h264PktLen,
				  sample->size,
				  pSendContext->h264BytesLeft));
			
			if (!pSendContext->h264BytesLeft) {
				pSendContext->h264sendStartCode = TRUE;
				pSendContext->h264readPacketSize = TRUE;
			}

			if (pSendContext->SendVideoData) {
				RMDBGLOG((SENDDBG, "sent a video packet (flags %lu) frameCount %lu size %ld / %lu (total %lu ) pts %lld ( 0x%llx ) %s\n", 
					  pSendContext->videosample.flags,
					  pSendContext->videoFrameCount,
					  sizeToSend,
					  pSendContext->videoSampleSize, 
					  pSendContext->videoDataSent,
					  pSendContext->video_Info.TimeStamp,
					  pSendContext->video_Info.TimeStamp,
					  (pSendContext->video_Info.ValidFields & TIME_STAMP_INFO) ? "valid":""));
				
			}

		}

		return RM_OK;
	}
	// else, just send the packets coming from the MKV parser


	if (pSendContext->SendVideoData) {
		status = RUASendData(pSendContext->pRUA, 
				     pSendContext->dcc_info->video_decoder, 
				     pSendContext->pDMA, 
				     pSendContext->videosample.buf, 
				     pSendContext->videosample.size, 
				     &pSendContext->video_Info, 
				     sizeof(pSendContext->video_Info));
		
		if (status != RM_OK) {
			if (status == RM_PENDING) {
				RMDBGLOG((ENABLE, "video transfer pending\n"));
				pSendContext->videoTransferPending = TRUE;
			}
			return status;
		}
		pSendContext->videoTransferPending = FALSE;
	}
	

	{
		RMuint8 picType = 0xFF;
		// check MPEG4 picture type, 0=I;1=P;2=B;3=S
		if ((pSendContext->videosample.size > 5) && (!pSendContext->isH264)) {
			RMuint8 *dummy = pSendContext->videosample.buf;
			RMuint8 byte;
			if ((dummy[0] == 0) &&
			    (dummy[1] == 0) &&
			    (dummy[2] == 1) &&
			    (dummy[3] == 0xb6)) {
				byte = dummy[4];
				byte &= 0xC0;
				byte >>= 6;
				picType = byte;
			}
		}
		
		if (pSendContext->SendVideoData) {
			RMDBGLOG((DISABLE, "sent a packet video %s dma 0x%08X, sample 0x%08X, size %ld, total %lu, pts %lld(0x%llx) %s\n", 
				  (picType == 0) ? "(picType: I)": ((picType == 1) ? "(picType: P)": ((picType == 2) ? "(picType: B)":"")),
				  pSendContext->pDMA, 
				  pSendContext->videosample.buf, 
				  pSendContext->videosample.size, 
				  pSendContext->videoDataSent,
				  pSendContext->video_Info.TimeStamp,
				  pSendContext->video_Info.TimeStamp,
				  (pSendContext->video_Info.ValidFields & TIME_STAMP_INFO) ? "valid":""));
			
		}
	}
	
	// required for prebuffering routine
	pSendContext->videoDataSent += pSendContext->videosample.size;

	pSendContext->videoBitrateCounter += pSendContext->videosample.size * 8;

	return RM_OK;
}



static RMstatus send_MKV_video(struct SendMKVDataContext * pSendContext)
{
	RMstatus status;
	RMuint64 pts;
	RMbool notEOS = TRUE;
	//enum RM_PSM_State PlaybackStatus = RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info));
	
	if (pSendContext->videoTransferPending)
		goto send_data;

	pSendContext->videosample.buf = pSendContext->videoDMABuffer + pSendContext->videoDMABufferOffset;
	
	pSendContext->videosample.size = (1<<pSendContext->dmaBufferSizeLog2) - pSendContext->videoDMABufferOffset;
	
	if (pSendContext->play_opt->disk_ctrl_state == DISK_CONTROL_STATE_SLEEPING)
		RMDBGLOG((ENABLE, "trying to read from a suspended drive!!\n"));

	if (pSendContext->isIFrameMode && pSendContext->IFrameDirection > 0) {
		notEOS = RMGetNextMKVRandomAccessSample(pSendContext->mkvtV, &(pSendContext->videosample), pSendContext->videosample.size);
	}
	else if (pSendContext->isIFrameMode && pSendContext->IFrameDirection < 0) {
		notEOS = RMGetPrevMKVRandomAccessSample(pSendContext->mkvtV, &(pSendContext->videosample), pSendContext->videosample.size);
	}
	else
		notEOS = RMGetNextMKVSample(pSendContext->mkvtV, &(pSendContext->videosample), pSendContext->videosample.size);
	
	if (!notEOS) {
		RMDBGLOG((ENABLE,"Failed to get Video sample -> end of video stream\n"));
		pSendContext->videoEOS = TRUE;
		return RM_EOS;
	}
	
	RMDBGLOG((DISABLE, "got video sample of size %ld\n", pSendContext->videosample.size));

	pSendContext->videoSampleSize = pSendContext->videosample.size;

	pSendContext->video_Info.TimeStamp = 0;
	pSendContext->video_Info.ValidFields = 0;

	if (pSendContext->videosample.flags & MKV_AU_START) {
		pSendContext->videoFrameCount++;
	}

	//RMDBGLOG((ENABLE, "[%lu] sample size %lu flags %lu\n", pSendContext->videoFrameCount, pSendContext->videoSampleSize, pSendContext->videosample.flags));

	if (pSendContext->videosample.flags & MKV_CTS_VALID) {
			
		pSendContext->video_Info.ValidFields = TIME_STAMP_INFO;
			
		if (pSendContext->videosample.DTS_MSB != pSendContext->videosample.CTS_MSB) {
			RMuint64 dts;
			RMuint64 cts;
			RMint32 diff = 0;

			//  this is a hack for some h264 mov files which contain "invalid" pts
			//  bug #5606
			  	
			// some debug:
			dts = (RMuint64)pSendContext->videosample.DTS_MSB;
			dts <<= 32;
			dts |= pSendContext->videosample.DTS_LSB;
			
			cts = (RMuint64)pSendContext->videosample.CTS_MSB;
			cts <<= 32;
			cts |= pSendContext->videosample.CTS_LSB;
			
			RMDBGLOG((DISABLE, "invalid pts from CTS. DTSh %08lx DTSl %08lx = %09llx CTSh %08lx CTSl %08lx = %09llx lastPTS %llx scale %lu\n",
                      pSendContext->videosample.DTS_MSB,
                      pSendContext->videosample.DTS_LSB,
                      dts,
                      pSendContext->videosample.CTS_MSB,
                      pSendContext->videosample.CTS_LSB,
                      cts,
                      pSendContext->lastVideoPTS,
                      pSendContext->video_vop_tir));
			
				
			/*
			  the hack: 
			  if following condition is satisfied, compute a pts from emulated CTS
			  where emulated CTS = DTS + diff
			  and diff = CTS_LSB - DTS_LSB (just to make sure delay between DTS and CTS is respected in final pts
			*/
			
			if ((pSendContext->videosample.DTS_MSB == 0) && (pSendContext->videosample.CTS_MSB == 1))
				diff = pSendContext->videosample.CTS_LSB - pSendContext->videosample.DTS_LSB;

			
			// emulated cts
			cts = (RMuint64)pSendContext->videosample.DTS_MSB;
			cts <<= 32;
			cts |= pSendContext->videosample.DTS_LSB  + diff ;
			RMDBGLOG((DISABLE, "send pts based on emulated CTS: DTSh %08lx DTSl %08lx ->  0x%09llx\n",
                      pSendContext->videosample.DTS_MSB,
                      pSendContext->videosample.DTS_LSB + diff,
                      cts));

			pts = RMuint64from2RMuint32(pSendContext->videosample.DTS_MSB, pSendContext->videosample.DTS_LSB + diff);
		} else
			pts = RMuint64from2RMuint32(pSendContext->videosample.CTS_MSB, pSendContext->videosample.CTS_LSB);

		pSendContext->video_Info.TimeStamp = pts;

        if (pSendContext->video_vop_tir != pSendContext->VideoCTSTimeScale) {
            pSendContext->video_Info.TimeStamp = round_int_div(pts * pSendContext->video_vop_tir, pSendContext->VideoCTSTimeScale);
            RMDBGLOG((DISABLE, "video pts scaling! old %lld => %lld thru a factor %ld/%ld\n",
                      pts,
                      pSendContext->video_Info.TimeStamp,
                      pSendContext->video_vop_tir,
                      pSendContext->VideoCTSTimeScale));
        } 
        else
            RMDBGLOG((DISABLE, "setting pts to %llu\n", pts));
			
	
		if (pSendContext->video_Info.TimeStamp) {
			RMint64 oldPTS, newPTS, diff, videoBitrate;
			
			newPTS = (RMint64)((1000 * pSendContext->video_Info.TimeStamp) / pSendContext->video_vop_tir);
			oldPTS = (RMint64)pSendContext->lastVideoPTS;

			diff = newPTS - oldPTS;
			if (diff < 0)
				diff *= -1;
			
			if (diff) {
				videoBitrate = (pSendContext->videoBitrateCounter * 1000) / diff;
				
				pSendContext->meanVideoBitrate += videoBitrate;
				pSendContext->meanCounter++;
				
				pSendContext->pseudoInstantaneousVideoBitrate = videoBitrate;
				
				if (pSendContext->maxPseudoInstantaneousVideoBitrate < videoBitrate)
					pSendContext->maxPseudoInstantaneousVideoBitrate = videoBitrate;

				pSendContext->videoBitrateCounter = 0;
			}

			pSendContext->lastVideoPTS = newPTS;

		}
	}

	if (!pSendContext->SendVideoPts) {
		pSendContext->video_Info.TimeStamp = 0;
		pSendContext->video_Info.ValidFields = 0;
	}

	RMDBGLOG((DISABLE, "[%lu] sample size %lu flags %lu pts %lld valid %lu\n", 
              pSendContext->videoFrameCount, 
              pSendContext->videoSampleSize, 
              pSendContext->videosample.flags,
              pSendContext->video_Info.TimeStamp,
              pSendContext->video_Info.ValidFields));

	// happens only once
	if (!pSendContext->video_ptsScalingMsg) {
		if (pSendContext->video_vop_tir != pSendContext->VideoCTSTimeScale) {
			RMDBGLOG((ENABLE, ">>>video pts scaling! thru a factor %ld/%ld\n",
                      pSendContext->video_vop_tir,
                      pSendContext->VideoCTSTimeScale));
		}
		else
			RMDBGLOG((ENABLE, ">>> no video pts scaling required\n"));
		pSendContext->video_ptsScalingMsg = TRUE;
	}
		
   
	if (pSendContext->FirstSystemTimeStamp) {
        enum RM_PSM_State PlaybackStatus = RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info));
   
		if (pSendContext->video_Info.ValidFields & TIME_STAMP_INFO) {
			RMuint64 dummyPTS = pSendContext->video_Info.TimeStamp;
			RMDBGLOG((ENABLE, "FirstSystemTimeStamp from video = %lld (0x%llX) at %ld/sec = %llu s\n",
                      dummyPTS,
                      dummyPTS,
                      pSendContext->video_vop_tir,
                      dummyPTS / pSendContext->video_vop_tir));
				
			pSendContext->FirstSystemTimeStamp = FALSE;
			RMDBGLOG((ENABLE, "setting timers\n"));
			
            //DCCSTCSetTime(pSendContext->dcc_info->pStcSource, dummyPTS/1000, pSendContext->video_vop_tir);
            DCCSTCSetTime(pSendContext->dcc_info->pStcSource, dummyPTS+pSendContext->stc_offset_ms*((RMint64)(pSendContext->video_vop_tir/1000)), pSendContext->video_vop_tir);
#if PLAY_TIMERS
			if ((PlaybackStatus != RM_PSM_Prebuffering) && (PlaybackStatus != RM_PSM_Paused) && (PlaybackStatus != RM_PSM_Stopped))
				DCCSTCPlay(pSendContext->dcc_info->pStcSource);
#endif
        }
        else {
			if (pSendContext->forceTimersToZero) {
				RMDBGLOG((ENABLE, ">>> no pts, force timers to zero\n"));
				DCCSTCSetTime(pSendContext->dcc_info->pStcSource, pSendContext->stc_offset_ms*((RMint64)(pSendContext->video_vop_tir/1000)), pSendContext->video_vop_tir);
#if PLAY_TIMERS
				if ((PlaybackStatus != RM_PSM_Prebuffering) && (PlaybackStatus != RM_PSM_Paused) && (PlaybackStatus != RM_PSM_Stopped))
					DCCSTCPlay(pSendContext->dcc_info->pStcSource);
#endif
				pSendContext->FirstSystemTimeStamp = FALSE;
			}
			else {
				//RMDBGLOG((ENABLE, "waiting for first time stamp to be detected, timer not set!!!, video skipped\n"));
				return RM_OK;
			}
        }
	}
		
	RMDBGLOG((DISABLE, "about to send a packet video dma 0x%08X, sample 0x%08X, size %ld, pts %lld(0x%llx) %s\n", 
              pSendContext->pDMA, 
              pSendContext->videosample.buf, 
              pSendContext->videosample.size, 
              pSendContext->video_Info.TimeStamp,
              pSendContext->video_Info.TimeStamp,
              (pSendContext->video_Info.ValidFields & TIME_STAMP_INFO) ? "valid":""));

	pSendContext->videoDMABufferOffset += pSendContext->videosample.size;

 send_data:

	if (pSendContext->videosample.size != 0) {

		status = send_video_payload(pSendContext);
		if (status != RM_OK)
			return status;
	}
	else {
		RMDBGLOG((ENABLE, "got video sample of size 0!\n"));
	}
	
	
	return RM_OK;
}

static RMstatus mkv_seek(struct SendMKVDataContext * pSendContext, RMuint64 time_ms)
{

	
	RMuint32 seeked_time;
	RMuint32 seekTo;

	seekTo = (RMuint32) time_ms;
	
	RMDBGLOG((ENABLE, "Should seek to %ld ms\n", seekTo));	
	//seekTo *= 1000;

	Stop(pSendContext, RM_DEVICES_STC | RM_DEVICES_AUDIO | RM_DEVICES_VIDEO);

	DCCSTCSetSpeed(pSendContext->dcc_info->pStcSource, pSendContext->play_opt->speed_N, pSendContext->play_opt->speed_M);
 
	seeked_time = RMSeekMKVmillisec(pSendContext->mkvc, seekTo);
	seeked_time /= 1000;
	RMDBGLOG((ENABLE, "Seeked to %ld s\n", seeked_time));

	pSendContext->FirstSystemTimeStamp = TRUE;

	RM_PSM_SetState(pSendContext->PSMcontext, &(pSendContext->dcc_info), RM_PSM_Playing);

	/* the stc will be put into "play" mode with the first pts */
	Play(pSendContext, RM_DEVICES_AUDIO | RM_DEVICES_VIDEO, DCCVideoPlayFwd);

	pSendContext->lastVideoPTS = 0;
	pSendContext->lastAudioPTS = 0;

	pSendContext->videoEOS = FALSE;
	pSendContext->audioEOS = FALSE;

	pSendContext->videoTransferPending = FALSE;
	pSendContext->audioTransferPending = FALSE;

	pSendContext->prebufferingDone = FALSE;
	return RM_OK;

}

static RMstatus setup_audio_decoder(struct SendMKVDataContext *pSendContext)
{
	RMstatus err;
	struct dcc_context *dcc_info = pSendContext->dcc_info;
	struct DCCAudioProfile audio_profiles[MAX_AUDIO_DECODER_INSTANCES];
	RMuint32 i;

	RMDBGLOG((ENABLE, "******** setup_audio_decoder (%lu instances)\n", pSendContext->audioInstances));

	for (i = 0; i < pSendContext->audioInstances; i++) {
		audio_profiles[i].BitstreamFIFOSize = pSendContext->audioBitstreamFIFOSize[i];
		audio_profiles[i].XferFIFOCount = pSendContext->audioFIFOXFERCount[i];
		audio_profiles[i].DemuxProgramID = pSendContext->audio_opt[i].AudioEngineID * 2;
		audio_profiles[i].AudioEngineID = pSendContext->audio_opt[i].AudioEngineID;
		audio_profiles[i].AudioDecoderID = pSendContext->audio_opt[i].AudioDecoderID;
		audio_profiles[i].STCID = pSendContext->play_opt->STCid;
	}
	
	err = DCCOpenMultipleAudioDecoderSource(dcc_info->pDCC, audio_profiles, pSendContext->audioInstances, &(dcc_info->pMultipleAudioSource));
	
	if (RMFAILED(err)) {
		fprintf(stderr, "Cannot open multiple audio decoder %d\n", err);
		return err;
	}
	
	for (i = 0; i < pSendContext->audioInstances; i++) {

		/* Set the right sample rate if not overriden */
		if (!pSendContext->audio_opt[i].ForceSampleRate){
			pSendContext->audio_opt[i].SampleRate = pSendContext->audioSampleRate;
			RMDBGLOG((ENABLE, ">>> sample rate: %ld\n", pSendContext->audioSampleRate));
		} 
		else
			RMDBGLOG((ENABLE, ">>> sample rate specified on cmdline %ld, overriding %ld from DSI\n", pSendContext->audio_opt[i].SampleRate, pSendContext->audioSampleRate)); 
		
		
		if (!pSendContext->audio_opt[i].SampleRate) {
			RMDBGLOG((ENABLE, ">>> sample rate = 0, defaulting to 44100\n"));
			pSendContext->audio_opt[i].SampleRate = 44100;
		}
		
		// apply the sample rate, serial out status
	
		RMDBGLOG((ENABLE, "apply audio engine options for instance %lu\n", i));
		err = apply_audio_engine_options(dcc_info, &(pSendContext->audio_opt[i]));
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Error applying audio engine options %d\n", err));
			return err;
		}
	
		// apply the audio format - uninit, set codec, set specific parameters, init
		RMDBGLOG((ENABLE, "apply audio decoder options for instance %lu\n", i));
		err = apply_audio_decoder_options(dcc_info, &(pSendContext->audio_opt[i]));
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Error applying audio_decoder_options %d\n", err));
			return err;
		}
	}	

#if 1
	RMDBGLOG((ENABLE, "apply dvi/hdmi audio options\n"));
	// TODO determine NumChannel from options->Codec and options->SubCodec
	apply_dvi_hdmi_audio_options(dcc_info, &(pSendContext->audio_opt[0]), 0, FALSE, FALSE, FALSE);
#ifndef WITH_MONO
	if (! pSendContext->disp_opt->configure_outports) apply_hdcp(dcc_info, pSendContext->disp_opt);
#endif
#endif
	
	return RM_OK;
	
}


static RMstatus SwitchAudio(struct SendMKVDataContext *pSendContext)
{
	RMstatus status;
	RMuint64 time;
	RMuint32 seeked_time;

	RMDBGLOG((ENABLE, "++ got audio stream change command\n"));

	if ((pSendContext->dcc_info->selectAudioStream > (RMint32)pSendContext->audioTracks) || 
	    (pSendContext->dcc_info->selectAudioStream == 0) ||
	    (pSendContext->audioTracks <= 1) || 
	    (pSendContext->dcc_info->selectAudioStream == (RMint32)pSendContext->currentAudioTrack)) {
		RMDBGLOG((ENABLE, "audio stream change ignored (total audioTracks %lu, selected %ld, current %lu)\n", 
			  pSendContext->audioTracks, 
			  pSendContext->dcc_info->selectAudioStream,
			  pSendContext->currentAudioTrack));
		return RM_ERROR;
	}

	if (!pSendContext->isAudioOnly) {
		status = DCCSTCGetTime(pSendContext->dcc_info->pStcSource, &time, pSendContext->video_vop_tir);
		RMDBGLOG((ENABLE, "switchAudio at %llu in %ld units\n", time, pSendContext->video_vop_tir));
		time = round_int_div(time, pSendContext->video_vop_tir);
	}
	else {
		status = DCCSTCGetTime(pSendContext->dcc_info->pStcSource, &time, pSendContext->audio_vop_tir);
		RMDBGLOG((ENABLE, "switchAudio at %llu in %ld units\n", time, pSendContext->audio_vop_tir));
		time = round_int_div(time, pSendContext->audio_vop_tir);
	}

	
	
	RMDBGLOG((ENABLE, "total audio streams %lu\n", pSendContext->audioTracks));
	if (pSendContext->dcc_info->selectAudioStream == -1) {
		RMuint32 stream = pSendContext->currentAudioTrack;		

		stream++;
		if (stream > pSendContext->audioTracks)
			stream = 1;

		status = mkv_select_audio_track(pSendContext, stream);
		if (status != RM_OK) {
			RMDBGLOG((ENABLE, "couldn't switch, ignore audio switch\n"));
			return status;
		}

		RMDBGLOG((ENABLE, "current stream %lu, switch to %lu\n", pSendContext->currentAudioTrack, stream));
		pSendContext->currentAudioTrack = stream;

	}
	else {
		status = mkv_select_audio_track(pSendContext, pSendContext->dcc_info->selectAudioStream);
		if (status != RM_OK) {
			RMDBGLOG((ENABLE, "couldn't switch, ignore audio switch\n"));
			return status;
		}
		
		RMDBGLOG((ENABLE, "current stream %lu, switch to %lu\n", pSendContext->currentAudioTrack, pSendContext->dcc_info->selectAudioStream));
		pSendContext->currentAudioTrack = pSendContext->dcc_info->selectAudioStream;

	}
	
	Stop(pSendContext, RM_DEVICES_AUDIO);

	RMDBGLOG((ENABLE, "++ closing audio source\n"));
	status = DCCCloseMultipleAudioSource(pSendContext->dcc_info->pMultipleAudioSource);
	if (RMFAILED(status)) {
		RMDBGLOG((ENABLE, "Error cannot close audio decoder %d\n", status));
		return status;
	}

	RMDBGLOG((ENABLE, "++ setup_audio_decoder\n"));
	setup_audio_decoder(pSendContext);
	//DCCSTCSetTimeResolution(pSendContext->dcc_info->pStcSource, DCC_Audio, pSendContext->AudioCTSTimeScale);

	if (RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info)) != RM_PSM_Stopped)
		Play(pSendContext, RM_DEVICES_AUDIO, (enum DCCVideoPlayCommand)0);

	seeked_time = RMSeekMKVTrack(pSendContext->mkvc, time*1000, pSendContext->mkvtA);
	RMDBGLOG((ENABLE, "time: %llu, seeked audio track to %lu\n", time, seeked_time));

	pSendContext->lastAudioPTS = 0;
	pSendContext->audioEOS = FALSE;

#if 0
	// turn on discs, required to start buffering data right away
	switch (pSendContext->play_opt->disk_ctrl_state) {
	case DISK_CONTROL_STATE_DISABLE:
	case DISK_CONTROL_STATE_RUNNING:
		break;
	case DISK_CONTROL_STATE_SLEEPING:
		if(pSendContext->play_opt->disk_ctrl_callback && pSendContext->play_opt->disk_ctrl_callback(DISK_CONTROL_ACTION_RUN) == RM_OK)
			pSendContext->play_opt->disk_ctrl_state = DISK_CONTROL_STATE_RUNNING;
	}
#endif

	return RM_OK;

}

static RMstatus SyncTimerWithDecoderPTS(struct SendMKVDataContext *pSendContext)
{
	RMuint64 videoPTS;
	RMuint64 CurrentSTC;
	RMstatus err = RM_OK;
	RMuint64 time = 0;

	
	
	if ((!pSendContext->isAudioOnly) && (pSendContext->dcc_info->pVideoSource)) {
		DCCSTCGetTime(pSendContext->dcc_info->pStcSource, &CurrentSTC, pSendContext->video_vop_tir);
		time = round_int_div(CurrentSTC, pSendContext->video_vop_tir);
		RMDBGLOG((ENABLE, "stc is %llu/%lu = %llu s\n", CurrentSTC, pSendContext->video_vop_tir, time));

		err = RUAGetProperty(pSendContext->pRUA, pSendContext->dcc_info->SurfaceID, RMGenericPropertyID_CurrentDisplayPTS, &videoPTS, sizeof(videoPTS));
		if (err != RM_OK) {
			RMDBGLOG((ENABLE, "error %d while getting CurrentDisplayPTS\n", err));
			return err;
		}

		// internally, the PTS are divided by 2, so we must compensate
		if (pSendContext->video_vop_tir == 90000)
			videoPTS *= 2;

		RMDBGLOG((ENABLE, ">> resync timer (%llu) with videoDecoder current PTS (%llu)\n", CurrentSTC, videoPTS));
		DCCSTCSetTime(pSendContext->dcc_info->pStcSource, videoPTS, pSendContext->video_vop_tir);

		return err;
	}
	else {
		DCCSTCGetTime(pSendContext->dcc_info->pStcSource, &CurrentSTC, pSendContext->audio_vop_tir);
		time = round_int_div(CurrentSTC, pSendContext->audio_vop_tir);
		RMDBGLOG((ENABLE, "stc is %llu/%lu = %llu s\n", CurrentSTC, pSendContext->audio_vop_tir, time));
		
		//err = mkv_seek(pSendContext, time);
		return err;
	}


}


static RMstatus DetectLength(struct SendMKVDataContext *pSendContext)
{
	if (!pSendContext->isAudioOnly) {
		RMDBGLOG((ENABLE, "detecting video length\n"));
	}
	else {
		RMDBGLOG((ENABLE, "detecting audio length (audio only file)\n"));
	}

    pSendContext->Duration = RMGetMKVTrackDuration(pSendContext->mkvtV);
    fprintf(stderr, "duration %llu secs\n", pSendContext->Duration/1000);

	return RM_OK;

}


static RMstatus mkv_IFrameTrickMode(struct SendMKVDataContext * pSendContext)
{

	
	RMstatus err = RM_OK;
	RMint32 direction = 1;
	struct dcc_context *dcc_info = pSendContext->dcc_info;
	RMuint64 time;
	RMuint32 seeked_time = 0;
	RMuint8 dummybuf[MAX_SAMPLE_SIZE];
	RMmkvSample dummysample;
	enum RM_PSM_State PlaybackStatus = RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info));

	dummysample.buf = dummybuf;
	dummysample.size = MAX_SAMPLE_SIZE;
	
	RMDBGLOG((ENABLE, "Init IFrame trickmode\n"));

	err = DCCSTCGetTime(dcc_info->pStcSource, &time, pSendContext->video_vop_tir);

	if (PlaybackStatus == RM_PSM_IForward)
		direction = 1;
	else if (PlaybackStatus == RM_PSM_IRewind)
		direction = -1;
	else {
		RMDBGLOG((ENABLE, "cant determine iframe trickmode playback direction, defaulting to forward!"));
		direction = 1;
	}

	pSendContext->IFrameDirection = direction;

 	/* Since we seek, we need to flush Frames from the video decoder */
 	Stop(pSendContext, RM_DEVICES_VIDEO);

	RMDBGLOG((ENABLE, "playing trickmode...direction %s\n", (direction >= 0) ? "forward":"backward"));

	RMDBGLOG((ENABLE, "current time %llu in %ld units\n", time, pSendContext->video_vop_tir));
	time = time / pSendContext->video_vop_tir;

	/* 
	   this "seeking" part is required to init the iframe tables in the mkv API so that the 
	   calls to GetNextRandom... work properly.
	*/

	RMDBGLOG((ENABLE, "seeking to %llu s\n", time));
	
	seeked_time = RMSeekMKVmillisec(pSendContext->mkvc, time*1000);
	seeked_time /= 1000;
	
	RMDBGLOG((ENABLE, "Seeked to %ld s\n", seeked_time));

	/* 
	   seeking always takes us to the nearest past iframe, 
	   so in forward mode, we have to skip ahead the one after the seek 
	*/
	if (direction > 0) {
		RMDBGLOG((ENABLE, "skipping one iframe\n"));
		RMGetNextMKVRandomAccessSample(pSendContext->mkvtV, &dummysample, dummysample.size);
	}
	else {
		RMDBGLOG((ENABLE, "skipping one iframe\n"));
		RMGetPrevMKVRandomAccessSample(pSendContext->mkvtV, &dummysample, dummysample.size);
	}

	pSendContext->FirstSystemTimeStamp = TRUE;
	
	pSendContext->lastVideoPTS = 0;
	pSendContext->lastAudioPTS = 0;
	pSendContext->videoEOS = FALSE;
	pSendContext->audioEOS = FALSE;

	Play(pSendContext, RM_DEVICES_VIDEO | RM_DEVICES_STC, DCCVideoPlayIFrame);

	//DCCSTCSetSpeed(dcc_info->pStcSource, play_opt->speed_N * direction, play_opt->speed_M);

	return RM_OK;

}


static RMstatus mkv_ResumeFromIFrameMode(struct SendMKVDataContext * pSendContext)
{
	RMstatus err = RM_OK;
	RMuint64 time;
	struct dcc_context *dcc_info = pSendContext->dcc_info;
	
	if (!pSendContext->isAudioOnly) {
		err = RUAGetProperty(pSendContext->pRUA, pSendContext->dcc_info->SurfaceID, RMGenericPropertyID_CurrentDisplayPTS, &time, sizeof(time));
		if (pSendContext->video_vop_tir == 90000)
			time *= 2;
		RMDBGLOG((ENABLE, "resume from trickmode at %llu in %ld units\n", time, pSendContext->video_vop_tir));
		time = round_int_div(time, pSendContext->video_vop_tir);
	}
	else {
		err = DCCSTCGetTime(dcc_info->pStcSource, &time, pSendContext->audio_vop_tir);
		RMDBGLOG((ENABLE, "resume from audio trickmode at %llu in %ld units\n", time, pSendContext->audio_vop_tir));
		time = round_int_div(time, pSendContext->audio_vop_tir);
	}
	
	RMDBGLOG((ENABLE, "seeking to %llu s\n", time));
		
	mkv_seek(pSendContext, time * 1000);

	pSendContext->lastVideoPTS = 0;
	pSendContext->lastAudioPTS = 0;
		
	return RM_OK;
}


static RMstatus mkv_ResumeFromTrickMode(struct SendMKVDataContext * pSendContext)
{
	RMstatus err = RM_OK;
	RMuint64 time;
	struct dcc_context *dcc_info = pSendContext->dcc_info;
	
	if (pSendContext->isAudioOnly) {
		
		err = DCCSTCGetTime(dcc_info->pStcSource, &time, pSendContext->audio_vop_tir);
		RMDBGLOG((ENABLE, "resume from audio trickmode at %llu in %ld units\n", time, pSendContext->audio_vop_tir));
		time = round_int_div(time, pSendContext->audio_vop_tir);

		RMDBGLOG((ENABLE, "seeking to %llu s\n", time));
		
		mkv_seek(pSendContext, time * 1000);

	}
	else {
		RMuint32 seeked_time;

		err = DCCSTCGetTime(dcc_info->pStcSource, &time, pSendContext->video_vop_tir);
		RMDBGLOG((ENABLE, "resume from trickmode at %llu in %ld units\n", time, pSendContext->video_vop_tir));
		time = round_int_div(time, pSendContext->video_vop_tir);

		RMDBGLOG((ENABLE, "syncing additional tracks\n"));

		if (pSendContext->mkvtA) {
			seeked_time = RMSeekMKVTrack(pSendContext->mkvc, time*1000, pSendContext->mkvtA);
			RMDBGLOG((ENABLE, "time: %llu, seeked audio track to %lu\n", time, seeked_time));
		}

	}
		
	pSendContext->lastVideoPTS = 0;
	pSendContext->lastAudioPTS = 0;

	pSendContext->videoEOS = FALSE;
	pSendContext->audioEOS = FALSE;

		
	return RM_OK;

}

static RMstatus mkv_ResumeFromStop(struct SendMKVDataContext * pSendContext)
{
	RMstatus err = RM_OK;
	RMuint64 time;
	struct dcc_context *dcc_info = pSendContext->dcc_info;

	if (!pSendContext->Duration) {
		fprintf(stderr, "seek is not allowed, seeking to beginning\n");
		mkv_seek(pSendContext, 0);
		return RM_OK;
	}
	
	if (pSendContext->isAudioOnly) {
		
		err = DCCSTCGetTime(dcc_info->pStcSource, &time, pSendContext->audio_vop_tir);
		RMDBGLOG((ENABLE, "resume from audio stop at %llu in %ld units\n", time, pSendContext->audio_vop_tir));
		time = round_int_div(time, pSendContext->audio_vop_tir);

		RMDBGLOG((ENABLE, "seeking to %llu s\n", time));
		
		mkv_seek(pSendContext, time * 1000);

	}
	else {
		RMuint32 seeked_time = 0;

		RMDBGLOG((ENABLE, "last audio PTS %llu last video PTS %llu duration %llu\n", pSendContext->lastAudioPTS, pSendContext->lastVideoPTS, pSendContext->Duration));
		time = RMmax(pSendContext->lastAudioPTS, pSendContext->lastVideoPTS);
		//time = 1000;   // in miniseconds

		while(seeked_time < pSendContext->Duration * 1000) {
			if (pSendContext->mkvtA) {
				seeked_time = RMSeekMKVTrack(pSendContext->mkvc, time, pSendContext->mkvtA);
				RMDBGLOG((ENABLE, "time: %llu, seeked audio track to %lu\n", time, seeked_time));
			}

			seeked_time = RMSeekMKVTrack(pSendContext->mkvc, time, pSendContext->mkvtV);
            
			RMDBGLOG((ENABLE, "time: %llu, seeked video track to %lu\n", time, seeked_time));
			if (seeked_time >= time)
				break;
			time += 1000;
		}


		if (time < pSendContext->Duration * 1000)
			mkv_seek(pSendContext, time);

	}

	pSendContext->lastVideoPTS = 0;
	pSendContext->lastAudioPTS = 0;

	pSendContext->videoEOS = FALSE;
	pSendContext->audioEOS = FALSE;

	return RM_OK;
}


static void advertise_stream_information(struct SendMKVDataContext * pSendContext)
{
	return;
}


static void check_prebuf_state(struct SendMKVDataContext * pSendContext, RMuint32 buffersize)
{
	RMbool quit_prebuf = FALSE;
	enum RM_PSM_State PlaybackStatus = RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info));
	
	if (PlaybackStatus != RM_PSM_Prebuffering)
		return;

	/* if fail in getbuffer force quitting prebuffering state */
	if (!buffersize) {
		RMDBGLOG((ENABLE, "1\n"));
		quit_prebuf = TRUE;
	}

	if ((pSendContext->mkvtV) && (pSendContext->mkvtA)) {
		if ((pSendContext->videoDataSent >= pSendContext->videoPrebufferSize) &&
		    (pSendContext->audioDataSent >= pSendContext->audioPrebufferSize)) {
			RMDBGLOG((ENABLE, "2\n"));
			quit_prebuf = TRUE;
		}
	}
	else if (pSendContext->mkvtV) {
		if (pSendContext->videoDataSent >= pSendContext->videoPrebufferSize) {
			RMDBGLOG((ENABLE, "3\n"));
			quit_prebuf = TRUE;
		}
	}
	else if (pSendContext->mkvtA) {
		if (pSendContext->audioDataSent >= pSendContext->audioPrebufferSize) {
			RMDBGLOG((ENABLE, "4\n"));
			quit_prebuf = TRUE;
		}
	}

	if ((pSendContext->audioTransferPending) || (pSendContext->videoTransferPending)) {
		RMDBGLOG((ENABLE, "5\n"));
		quit_prebuf = TRUE;
	}

	RMDBGLOG((ENABLE, "prebuffered %lu/%lu bytes of video and %lu/%lu bytes of audio\n", 
		  pSendContext->videoDataSent,
		  pSendContext->videoPrebufferSize,
		  pSendContext->audioDataSent,
		  pSendContext->audioPrebufferSize));

			
	if (quit_prebuf) {

		monitorFIFO(pSendContext, TRUE);

		RMDBGLOG((ENABLE, "exit prebuffering state\n"));
		RMDBGLOG((ENABLE, "setting play state\n"));
		RM_PSM_SetState(pSendContext->PSMcontext, &(pSendContext->dcc_info), RM_PSM_Playing);
		fprintf(stderr, "now playing\n");
		Play(pSendContext, RM_DEVICES_AUDIO | RM_DEVICES_VIDEO | RM_DEVICES_STC, DCCVideoPlayFwd);
		
		pSendContext->prebufferingDone = TRUE;
	}
}


#define MAX_INTERLEAVE_OFFSET 3000           // in milliseconds

static RMuint32 check_interleave(struct SendMKVDataContext * pSendContext)
{
	enum RM_PSM_State PlaybackStatus = RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info));

	RMuint64 nextVpos, nextApos, nextSpos;
	RMuint64 minPos;

	RMuint64 lastVpts, lastApts, lastSpts;

	RMbool   sendAudio, sendVideo;
	
	RMuint64 minPTS, maxPTS;
	RMuint64 min1, min2, min3;
	RMuint64 max1, max2, max3;


	switch (PlaybackStatus) {
	case RM_PSM_NextPic:
	case RM_PSM_Slow:
	case RM_PSM_Fast:
	case RM_PSM_IForward:
	case RM_PSM_IRewind:
	case RM_PSM_IPaused:
	case RM_PSM_INextPic:
		if ((!pSendContext->videoEOS) && (pSendContext->mkvtV))
			return SEND_VIDEO_TRACK;
		else
			return 0;
	default:
		break;
	}
	
    if ((pSendContext->mkvtA) && (!pSendContext->mkvtV)) {
        if (!pSendContext->audioEOS)
            return SEND_AUDIO_TRACK;
        else
            return 0;
    }
    if ((pSendContext->mkvtV) && (!pSendContext->mkvtA)) {
        if (!pSendContext->videoEOS)
            return SEND_VIDEO_TRACK;
        else
            return 0;
    }

	// if reached here, there is more than one track opened

	sendAudio = sendVideo = FALSE;
	
	if ((pSendContext->mkvtA) && (!pSendContext->audioEOS))
		sendAudio = TRUE;

	if ((pSendContext->mkvtV) && (!pSendContext->videoEOS))
		sendVideo = TRUE;

	// check if linear interleaving is still good

	min1 = min2 = 0;
	max1 = max2 = 0;
	
	if (sendAudio)
		min1 = max1 = pSendContext->lastAudioPTS;
	if (sendVideo)
		min2 = max2 = pSendContext->lastVideoPTS;

	minPTS = RMmin(min1, min2);

	maxPTS = RMmax(max1, max2);
	
	if (maxPTS - minPTS > MAX_INTERLEAVE_OFFSET) {
		RMDBGLOG((INTERLEAVE_DBG, "minPTS %llu maxPTS %llu : A %llu V %llu\n", minPTS, maxPTS, min1, min2));
		
		if (pSendContext->linearInterleaving) {
			RMDBGLOG((INTERLEAVE_DBG, "\nA>> maxPTS(%llu) - minPTS(%llu) > %lu | A %llu V %llu| switch to non-linear interleaving\n\n", maxPTS, minPTS, (RMuint32)MAX_INTERLEAVE_OFFSET, min1, min2));
			pSendContext->linearInterleaving = FALSE;
		}
	}
	else {
		RMDBGLOG((INTERLEAVE_DBG, "minPTS %llu maxPTS %llu : A %llu V %llu\n", minPTS, maxPTS, min1, min2));
		
		if (!pSendContext->linearInterleaving) {
			RMDBGLOG((INTERLEAVE_DBG, "\nB>> maxPTS(%llu) - minPTS(%llu) < %lu | A %llu V %llu | switch to linear interleaving\n\n", maxPTS, minPTS, (RMuint32)MAX_INTERLEAVE_OFFSET, min1, min2));
			pSendContext->linearInterleaving = TRUE;
		}
	}

	nextVpos = nextApos = 0;
	lastVpts = lastApts = 0;

	if (sendAudio) {
		if (pSendContext->play_opt->disk_ctrl_state == DISK_CONTROL_STATE_SLEEPING) {
			RMDBGLOG((ENABLE, "trying to read from a suspended drive!!\n"));
			RMDBGLOG((ENABLE, "status:\ndmabufferindex %lu threshold %lu count %lu\n", 
                      pSendContext->dmabuffer_index, 
                      pSendContext->play_opt->disk_ctrl_low_level,
                      pSendContext->play_opt->dmapool_count));
			//monitorFIFO(pSendContext, TRUE);
		}

		RMGetMKVTrackNextReadPos(pSendContext->mkvtA, &nextApos);
		lastApts = pSendContext->lastAudioPTS;
	}

	if (sendVideo) {
		if (pSendContext->play_opt->disk_ctrl_state == DISK_CONTROL_STATE_SLEEPING) {
			RMDBGLOG((ENABLE, "trying to read from a suspended drive!!\n"));
			RMDBGLOG((ENABLE, "status:\ndmabufferindex %lu threshold %lu count %lu\n", 
                      pSendContext->dmabuffer_index, 
                      pSendContext->play_opt->disk_ctrl_low_level,
                      pSendContext->play_opt->dmapool_count));
			//monitorFIFO(pSendContext, TRUE);
		}

		RMGetMKVTrackNextReadPos(pSendContext->mkvtV, &nextVpos);
		lastVpts = pSendContext->lastVideoPTS;
	}

	minPos = RMmin(nextVpos, nextApos);
	minPTS = RMmin(lastVpts, lastApts);

	RMDBGLOG((INTERLEAVE_DBG, "lastApts %llu lastVpts %llu minPTS %llu\n", lastApts, lastVpts, minPTS));
	RMDBGLOG((INTERLEAVE_DBG, "nextApos %llu nextVpos %llu minPos %llu\n", nextApos, nextVpos, minPos));


	if (sendVideo && !sendAudio)
		return SEND_VIDEO_TRACK;
	else if (!sendVideo && sendAudio)
		return SEND_AUDIO_TRACK;

	// if reached here, there is more than one track to send

	RMDBGLOG((INTERLEAVE_DBG, "interleaving: linear %lu : video %lu audio %lu\n", 
              (RMuint32)pSendContext->linearInterleaving, 
              (RMuint32)sendVideo, 
              (RMuint32)sendAudio));


	if (pSendContext->linearInterleaving) {

		// interleave following consecutiveness on disk (linear reads)
		
		if (minPos == nextVpos) {
			// we need to send more video to compensate
			return SEND_VIDEO_TRACK;
		}
		
		if (minPos == nextApos) {
			// we need to send more audio to compensate
			return SEND_AUDIO_TRACK;
		}
		
	}
	else {

		// interleave following PTS


		/* 
		   check fifo's fullness (tries to prevent starvation of the decoders)
		*/

#define AUDIO_FIFO_LOWLEVEL_FULLNESS 5     
#define VIDEO_FIFO_LOWLEVEL_FULLNESS 50    // because it has a higher bitrate than audio

		if (PlaybackStatus != RM_PSM_Prebuffering) {
			struct DataFIFOInfo DataFIFOInfo;
			struct DCCAudioSourceHandle audioHandle;
			RMuint32 vfullness = 100;
			RMuint32 afullness = 100;
			
			if (pSendContext->dcc_info->pVideoSource) {
				RUAGetProperty(pSendContext->dcc_info->pRUA, pSendContext->dcc_info->video_decoder, RMGenericPropertyID_DataFIFOInfo, &DataFIFOInfo, sizeof(DataFIFOInfo));
				vfullness = (100 * DataFIFOInfo.Readable) / DataFIFOInfo.Size;
			}

			if (pSendContext->dcc_info->pMultipleAudioSource) {
				DCCMultipleAudioSourceGetSingleDecoderHandleForInstance(pSendContext->dcc_info->pMultipleAudioSource, 0, &audioHandle);
				RUAGetProperty(pSendContext->dcc_info->pRUA, audioHandle.moduleID, RMGenericPropertyID_DataFIFOInfo, &DataFIFOInfo, sizeof(DataFIFOInfo));
				afullness = (100 * DataFIFOInfo.Readable) / DataFIFOInfo.Size;
			}

			
			RMDBGLOG((DISABLE, "video fullness %lu/100 audio fullness %lu/100\n", vfullness, afullness));

			if ((afullness < AUDIO_FIFO_LOWLEVEL_FULLNESS) &&
			    (vfullness > VIDEO_FIFO_LOWLEVEL_FULLNESS)) {
				// force send audio
				RMDBGLOG((DISABLE, "audio fifo fullness low (audio: %lu / 100, video: %lu / 100), should send more audio\n", afullness, vfullness));
				//minPTS = lastApts;
			}
			
			if ((vfullness < VIDEO_FIFO_LOWLEVEL_FULLNESS) &&
			    (afullness > AUDIO_FIFO_LOWLEVEL_FULLNESS)) {
				// force send video
				RMDBGLOG((ENABLE, "video fifo fullness low (audio: %lu / 100, video: %lu / 100), should send more video\n", afullness, vfullness));
				//minPTS = lastVpts;
			}
				
		}

		
		
		if (minPTS == lastVpts) {
			// we need to send more video to compensate
			return SEND_VIDEO_TRACK;
		}
		
		if (minPTS == lastApts) {
			// we need to send more audio to compensate
			return SEND_AUDIO_TRACK;
		}
    }
		


	RMDBGLOG((INTERLEAVE_DBG, "nothing [else] to interleave!!!\n"));
	return 0;

}



static void flush_and_release_buffers(struct SendMKVDataContext * pSendContext)
{

	RMDBGLOG((ENABLE, "flush dma buffers\n"));

	if (pSendContext->videoDMABuffer) {
		RMDBGLOG((ENABLE, "released a video buffer\n"));
		RUAReleaseBuffer(pSendContext->pDMA, pSendContext->videoDMABuffer);
		pSendContext->videoDMABuffer = NULL;
		pSendContext->videoDMABufferOffset = 0;
		pSendContext->videoTransferPending = FALSE;
	}

	if (pSendContext->audioDMABuffer) {
		RMDBGLOG((ENABLE, "released an audio buffer\n"));
		RUAReleaseBuffer(pSendContext->pDMA, pSendContext->audioDMABuffer);
		pSendContext->audioDMABuffer = NULL;
		pSendContext->audioDMABufferOffset = 0;
		pSendContext->audioTransferPending = FALSE;
	}

	if (pSendContext->startCodeDMABuffer) {
		RMDBGLOG((ENABLE, "released a startcode buffer\n"));
		RUAReleaseBuffer(pSendContext->pDMA, pSendContext->startCodeDMABuffer);
		pSendContext->startCodeDMABuffer = NULL;
		pSendContext->startCodeDMABufferOffset = 0;
	}

	if (pSendContext->play_opt->disk_ctrl_state != DISK_CONTROL_STATE_DISABLE) {
		while (pSendContext->dmabuffer_index > 0) {
			pSendContext->dmabuffer_index--;
			if (pSendContext->dmabuffer_array[pSendContext->dmabuffer_index]) {
				RMDBGLOG((ENABLE, "releasing buffer index %lu buffer %p\n", pSendContext->dmabuffer_index, pSendContext->dmabuffer_array[pSendContext->dmabuffer_index]));
				RUAReleaseBuffer(pSendContext->pDMA, (RMuint8 *)pSendContext->dmabuffer_array[pSendContext->dmabuffer_index]);
			}
		}
	}
	
	return;

}


static RMbool local_process_key(struct SendMKVDataContext * pSendContext, RMbool getkey, RMbool release)
{
	pSendContext->processKeyResultValid = FALSE;

	if (pSendContext->monitorFIFOs)
		monitorFIFO(pSendContext, FALSE);
	
	if (getkey) {
		RMstatus err;
		enum RM_PSM_State FSMstate = RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info));
		if ((FSMstate == RM_PSM_Stopped) || (FSMstate == RM_PSM_Paused)) {
			switch (pSendContext->play_opt->disk_ctrl_state) {
			case DISK_CONTROL_STATE_DISABLE:
			case DISK_CONTROL_STATE_SLEEPING:
				break;
			case DISK_CONTROL_STATE_RUNNING:
				if(pSendContext->play_opt->disk_ctrl_callback && pSendContext->play_opt->disk_ctrl_callback(DISK_CONTROL_ACTION_SLEEP) == RM_OK)
					pSendContext->play_opt->disk_ctrl_state = DISK_CONTROL_STATE_SLEEPING;
				break;
			}
		}
		err = process_command(pSendContext->PSMcontext, &(pSendContext->dcc_info), &(pSendContext->actions));
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Error while processing key %d\n", err));
			goto cleanup;
		}
	}

	pSendContext->ACTUAL_PlaybackStatus = RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info));

	if (pSendContext->actions.toDoActions & RM_PSM_RESYNC_TIMER) {
		RMDBGLOG((ENABLE, "resyncTimer\n"));
		SyncTimerWithDecoderPTS(pSendContext);
		pSendContext->actions.toDoActions &= ~RM_PSM_RESYNC_TIMER;
	}
	if (pSendContext->actions.toDoActions & RM_PSM_FLUSH_VIDEO) {
		RMDBGLOG((ENABLE, "flushVIDEO\n"));
		Stop(pSendContext, RM_DEVICES_VIDEO);
		pSendContext->actions.toDoActions &= ~RM_PSM_FLUSH_VIDEO;
	}
	if (pSendContext->actions.toDoActions & RM_PSM_NORMAL_PLAY) {
		RMDBGLOG((ENABLE, "got play after trickmode\n"));
		if (RMFAILED(mkv_ResumeFromTrickMode(pSendContext))) {
			RMDBGLOG((ENABLE,"Error during resume from trickmode, abort.\n"));
			goto cleanup;
		}
		pSendContext->actions.toDoActions &= ~RM_PSM_NORMAL_PLAY;
		if (pSendContext->isAudioOnly) {
			RMDBGLOG((ENABLE, "seeking"));
			goto mainloop_noseek;
		}
		goto start_sending_data;
	}
	if (pSendContext->actions.toDoActions & RM_PSM_DEMUX_NORMAL) {
		RMDBGLOG((ENABLE, "got play after iframe trickmode\n"));
		if (RMFAILED(mkv_ResumeFromIFrameMode(pSendContext))) {
			RMDBGLOG((ENABLE,"Error during resume from trickmode, abort.\n"));
			goto cleanup;
		}
		pSendContext->isIFrameMode = FALSE;
		pSendContext->IFrameDirection = 0;
		pSendContext->actions.toDoActions &= ~RM_PSM_DEMUX_NORMAL;
		goto mainloop_noseek;
	}
	if (pSendContext->actions.toDoActions & RM_PSM_DEMUX_IFRAME) {
		RMDBGLOG((ENABLE, "trickmode iframe\n"));
        if (RMFAILED(mkv_IFrameTrickMode(pSendContext))) {
			RMDBGLOG((ENABLE,"Error during init trickmode, abort.\n"));
			goto cleanup;
		}
		pSendContext->isIFrameMode = TRUE;
		pSendContext->actions.toDoActions &= ~RM_PSM_DEMUX_IFRAME;
		goto start_sending_data;
	}
	if (pSendContext->actions.toDoActions & RM_PSM_FIRST_PTS) {
		RMDBGLOG((ENABLE, "firstPTS\n"));
		/*pSendContext->FirstSystemTimeStamp = TRUE;*/
		pSendContext->actions.toDoActions &= ~RM_PSM_FIRST_PTS;
	}
	if (pSendContext->actions.performedActions & RM_PSM_VIDEO_STOPPED) {
		RMDBGLOG((ENABLE, "video stopped\n"));
		pSendContext->sendVideoDSI = TRUE;
		pSendContext->actions.performedActions &= ~RM_PSM_VIDEO_STOPPED;
	}
	if (pSendContext->actions.performedActions & RM_PSM_AUDIO_STOPPED) {
		RMDBGLOG((ENABLE, "audio stopped\n"));
		pSendContext->actions.performedActions &= ~RM_PSM_AUDIO_STOPPED;
	}
	if (pSendContext->actions.performedActions & RM_PSM_STC_STOPPED) {
		RMDBGLOG((ENABLE, "stc stopped\n"));
		pSendContext->actions.performedActions &= ~RM_PSM_STC_STOPPED;
	}
	if ((RM_PSM_GetState(pSendContext->PSMcontext, &(pSendContext->dcc_info)) == RM_PSM_Stopped) && (pSendContext->actions.cmdProcessed)) {
		RMDBGLOG((ENABLE,"Got stop command\n"));
		if (release) 
			flush_and_release_buffers(pSendContext);
		pSendContext->isIFrameMode = FALSE;
		pSendContext->IFrameDirection = 0;
		pSendContext->isTrickMode = FALSE;
		pSendContext->FirstSystemTimeStamp = TRUE;
		pSendContext->sendVideoDSI = TRUE;
		if (RMFAILED(mkv_ResumeFromStop(pSendContext))) {
			RMDBGLOG((ENABLE,"Error during resume from trickmode, abort.\n"));
			goto cleanup;
		}
		Stop(pSendContext, RM_DEVICES_STC | RM_DEVICES_AUDIO | RM_DEVICES_VIDEO);
		RM_PSM_SetState(pSendContext->PSMcontext, &(pSendContext->dcc_info), RM_PSM_Stopped);
		goto mainloop_noseek;
	}
	if ((pSendContext->actions.cmd == RM_QUIT) && (!pSendContext->actions.cmdProcessed)) {
		RMDBGLOG((ENABLE, "Got quit command\n"));
		if (release) 
			flush_and_release_buffers(pSendContext);
		pSendContext->actions.cmdProcessed = TRUE;
		goto cleanup;
	}
	if ((pSendContext->actions.cmd == RM_STOP_SEEK_ZERO) && (!pSendContext->actions.cmdProcessed)) {
		RMDBGLOG((ENABLE,"Got stop seek zero command\n"));
		Stop(pSendContext, RM_DEVICES_STC | RM_DEVICES_AUDIO | RM_DEVICES_VIDEO);
		RM_PSM_SetState(pSendContext->PSMcontext, &(pSendContext->dcc_info), RM_PSM_Stopped);
		if (release) 
			flush_and_release_buffers(pSendContext);
		pSendContext->isIFrameMode = FALSE;
		pSendContext->IFrameDirection = 0;
		pSendContext->isTrickMode = FALSE;
		pSendContext->FirstSystemTimeStamp = TRUE;
		pSendContext->sendVideoDSI = TRUE;
		pSendContext->actions.cmdProcessed = TRUE;
		goto mainloop;
	}
	if ((pSendContext->actions.cmd == RM_SEEK) && (!pSendContext->actions.cmdProcessed)){
		RMDBGLOG((ENABLE,"Got seek command\n"));

		if (release) 
			flush_and_release_buffers(pSendContext);

		if (pSendContext->Duration < (RMuint64)pSendContext->dcc_info->seek_time) {
			fprintf(stderr, "cant seek to %lu s! stream length %llu s!, seeking to beginning\n", pSendContext->dcc_info->seek_time, pSendContext->Duration);
			pSendContext->dcc_info->seek_time = 0;
		}
		
        if (RMFAILED(mkv_seek(pSendContext, (RMuint64)pSendContext->dcc_info->seek_time * 1000))) {
			RMDBGLOG((ENABLE,"Error during seek, abort.\n"));
			goto cleanup;
		}
		pSendContext->isIFrameMode = FALSE;
		pSendContext->IFrameDirection = 0;
		pSendContext->isTrickMode = FALSE;
		pSendContext->lastSTC = 0;
		pSendContext->actions.cmdProcessed = TRUE;
		goto mainloop_noseek;
	}
	if ((pSendContext->actions.asyncCmd == RM_AUDIO_STREAM_CHANGE) && (pSendContext->actions.asyncCmdPending)) {
		RMstatus status;

		RMDBGLOG((ENABLE, "got async cmd switch audio stream\n"));
		pSendContext->actions.asyncCmd = RM_NONE;
		pSendContext->actions.asyncCmdPending = FALSE;
		status = SwitchAudio(pSendContext);
		if (status == RM_OK)
			goto start_sending_data;
	}

	if ((pSendContext->actions.cmd == RM_DUALMODE_CHANGE) && (!pSendContext->actions.cmdProcessed)) {
		RMuint32 i;
		RMstatus err;

		fprintf(stderr, "Changing DualMode to :");
		switch(pSendContext->audio_opt[0].OutputDualMode) {
		case DualMode_LeftMono:
			fprintf(stderr, " RightMono\n");
			pSendContext->audio_opt[0].OutputDualMode = DualMode_RightMono;
			break;
		case DualMode_RightMono:
			fprintf(stderr, " MixMono\n");
			pSendContext->audio_opt[0].OutputDualMode = DualMode_MixMono;
			break;
		case DualMode_MixMono:
			fprintf(stderr, " Stereo\n");
			pSendContext->audio_opt[0].OutputDualMode = DualMode_Stereo;
			break;
		case DualMode_Stereo:
			fprintf(stderr, " LeftMono\n");
			pSendContext->audio_opt[0].OutputDualMode = DualMode_LeftMono;
			break;
		default:
			fprintf(stderr, " Unknown dual mode\n");
			break;
		}
		for (i = 0; i < pSendContext->audioInstances; i++) {
			pSendContext->audio_opt[i].OutputDualMode = pSendContext->audio_opt[0].OutputDualMode;

			err = apply_audio_decoder_options_onthefly(pSendContext->dcc_info, &(pSendContext->audio_opt[i]));
			if (RMFAILED(err)) {
				fprintf(stderr, "Error applying audio decoder options on the fly %d\n", err);
			}
		}
		pSendContext->actions.cmdProcessed = TRUE;
	}

	pSendContext->processKeyResult = LABEL_NONE;
	pSendContext->processKeyResultValid = FALSE;
	return FALSE;

 cleanup:
	pSendContext->processKeyResult = LABEL_CLEANUP;
	pSendContext->processKeyResultValid = TRUE;
	return TRUE;

 mainloop:
	pSendContext->processKeyResult = LABEL_MAINLOOP;
	pSendContext->processKeyResultValid = TRUE;
	return TRUE;

 mainloop_noseek:
	pSendContext->processKeyResult = LABEL_MAINLOOP_NOSEEK;
	pSendContext->processKeyResultValid = TRUE;
	return TRUE;

 start_sending_data:
	pSendContext->processKeyResult = LABEL_START_SENDING_DATA;
	pSendContext->processKeyResultValid = TRUE;
	return TRUE;

 signal_EOS_and_exit_loop:
	pSendContext->processKeyResult = LABEL_SIGNAL_EOS_AND_EXIT;
	pSendContext->processKeyResultValid = TRUE;
	return TRUE;

}





static void get_buffer(struct SendMKVDataContext *pSendContext, RMuint8 **buf)
{
	RMuint8 *buffer;

 get_buffer:
	switch (pSendContext->play_opt->disk_ctrl_state) {
	case DISK_CONTROL_STATE_DISABLE:
	case DISK_CONTROL_STATE_SLEEPING:
		break;
	case DISK_CONTROL_STATE_RUNNING:
		if (pSendContext->dmabuffer_index > 0) {
			pSendContext->dmabuffer_index--;
			buffer = (RMuint8 *)pSendContext->dmabuffer_array[pSendContext->dmabuffer_index];
			RMDBGLOG((ENABLE, "fill buffer phase: got buffer %p (index %lu, level %lu, count %lu)\n", 
				  buffer,
				  pSendContext->dmabuffer_index,
				  pSendContext->play_opt->disk_ctrl_low_level,
				  pSendContext->play_opt->dmapool_count));

			goto exit;
		}
		break;
	}
	
	
	while (RUAGetBuffer(pSendContext->pDMA, &(buffer),  COMMON_TIMEOUT_US) != RM_OK) {
		RMDBGLOG((DISABLE, "there are no free buffers for right now, retry later...\n"));
		
		check_prebuf_state(pSendContext, 0);
		
		switch (pSendContext->play_opt->disk_ctrl_state) {
		case DISK_CONTROL_STATE_DISABLE:
		case DISK_CONTROL_STATE_SLEEPING:
			break;
		case DISK_CONTROL_STATE_RUNNING:
			if(pSendContext->play_opt->disk_ctrl_callback && pSendContext->play_opt->disk_ctrl_callback(DISK_CONTROL_ACTION_SLEEP) == RM_OK)
				pSendContext->play_opt->disk_ctrl_state = DISK_CONTROL_STATE_SLEEPING;
			break;
		}
		
		local_process_key(pSendContext, TRUE, FALSE);

		if (pSendContext->ACTUAL_PlaybackStatus != pSendContext->OLD_PlaybackStatus) {
			RMDBGLOG((ENABLE, "old status %lu new status %lu processKeyResult %lu (valid %lu)\n", 
				  (RMuint32)pSendContext->OLD_PlaybackStatus, 
				  (RMuint32)pSendContext->ACTUAL_PlaybackStatus, 
				  (RMuint32)pSendContext->processKeyResult,
				  (RMuint32)pSendContext->processKeyResultValid));
			pSendContext->OLD_PlaybackStatus = pSendContext->ACTUAL_PlaybackStatus;
		}

		if ((pSendContext->processKeyResult != LABEL_NONE) && (pSendContext->processKeyResultValid))
			goto key_pressed;
	}
	
	check_prebuf_state(pSendContext, (1 << pSendContext->dmaBufferSizeLog2));
	
	switch (pSendContext->play_opt->disk_ctrl_state) {
	case DISK_CONTROL_STATE_DISABLE:
	case DISK_CONTROL_STATE_RUNNING:
		break;
	case DISK_CONTROL_STATE_SLEEPING:
		pSendContext->dmabuffer_array[pSendContext->dmabuffer_index] = buffer;
		pSendContext->dmabuffer_index ++;
		if (pSendContext->dmabuffer_index + pSendContext->play_opt->disk_ctrl_low_level >= pSendContext->play_opt->dmapool_count) {
			if(pSendContext->play_opt->disk_ctrl_callback && pSendContext->play_opt->disk_ctrl_callback(DISK_CONTROL_ACTION_RUN) == RM_OK)
				pSendContext->play_opt->disk_ctrl_state = DISK_CONTROL_STATE_RUNNING;
		}
		RMDBGLOG((ENABLE, "recycle buffer phase: got buffer %p (index %lu, level %lu, count %lu)\n", 
			  buffer,
			  pSendContext->dmabuffer_index,
			  pSendContext->play_opt->disk_ctrl_low_level,
			  pSendContext->play_opt->dmapool_count));
		
		goto get_buffer;
	}
	

 exit:
	RMDBGLOG((DISABLE, "got buffer 0x%lx\n", (RMuint32)buffer));

	*buf = buffer;

 key_pressed:

	return;
	
}

#include <sys/time.h>

#ifdef WITH_MONO
int main_mkv(struct mono_info *mono)
{
#else
int main(int argc, char *argv[])
{
	/*for MONO compatibility, always access these variables through the global pointers*/
	struct playback_cmdline playback_options; /*access through play_opt*/
	struct display_cmdline  display_options;/*access through disp_opt*/
	struct video_cmdline video_options; /*access through video_opt*/
	struct audio_cmdline audio_options[MAX_AUDIO_DECODER_INSTANCES]; /*access through audio_opt*/
	struct display_context disp_info;

	// comment the following line to compile this file under em8620branch
	struct dh_context dh_info = {0,};

#endif

	RMstatus err = RM_OK;
	RMint32 error = 0;
	RMuint32 videoscaler_id = 0;
	struct dcc_context dcc_info = {0,}; 
	
	struct RM_PSM_Context PSMContext;

	struct SendMKVDataContext SendContext;
	RMuint32 i;
    MKVTrackCacheBuffer vcache;
    MKVTrackCacheBuffer acache;

    fprintf (stderr, "%s ready to run..., pid = %u, ppid = %u\n", __FUNCTION__, (uint32_t)getpid(), (uint32_t)pthread_self());

    struct timeval begin, current;
    gettimeofday (&begin, NULL);

	// clear context
	memset(&SendContext, 0, sizeof(SendContext));
	memset(&dcc_info, 0, sizeof(dcc_info));

	SendContext.h264PktLenBuf = SendContext.h264DummyBuffer;

	init_private_options(&(SendContext.priv_opt));

    RMDBGLOG((ENABLE, "mkv play!\n"));
#ifdef WITH_MONO
    vcache.buffer = mono->video_cache_buffer;
    vcache.buffer_size = mono->video_cache_buffer_size;

    acache.buffer = mono->audio_cache_buffer;
    acache.buffer_size = mono->audio_cache_buffer_size;


	/*make the mono arguments global*/
	SendContext.play_opt = mono->play_opt;
	SendContext.video_opt = mono->video_opt;
	SendContext.disp_opt = mono->disp_opt;
	SendContext.audio_opt = mono->audio_opt;

	SendContext.audioInstances = SendContext.audio_opt[0].audioInstances;

	RMDBGLOG((ENABLE, "audio instances %lu, %lu\n", SendContext.audioInstances, SendContext.audio_opt[0].audioInstances));

	print_parsed_audio_options(SendContext.audio_opt);

	dcc_info.pRUA = mono->pRUA;
	dcc_info.pDCC = mono->pDCC;
	videoscaler_id = mono->video_scaler;
	dcc_info.route = DCCRoute_Main;

#else
    vcache.buffer = NULL; //mono->video_track_cache_buffer;
    vcache.buffer_size = 4*1024*1024 - 1024; //mono->video_track_cache_size;

    acache.buffer = NULL; //mono->audio_track_cache_buffer;
    acache.buffer_size = 256*1024 - 1024; //mono->audio_track_cache_size;

	SendContext.play_opt = &playback_options;
	SendContext.disp_opt = &display_options;
	SendContext.video_opt = &video_options;
	SendContext.audio_opt = audio_options;

	init_display_options(SendContext.disp_opt);
	init_video_options(SendContext.video_opt);
	init_playback_options(SendContext.play_opt);

	init_audio_options2(SendContext.audio_opt, MAX_AUDIO_DECODER_INSTANCES);

	SendContext.disp_opt->dh_info = &dh_info;

	for (i = 0; i < MAX_AUDIO_DECODER_INSTANCES; i++)
		SendContext.audio_opt[i].dh_info = &dh_info;

	parse_cmdline(&SendContext, argc, argv, &SendContext.audioInstances);
	SendContext.audioInstances++;

	RMDBGLOG((ENABLE, "audio instances %lu, %lu\n", SendContext.audioInstances, SendContext.audio_opt[0].audioInstances));

	print_parsed_audio_options(SendContext.audio_opt);

	videoscaler_id = SendContext.disp_opt->video_scaler;
	dcc_info.route = SendContext.disp_opt->route;


	RMDBGLOG((ENABLE, "create RUA instance\n"));
	err = RUACreateInstance(&dcc_info.pRUA, SendContext.play_opt->chip_num);
	if (RMFAILED(err)) {
		RMDBGLOG((ENABLE, "Error creating RUA instance! %d\n", err));
		goto exit_with_error;
	}
       
	RMDBGLOG((ENABLE, "open dcc\n"));
	err = DCCOpen(dcc_info.pRUA, &dcc_info.pDCC);
	if (RMFAILED(err)) {
		RMDBGLOG((ENABLE, "Error Opening DCC! %d\n", err));
		goto exit_with_error;
	}

	if (!SendContext.play_opt->noucode) {
		RMDBGLOG((ENABLE, "init microcode\n"));
		err = DCCInitMicroCodeEx(dcc_info.pDCC, SendContext.disp_opt->init_mode);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Cannot initialize microcode %d\n", err));
			goto exit_with_error;
		}
	}
	else
		RMDBGLOG((ENABLE, "microcode not loaded\n"));

//	display_key_usage(KEYFLAGS);
#endif //WITH_MONO

	dcc_info.disp_info = NULL;
	SendContext.dcc_info = &dcc_info;
	SendContext.PSMcontext = &PSMContext;
	PSMContext.validPSMContexts = 1;
	PSMContext.currentActivePSMContext = 1;
	PSMContext.keyflags = KEYFLAGS;


	/* if HD control is enabled and mode is auto, setup parameters */
	if ((SendContext.play_opt->disk_ctrl_low_level) &&
	    (SendContext.play_opt->disk_ctrl_log2_block_size) &&
	    (SendContext.play_opt->disk_ctrl_max_mem)) {
		RMuint32 bufferSize = 0;
		RMuint32 bufferCount = 0;
		RMuint32 log2BlockSize = SendContext.play_opt->disk_ctrl_log2_block_size;
		RMuint32 maxBufferingMem = SendContext.play_opt->disk_ctrl_max_mem;
		enum MPEG_Profile temp;

		bufferSize = (1 << log2BlockSize);
		bufferCount = maxBufferingMem >> log2BlockSize;
	
		SendContext.play_opt->dmapool_count = bufferCount;
		SendContext.play_opt->dmapool_log2size = log2BlockSize;
	
		/* from #4005
		   
		videoOpt.fifo_size = 4*1024*1024; 
		videoOpt.xfer_count = (1<<playOpt.dmapool_log2size)/1024*playOpt.dmapool_count;
		audioOpt.fifo_size = 1*1024*1024; 
		audioOpt.xfer_count = (1<<playOpt.dmapool_log2size)/512*playOpt.dmapool_count;
		
		*/

		if (SendContext.play_opt->disk_ctrl_low_level >= bufferCount)
			SendContext.play_opt->disk_ctrl_low_level = bufferCount >> 1;
	
		SendContext.video_opt->fifo_size = 4 * (1024 * 1024);
		SendContext.audio_opt[0].fifo_size = 2 * (1024 * 1024);

		fprintf(stderr, ">> low level %lu => %lu bytes bufferized (+ bitstreamFIFO)\n", 
			SendContext.play_opt->disk_ctrl_low_level,
			SendContext.play_opt->disk_ctrl_low_level * bufferSize);

		SendContext.video_opt->xfer_count = (bufferSize / 512) * bufferCount;
		SendContext.audio_opt[0].xfer_count = (bufferSize / 512) * bufferCount;

		
		temp = SendContext.video_opt->MPEGProfile;
#ifdef WITH_MONO
		// we still dont know if it's HD or SD, check for H264 HD 
		SendContext.video_opt->MPEGProfile = Profile_H264_HD_DeInt;
#endif
		err = setup_disk_control_parameters(&dcc_info, SendContext.play_opt, &(SendContext.audio_opt[0]), SendContext.video_opt, NULL);
		SendContext.video_opt->MPEGProfile = temp;

		if (err != RM_OK) {
			fprintf(stderr, "Error %d trying to setup HD control params\n", err);
			goto exit_with_error;
		}
	}

	/* update dmapool size and count */
	if (SendContext.play_opt->dmapool_count)
		SendContext.dmaBufferCount = SendContext.play_opt->dmapool_count;
	else
		SendContext.dmaBufferCount = DMA_BUFFER_COUNT;

	if (SendContext.play_opt->dmapool_log2size)
		SendContext.dmaBufferSizeLog2 = SendContext.play_opt->dmapool_log2size;
	else
		SendContext.dmaBufferSizeLog2 = DMA_BUFFER_SIZE_LOG2;

	/* update fifo and xfer size */
	if (SendContext.video_opt->fifo_size) 
		SendContext.videoBitstreamFIFOSize = SendContext.video_opt->fifo_size;
	else
		SendContext.videoBitstreamFIFOSize = VIDEO_FIFO_SIZE;

	if (SendContext.video_opt->xfer_count) 
		SendContext.videoFIFOXFERCount = SendContext.video_opt->xfer_count;
	else
		SendContext.videoFIFOXFERCount = VIDEO_XFER_FIFO_COUNT;

	for (i = 0; i < SendContext.audioInstances; i++) {

		if (SendContext.audio_opt[i].fifo_size) 
			SendContext.audioBitstreamFIFOSize[i] = SendContext.audio_opt[i].fifo_size;
		else
			SendContext.audioBitstreamFIFOSize[i] = AUDIO_FIFO_SIZE;
		
		if (SendContext.audio_opt[i].xfer_count) 
			SendContext.audioFIFOXFERCount[i] = SendContext.audio_opt[i].xfer_count;
		else
			SendContext.audioFIFOXFERCount[i] = AUDIO_XFER_FIFO_COUNT;

		RMDBGLOG((ENABLE, "Audio[%lu]:\n\tBitstreamFIFOSize: %lu\n\tFIFOXFERCount: %lu\n", 
			  i,
			  SendContext.audioBitstreamFIFOSize[i], 
			  SendContext.audioFIFOXFERCount[i]));
	}
		
	RMDBGLOG((ENABLE, "Video:\n\tBitstreamFIFOSize: %lu\n\tFIFOXFERCount: %lu\n", 
		  SendContext.videoBitstreamFIFOSize, 
		  SendContext.videoFIFOXFERCount));
	RMDBGLOG((ENABLE, "DMA Pool:\n\tSize: %ld\n\tBufferCount: %ld\n\tBufferSize: %ld\n", 
		  (SendContext.dmaBufferCount << SendContext.dmaBufferSizeLog2), 
		  SendContext.dmaBufferCount, 
		  (1<<SendContext.dmaBufferSizeLog2)));


	switch (SendContext.play_opt->disk_ctrl_state) {
	case DISK_CONTROL_STATE_DISABLE:
		break;
	case DISK_CONTROL_STATE_SLEEPING:
	case DISK_CONTROL_STATE_RUNNING:
		SendContext.dmabuffer_array = (void **) RMMalloc(sizeof(void*) * SendContext.play_opt->dmapool_count);
		SendContext.dmabuffer_index = 0;
		if (SendContext.dmabuffer_array == NULL) {
			RMDBGLOG((ENABLE, "Cannot allocate dmapool array! Disable disk control\n"));
			SendContext.play_opt->disk_ctrl_state = DISK_CONTROL_STATE_DISABLE;
		}
		break;
	}

	SendContext.monitorFIFOs = SendContext.priv_opt.monitorFIFOs;

	//SendContext.monitorFIFOs = TRUE;

//	if (SendContext.priv_opt.internal_file_open){
		//RMnonAscii *naname = RMnonAsciiFromAscii(SendContext.play_opt->filename);
    RMOpenFileMKV(SendContext.play_opt->filename, &SendContext.mkvc);
		//RMFreeNonAscii(naname);
		//if (RMFAILED(err)) {
		//	RMDBGLOG((ENABLE, "Error opening mkv file %s\n", SendContext.play_opt->filename));
		//	goto exit_with_error;
		//}
//	} 
    /*
    else {
		
		struct stream_options_s stream_options;

		err = init_stream_options(&stream_options);
		if (RMFAILED(err)){
			RMDBGLOG((ENABLE,"Error setting stream options\n"));
			goto cleanup;
		}

		err = set_http_options(&stream_options, RM_HTTP_OPEN_CACHED, NULL);
		if (RMFAILED(err)){
			RMDBGLOG((ENABLE,"Error setting stream options\n"));
			goto cleanup;
		}

		RMDBGLOG((ENABLE, "Open external file %s\n", SendContext.play_opt->filename));
		SendContext.mkv_file = open_stream(SendContext.play_opt->filename, RM_FILE_OPEN_READ, &stream_options);
		if (SendContext.mkv_file == NULL) {
			fprintf(stderr, "Cannot open file %s\n", SendContext.play_opt->filename);
			err = RM_ERROR;
			goto exit_with_error;
		}

		err = RMOpenExternalFileMKV(SendContext.mkv_file, "MKV", &SendContext.mkvc);
		if (RMFAILED(err)) {
			fprintf(stderr, "Error opening file %s\n", SendContext.play_opt->filename);
			goto exit_with_error;
		}

		RMSizeOfOpenFile(SendContext.mkv_file, &SendContext.fileSize);
	}
*/

    RMSetAVTrackCacheMKV(SendContext.mkvc, &vcache, &acache);

	SendContext.SendVideoData = SendContext.play_opt->send_video;
	SendContext.SendVideoPts = SendContext.play_opt->send_video_pts;
	
	SendContext.SendAudioData = SendContext.play_opt->send_audio;
	SendContext.SendAudioPts = SendContext.play_opt->send_audio_pts;
	
	/* check if it's a NeroDigital file */
//	SendContext.isNeroDigitalFile = RMisNeroMKV(SendContext.mkvc);
//	if (SendContext.isNeroDigitalFile) {
//		RMDBGLOG((ENABLE, ">> NeroDigital file\n"));
//		/* check for NeroDigital DRM permission for file playback */
//		SendContext.playbackAllowed = RMcanNeroMKVbePlayed(SendContext.mkvc);
//	}

/*
	{
		RMuint32 time;
		err = RMGetMKVClosestChapter(SendContext.mkvc, 0, &time, NULL, 256, 0);
		if (err == RM_OK)
			SendContext.hasChapters = TRUE;
		else
			SendContext.hasChapters = FALSE;
	}
*/	
	err = init_MKV_tracks(&SendContext);
	if (err != RM_OK) {
		RMDBGLOG((ENABLE,"Failed to init all known video and audio tracks!!\n"));
		goto exit_with_error;
	}

	if ((SendContext.videoTracks > 0) && (SendContext.play_opt->require_video_audio) && (SendContext.currentVideoTrack == 0) && (SendContext.play_opt->send_video)) {
		err = RM_ERROR;
		RMDBGLOG((ENABLE, "couldnt init the video track and video required is enabled, exit\n"));
		goto exit_with_error;
	}

	if (SendContext.mkvtV) {
		RMuint32 Vcount = 0;

		RMGetMKVTrackSize(SendContext.mkvtV, &(SendContext.videoTrackSize), &Vcount);
		if (Vcount)
			SendContext.videoMeanSampleSize = SendContext.videoTrackSize / Vcount;

		RMDBGLOG((ENABLE, "video track size %lu, count %lu, mean %lu\n", SendContext.videoTrackSize, Vcount, SendContext.videoMeanSampleSize));

		SendContext.avRatio = -1;
	}

	if (SendContext.mkvtA) {
		RMuint32 Acount = 0;

		RMGetMKVTrackSize(SendContext.mkvtA, &(SendContext.audioTrackSize), &Acount);
		if (Acount)
			SendContext.audioMeanSampleSize = SendContext.audioTrackSize / Acount;

		RMDBGLOG((ENABLE, "audio track size %lu, count %lu, mean %lu\n", SendContext.audioTrackSize, Acount, SendContext.audioMeanSampleSize));

		if (SendContext.audioTrackSize) {
			SendContext.avRatio =  (RMint32) (SendContext.videoTrackSize / SendContext.audioTrackSize);
			RMDBGLOG((ENABLE, ">> A/V ratio: 1:%lu\n", SendContext.avRatio));
		}
	}
	
	

	{
		RMuint32 total = (RMuint32)SendContext.avRatio + 1;
		RMuint32 prebuf = SendContext.dmaBufferCount << SendContext.dmaBufferSizeLog2; //prebuf using the whole dmaPoolSize
		RMuint32 Axfer = 0;
		RMuint32 Vxfer = 0;

		if ((SendContext.play_opt->prebuf_max) /*&& (SendContext.play_opt->prebuf_max < prebuf)*/) {
		        prebuf = SendContext.play_opt->prebuf_max;
		
			RMDBGLOG((ENABLE, "prebuf_max %lu\n", prebuf));
		}
		else
			RMDBGLOG((ENABLE, "dmaPoolSize %lu (%lu buffers of %lu bytes)\n", prebuf, SendContext.dmaBufferCount, (1<<SendContext.dmaBufferSizeLog2)));

		if (SendContext.avRatio >= 0) {
			SendContext.videoPrebufferSize = (SendContext.avRatio * prebuf) / total;
			SendContext.audioPrebufferSize = prebuf / total;
		}
		else {
			SendContext.videoPrebufferSize = prebuf;
			SendContext.audioPrebufferSize = 0;
		}
		
		if (SendContext.audioMeanSampleSize)
			Axfer = (SendContext.audioPrebufferSize / SendContext.audioMeanSampleSize) * 2;

		if (SendContext.videoMeanSampleSize)
			Vxfer = (SendContext.videoPrebufferSize / SendContext.videoMeanSampleSize) * 2;

		RMDBGLOG((ENABLE, "prebuffering ratio video/audio = %lu/%lu\n", SendContext.videoPrebufferSize, SendContext.audioPrebufferSize));
		RMDBGLOG((ENABLE, "estimated videoXFERcount %lu, audioXFERcount %lu\n", Vxfer, Axfer));

		for (i = 0; i < SendContext.audioInstances; i++) {
			if (SendContext.audioFIFOXFERCount[i] < Axfer) {
				if (SendContext.audio_opt[i].xfer_count)
					fprintf(stderr, "not enough xfer entries to prebuffer audio, playback might fail!, increase the value you specified on cmdline\n");
				else {
					RMDBGLOG((ENABLE, ">> increasing audio xfer count to %lu\n", Axfer));
					SendContext.audioFIFOXFERCount[i] = Axfer;
				}
			}
		}
		
		if (SendContext.videoFIFOXFERCount < Vxfer) {
			if (SendContext.video_opt->xfer_count)
				fprintf(stderr, "not enough xfer entries to prebuffer video, playback might fail!, increase the value you specified on cmdline\n");
			else {
				RMDBGLOG((ENABLE, ">> increasing video xfer count to %lu\n", Vxfer));
				SendContext.videoFIFOXFERCount = Vxfer;
			}
		}
	}



	err = open_save_files(SendContext.play_opt);
	if (RMFAILED(err)) {
		RMDBGLOG((ENABLE, "cannot open files to save data %d\n", err));
		goto exit_with_error;
	}

	//play_opt->save_video = FALSE;
	//play_opt->save_audio = FALSE;

	if (SendContext.isAudioOnly)
		RMDBGLOG((ENABLE, ">>> audio only file\n"));

	SendContext.pRUA = dcc_info.pRUA;
	SendContext.pDCC = dcc_info.pDCC;

	/* playback commands enabled */
	SendContext.dcc_info->RM_PSM_commands = RM_PSM_ENABLE_PLAY;
	SendContext.dcc_info->RM_PSM_commands |= RM_PSM_ENABLE_STOP;
	SendContext.dcc_info->RM_PSM_commands |= RM_PSM_ENABLE_PAUSE;
	SendContext.dcc_info->RM_PSM_commands |= RM_PSM_ENABLE_SPEED;
	SendContext.dcc_info->RM_PSM_commands |= RM_PSM_ENABLE_FASTER;
	SendContext.dcc_info->RM_PSM_commands |= RM_PSM_ENABLE_SLOWER;
	SendContext.dcc_info->RM_PSM_commands |= RM_PSM_ENABLE_SEEK;

	if (SendContext.audioTracks > 1) {
		RMDBGLOG((ENABLE, ">> enable audio switching\n"));
		SendContext.dcc_info->RM_PSM_commands |= RM_PSM_ENABLE_SWITCHAUDIO;
	}

	RMDBGLOG((ENABLE, "file: %s, size %llu, duration %llu\n", SendContext.play_opt->filename, SendContext.fileSize, SendContext.play_opt->duration / 1000));
	SendContext.Duration = SendContext.play_opt->duration / 1000;

	if (!SendContext.isAudioOnly) {
		SendContext.dcc_info->RM_PSM_commands |= RM_PSM_ENABLE_NEXTPIC;
		SendContext.dcc_info->RM_PSM_commands |= RM_PSM_ENABLE_IFWD;
		SendContext.dcc_info->RM_PSM_commands |= RM_PSM_ENABLE_IRWD;
	}


	RMDBGLOG((ENABLE, "apply playback options\n"));
	err = apply_playback_options(&dcc_info, SendContext.play_opt);
	if (RMFAILED(err)) {
		RMDBGLOG((ENABLE, "Cannot set playback options %d\n", err));
		goto exit_with_error;
	}


	{
		// open first stc module
		struct DCCStcProfile stc_profile;

		RMDBGLOG((ENABLE, "using STC ID %lu\n", SendContext.play_opt->STCid));
		stc_profile.STCID = SendContext.play_opt->STCid;
		stc_profile.master = Master_STC;
		
		stc_profile.stc_timer_id = 3*stc_profile.STCID+0;
		stc_profile.stc_time_resolution = 90000;
		
		stc_profile.video_timer_id = 3*stc_profile.STCID+1;
		stc_profile.video_time_resolution = 90000;
		stc_profile.video_offset = 0;
		
		stc_profile.audio_timer_id = 3*stc_profile.STCID+2;
		stc_profile.audio_time_resolution = 90000;
		stc_profile.audio_offset = 0;
		
		SendContext.stc_offset_ms = -SendContext.play_opt->audio_delay_ms;
		
		RMDBGLOG((ENABLE, "open STC\n"));
		err = DCCSTCOpen(dcc_info.pDCC, &stc_profile, &dcc_info.pStcSource);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Cannot open stc module %d\n", err));
			goto exit_with_error;
		}
	}

	/* Setup Video */
	if (SendContext.SendVideoData) {
		struct DCCXVideoProfile video_profile;
		enum EMhwlibVideoCodec vcodec;

		video_profile.BitstreamFIFOSize = SendContext.videoBitstreamFIFOSize;
	 	video_profile.XferFIFOCount = SendContext.videoFIFOXFERCount;
		video_profile.MpegEngineID = SendContext.video_opt->MpegEngineID;
		video_profile.VideoDecoderID = SendContext.video_opt->VideoDecoderID;

		video_profile.ProtectedFlags = 0;
		video_profile.PtsFIFOCount = 60 * 10; // 10seconds at 60fps
		video_profile.InbandFIFOCount = 16;
		video_profile.XtaskInbandFIFOCount = 0;
		video_profile.SPUBitstreamFIFOSize = 0;
		video_profile.SPUXferFIFOCount = 0;
		video_profile.STCID = SendContext.play_opt->STCid;
	
		/* set codec based on command line options either "-pv" or "-vcodec" */
		if (SendContext.video_opt->vcodec_max_width) {
			video_profile.Codec = SendContext.video_opt->vcodec;
			video_profile.Profile = SendContext.video_opt->vcodec_profile;
			video_profile.Level = SendContext.video_opt->vcodec_level;
			video_profile.MaxWidth = SendContext.video_opt->vcodec_max_width;
			video_profile.MaxHeight = SendContext.video_opt->vcodec_max_height;
		}
		else {
			err = video_profile_to_codec(SendContext.video_opt->MPEGProfile, 
						     &video_profile.Codec,
						     &video_profile.Profile, 
						     &video_profile.Level, 
						     &video_profile.ExtraPictureBufferCount,
						     &video_profile.MaxWidth, 
						     &video_profile.MaxHeight);
			if (RMFAILED(err)) {
				RMDBGLOG((ENABLE, "Unknown video decoder codec \n"));
				goto exit_with_error;
			}
		}

		/* set the extra pictures after the profile to codec conversion */
		video_profile.ExtraPictureBufferCount = SendContext.video_opt->vcodec_extra_pictures;

		err = DCCXOpenVideoDecoderSource(dcc_info.pDCC, &video_profile, &(dcc_info.pVideoSource));
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Cannot open video decoder %d\n", err));
			goto exit_with_error;
		}
		
	
		vcodec = video_profile.Codec;
		err = DCCXSetVideoDecoderSourceCodec(dcc_info.pVideoSource, vcodec);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Cannot set video decoder codec %d\n", err));
			goto exit_with_error;
		}
	
		RMDBGLOG((ENABLE, "get scaler moduleID\n"));
		err = DCCGetScalerModuleID(dcc_info.pDCC, dcc_info.route, DCCSurface_Video, videoscaler_id, &(dcc_info.SurfaceID));
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Cannot get surface to display video source %d\n", err));
			goto exit_with_error;
		}
	
		RMDBGLOG((ENABLE, "set surface source to %lu\n", dcc_info.SurfaceID));
		err = DCCSetSurfaceSource(dcc_info.pDCC, dcc_info.SurfaceID, dcc_info.pVideoSource);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Cannot set the surface source %d\n", err));
			goto exit_with_error;
		}
	
		RMDBGLOG((ENABLE, "get video decoder source info\n"));
		err = DCCGetVideoDecoderSourceInfo(dcc_info.pVideoSource, &(dcc_info.video_decoder), &(dcc_info.spu_decoder), &(dcc_info.video_timer));
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Error getting video decoder source information %d\n", err));
			goto exit_with_error;
		}
		

		RMDBGLOG((ENABLE, "apply video options\n"));
		err = apply_video_decoder_options(&dcc_info, SendContext.video_opt);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Cannot set video options %d\n", err));
			goto exit_with_error;
		}
		
#ifndef WITH_MONO	     
		dcc_info.disp_info = &disp_info;
		dcc_info.dh_info = &dh_info;
		set_default_out_window(&(dcc_info.disp_info->out_window));
		set_default_out_window(&(dcc_info.disp_info->osd_window[0]));
		set_default_out_window(&(dcc_info.disp_info->osd_window[1]));
		dcc_info.disp_info->active_window = &(dcc_info.disp_info->out_window);
		dcc_info.disp_info->video_enable = TRUE;

		RMDBGLOG((ENABLE, "apply display options\n"));
		err = apply_display_options(&dcc_info, SendContext.disp_opt);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Cannot set display options %d\n", err));
			goto exit_with_error;
		}
#endif /*WITH_MONO*/
		
	}

	RMDBGLOG((ENABLE, "open dma pool, size %ld, count %ld\n", (1<<SendContext.dmaBufferSizeLog2), SendContext.dmaBufferCount));
	err = RUAOpenPool(dcc_info.pRUA, 0, SendContext.dmaBufferCount, SendContext.dmaBufferSizeLog2, RUA_POOL_DIRECTION_SEND, &(SendContext.pDMA));
	if (RMFAILED(err)) {
		RMuint32 poolSize = SendContext.play_opt->dmapool_count << SendContext.play_opt->dmapool_log2size;

		fprintf(stderr, "Error cannot open dmapool %d\n\n"
			"requested %lu bytes of dmapool (%lu buffers of %lu bytes), make sure you\n"
			"loaded llad with the right parameters. For example:\n"
			"max_dmapool_memory_size >= %lu max_dmabuffer_log2_size >= %lu\n\n",
			err,
			poolSize,
			SendContext.play_opt->dmapool_count,
			(RMuint32)(1<<SendContext.play_opt->dmapool_log2size),
			poolSize,
			SendContext.play_opt->dmapool_log2size);

		goto exit_with_error;
	}

	if (SendContext.SendAudioData) {
		err = setup_audio_decoder(&SendContext);
		if (err != RM_OK)
			goto exit_with_error;
	}

#ifndef WITH_MONO
	RMTermInit(TRUE);    // don't allow ctrl-C and the like ...
	RMSignalInit(NULL, NULL);  // ... but catch other termination signals to call RMTermExit()
	SendContext.initTerminal = TRUE;
#endif
	DetectLength(&SendContext);


	/* inform curacao/mono about the stream properties/info of the file, a callback for curacao/mono will be called inside
	   this function
	*/
	advertise_stream_information(&SendContext);

	RMDBGLOG((ENABLE, "begin playback...\n"));

	SendContext.linearInterleaving = TRUE; // linear interleaving is default


	if ((!SendContext.AudioCTSTimeScale) && (!SendContext.VideoCTSTimeScale)) {
		RMDBGLOG((ENABLE, ">>> NO TIME SCALES!, exiting\n"));
		err = RM_ERROR;
		goto exit_with_error;
	}
	if (!SendContext.AudioCTSTimeScale) {
		RMDBGLOG((ENABLE, ">>> no AudioCTSTimeScale, reset to VideoCTSTimeScale(%lu)\n", SendContext.VideoCTSTimeScale));
		SendContext.AudioCTSTimeScale = SendContext.VideoCTSTimeScale;
	}
	if ((!SendContext.VideoCTSTimeScale) || (SendContext.isAudioOnly)) {
		RMDBGLOG((ENABLE, ">>> no VideoCTSTimeScale, reset to AudioCTSTimeScale(%lu)\n", SendContext.AudioCTSTimeScale));
		SendContext.VideoCTSTimeScale = SendContext.AudioCTSTimeScale;
	}
	
	if ((!SendContext.isAudioOnly) && (dcc_info.pVideoSource)) {
		if (SendContext.video_opt->VopInfo.FixedVopRate) {
			RMDBGLOG((ENABLE, ">>> fixed vop rate from command line: %ld / %ld frames per second\n",
				  SendContext.video_opt->VopInfo.VopTimeIncrementResolution, 
				  SendContext.video_opt->VopInfo.FixedVopTimeIncrement));
			SendContext.video_vop_tir = SendContext.video_opt->VopInfo.VopTimeIncrementResolution;
		}
		else if (SendContext.video_opt->vtimescale.enable) {
			RMDBGLOG((ENABLE, ">>> video time scale from command line: %ld ticks per second\n", SendContext.video_opt->vtimescale.time_resolution));
			SendContext.video_vop_tir = SendContext.video_opt->vtimescale.time_resolution;
		}
		else {
			/*
			The "system timestamps" or "system PTS" coming from the system layer are associated
			by video decoder with decoded frames, based on byte counter.
			For the frames that don't have associated "system PTS" the video decoders
			interpolate a PTS based on a value called "time increment" or "delta_pts".
			This value can result from the specific time information in the video stream or
			can be forced by application.
			
			Mpeg2 interpolates the PTS-es in 45k resolution based on the video frame rate.
			Divx3 and WMV have "system PTS" associated with every frame.
			H264, VC1 decoders have two modes to interpolate the timestamps (1 and 2).
			MPEG4 decoder has three modes to interpolate timestamps(1, 2, 3).
			
			1. Autodetection of "time increment resolution" and "time increment" specific
			to the decoder specification (frame rate for Mpeg2, DSI fot mpeg4, SEI).
			This is the default mode of video microcode.
			The application cannot change the detected time increment resolution and has to
			scale the "system PTS" in same resolution.
			The microcode interpolates the unknown time stamps using "time increment"
			information from the video stream.
			
			2. Fixed vop rate. This mode can be enabled/disabled by property 
			RMVideoDecoderPropertyID_VopInfo.
			The microcode uses the "time increment resolution" provided by application and
			interpolates the unknown time stamps using the fixed "time increment" provided
			by application.
			The microcode ignores both the "time increment resolution" and the "time increment"
			information from video stream.
			The application has to scale the "system PTS" to the fixed "time increment resolution"
			provided in the property RMVideoDecoderPropertyID_VopInfo.

			The MPEG4 decoder has an extra mode to interpolate timestamps:
			
			3. Scaling INTERPOLATED time stamps at a specified time resolution. This
			mode can be enabled/disabled by property RMVideoDecoderPropertyID_VideoTimeScale.
			The microcode interpolates the unknown timestamps based on the time information
			from the video stream.
			The application has to provide the "time increment resolution" in the property
			and to scale the "system PTS" to it.

			NOTE. If the time resolution is selected as VideoCTSTimeScale, no scaling is needed.
			We could also remove the DSI parsing.

			NOTE. Some RESTRICTIONS apply: the time resolution is limited to 16 bits or
			a value of 65535 !
			The value 90000 is the only exception accepted by emhwlib, because internally
			it is divided to 45000, toghether with the timestamps.

			The current emhwlib leaves the microcode in mode 1.
		   
			RMVideoDecoderPropertyID_VopInfo or RMVideoDecoderPropertyID_VideoTimeScale
			can be used to enable/disable or overwrite these values.
			*/
			
			/* for Mpeg4 decoder try to not scale in application the "system CTS".
			Try to use mode 3 for Mpeg4. */
			if (SendContext.VideoCTSTimeScale < 65536)
				SendContext.video_opt->vtimescale.time_resolution = SendContext.VideoCTSTimeScale;
			else
				SendContext.video_opt->vtimescale.time_resolution = 90000; /* scale CTS is needed */
				
			SendContext.video_opt->vtimescale.enable = TRUE;
			err = RUASetProperty(dcc_info.pRUA, dcc_info.video_decoder, RMVideoDecoderPropertyID_VideoTimeScale,
					     &SendContext.video_opt->vtimescale, sizeof(SendContext.video_opt->vtimescale), 0 );
			if (RMSUCCEEDED(err)) {
				SendContext.video_vop_tir = SendContext.video_opt->vtimescale.time_resolution;
				RMDBGLOG((ENABLE, ">>> set video time scale: %ld ticks per second\n",
					  SendContext.video_opt->vtimescale.time_resolution));
			}
			else {
#if 1
				SendContext.video_vop_tir = 90000;
				RMDBGLOG((ENABLE,">>> video time scale not supported, video_vop_tir %lu\n", SendContext.video_vop_tir));
				
#else
					
				/*
				  If we assume that:
				  - the video doesn't have a system PTS associated with every frame
				  - the video stream doesn't have any information about the frame rate
				we HAVE TO provide to the VideoDecoder the correct default time increment
				per video frame. This value is used by VideoDecoder to calculate (interpolate)
				the pts of the frames that don't have system pts-es associated.
				  We have to know the correct frame rate of the clip and set the fixed time
				increment according to it.
				  The property RMVideoDecoderPropertyID_VopInfo sets the fixed time increment
				for Mpeg4, H264, Vc1, Divx3 video decoders.
				Mpeg2 doesn't support this mode and the property returns error.
				For Divx3 and Wmv the property doesn't make sense to be used because there are
				"system PTS" associated for every frame.
   
				  At the moment when we set the property we also change the default time increment
				resolution to the value in VopInfo.VopTimeIncrementResolution. This means that
				we have to send the system PTS-es scaled at this resolution.
   
				  NOTE. The property is not needed if the video stream contains already the
				frame rate information - in this case the microcode sets the default time
				increment.
				  If the property is not called and the video stream doesn't contain frame rate
				information the default time increment (known also as delta_pts) is 30 frames
				per second.
				*/

				/* At this moment we have to know the correct frame rate otherwise it is better
				to not call the fixed rate property.
				   Assume SendContext.VideoCTSTimeScale gives a correct frame rate.*/
				
				if (0) {
					/* Method 1. Send system PTS-es scaled at 90k and set fixed time according to the frame rate.*/
					SendContext.video_opt->VopInfo.FixedVopRate = TRUE;
					SendContext.video_vop_tir = 90000;
					SendContext.video_opt->VopInfo.VopTimeIncrementResolution = 90000;
					SendContext.video_opt->VopInfo.FixedVopTimeIncrement = (90000 * 1000)/SendContext.VideoCTSTimeScale;
					if (SendContext.VideoCTSTimeScale == 23976)
						SendContext.video_opt->VopInfo.FixedVopTimeIncrement = (90000 * 1000)/SendContext.VideoCTSTimeScale;
					else if (SendContext.VideoCTSTimeScale == 2997)
						SendContext.video_opt->VopInfo.FixedVopTimeIncrement = (90000 * 100)/SendContext.VideoCTSTimeScale;
					else
						SendContext.video_opt->VopInfo.FixedVopTimeIncrement = 0;
				}
				else {
					/* Method 2. Send system PTS-es not scaled and set fixed time according to the frame rate.*/
					SendContext.video_opt->VopInfo.FixedVopRate = TRUE;
					SendContext.video_vop_tir = SendContext.VideoCTSTimeScale;
					SendContext.video_opt->VopInfo.VopTimeIncrementResolution = SendContext.VideoCTSTimeScale;
					if (SendContext.VideoCTSTimeScale == 23976)
						SendContext.video_opt->VopInfo.FixedVopTimeIncrement = 1000;
					else if (SendContext.VideoCTSTimeScale == 2997)
						SendContext.video_opt->VopInfo.FixedVopTimeIncrement = 100;
					else
						SendContext.video_opt->VopInfo.FixedVopTimeIncrement = 0;
				}
					
				if (SendContext.video_opt->VopInfo.FixedVopTimeIncrement) {
					err = RUASetProperty(dcc_info.pRUA, 
							dcc_info.video_decoder, 
							RMVideoDecoderPropertyID_VopInfo,
							&(SendContext.video_opt->VopInfo), 
							sizeof(SendContext.video_opt->VopInfo), 0 );
					if (RMSUCCEEDED(err)) {
						RMDBGLOG((ENABLE, ">>> video time scale not supported. Set fixed vop rate: %ld / %ld frames per second\n",
							SendContext.video_opt->VopInfo.VopTimeIncrementResolution, SendContext.video_opt->VopInfo.FixedVopTimeIncrement));
					}
					else {
						RMDBGLOG((ENABLE,">>> video time scale not supported. Error setting fixed VOP rate : %d !\n", err));
					}
				}
#endif
			}
		}
	}

	err = RM_OK;

	do {
		//RMuint32 cmd;
		enum RM_PSM_State PlaybackStatus;
		RMuint32 delay;
		RMuint64 movie_timescale;
		
		RMDBGLOG((ENABLE, "do-while\n"));
		
		RMGetMKVMovieTimescale(SendContext.mkvc, &movie_timescale);
		RMDBGLOG((ENABLE, "Value of movie timeScale : %d\n", movie_timescale));
		
		DCCSTCSetTimeResolution(dcc_info.pStcSource, DCC_Stc, SendContext.AudioCTSTimeScale);
		DCCSTCSetTimeResolution(dcc_info.pStcSource, DCC_Video, SendContext.video_vop_tir);
		DCCSTCSetTimeResolution(dcc_info.pStcSource, DCC_Audio, SendContext.audio_vop_tir);
		
		if (SendContext.mkvtV){
			RMGetMKVTrackStartupDelay(SendContext.mkvtV, &delay);
			RMDBGLOG((ENABLE,"Stc video delay : %d\n", delay));
			DCCSTCSetVideoOffset(dcc_info.pStcSource, -delay, movie_timescale);
		}
		
		if (SendContext.mkvtA){
			RMGetMKVTrackStartupDelay(SendContext.mkvtA, &delay);
			RMDBGLOG((ENABLE, "Stc audio delay : %d\n", delay));
			DCCSTCSetAudioOffset(dcc_info.pStcSource, -delay, movie_timescale);
		}
		
		if (SendContext.play_opt->start_pause) {
			RMDBGLOG((ENABLE, "start in pause mode!\n"));
			/* required, because if we do 'next' the decoder *must* be running */
			err = Play(&SendContext, RM_DEVICES_VIDEO, /*DCCVideoPlayIFrame*/DCCVideoPlayFwd);
			if (RMFAILED(err)) {
				fprintf(stderr, "Cannot start decoders %d\n", err);
				goto cleanup;
			}
			
			err = Pause(&SendContext, RM_DEVICES_VIDEO);
			if (RMFAILED(err)) {
				fprintf(stderr, "Cannot pause decoders %d\n", err);
				goto cleanup;
			}
			RM_PSM_SetState(SendContext.PSMcontext, &(SendContext.dcc_info), RM_PSM_Paused);
		}
		else 
			RM_PSM_SetState(SendContext.PSMcontext, &(SendContext.dcc_info), RM_PSM_Playing);

//		SendContext.play_opt->start_pause = FALSE;
		

	mainloop:

		RMDBGLOG((ENABLE, "mainloop, seek to zero\n"));

		RMSeekMKVmillisec(SendContext.mkvc, 0);

		DCCSTCSetTime(dcc_info.pStcSource, SendContext.stc_offset_ms*((RMint64)(SendContext.AudioCTSTimeScale/1000)), SendContext.AudioCTSTimeScale);

	mainloop_noseek:
		RMDBGLOG((ENABLE, "mainloop noseek\n"));

		SendContext.FirstSystemTimeStamp = TRUE;

		SendContext.sendVideoDSI = TRUE;

		SendContext.meanCounter = 0;

		SendContext.videoDataSent = 0;
		SendContext.videoBitrateCounter = 0;

		SendContext.audioDataSent = 0;

		SendContext.videoEOS = FALSE;
		SendContext.audioEOS = FALSE;

		SendContext.lastVideoPTS = 0;
		SendContext.lastAudioPTS = 0;

		SendContext.prebufferingDone = FALSE;

		SendContext.videoTransferPending = FALSE;
		SendContext.audioTransferPending = FALSE;
		
		SendContext.isIFrameMode = FALSE;
		SendContext.IFrameDirection = 0;
		SendContext.isTrickMode = FALSE;
		

		flush_and_release_buffers(&SendContext);

		RMDBGLOG((ENABLE, "available buffer count: %lu\n", RUAGetAvailableBufferCount(SendContext.pDMA)));

		if ((!SendContext.SendVideoPts) || (!SendContext.SendAudioPts))
			SendContext.forceTimersToZero = TRUE;


		PlaybackStatus = RM_PSM_GetState(SendContext.PSMcontext, &(SendContext.dcc_info));
		if ((PlaybackStatus != RM_PSM_Paused) && (PlaybackStatus != RM_PSM_Stopped)) {
			RMDBGLOG((ENABLE, "setting play state\n"));
			RM_PSM_SetState(SendContext.PSMcontext, &(SendContext.dcc_info), RM_PSM_Playing);	
		}
		else {
			RMDBGLOG((ENABLE, "either paused or stopped\n"));
			PROCESS_KEY(FALSE, TRUE);
		}

		
		DCCSTCSetSpeed(dcc_info.pStcSource, SendContext.play_opt->speed_N, SendContext.play_opt->speed_M);

		
                // required, because if we start straight into pause mode, we cant send buffers (weird)
		Play(&SendContext, RM_DEVICES_STC | RM_DEVICES_AUDIO | RM_DEVICES_VIDEO, DCCVideoPlayFwd);

		/* do prebufferization only when in playing state */
		if (RM_PSM_GetState(SendContext.PSMcontext, &(SendContext.dcc_info)) == RM_PSM_Playing) {

			Pause(&SendContext, RM_DEVICES_STC | RM_DEVICES_VIDEO | RM_DEVICES_AUDIO);
		
			RM_PSM_SetState(SendContext.PSMcontext, &(SendContext.dcc_info), RM_PSM_Prebuffering);
		}



#ifdef WITH_MONO
		RMDCCInfo(&dcc_info); // pass DCC context to application
#endif

	start_sending_data:
		RMDBGLOG((ENABLE, "start sending data\n"));

		/* wake up disks if necessary */
		switch (SendContext.play_opt->disk_ctrl_state) {
		case DISK_CONTROL_STATE_DISABLE:
		case DISK_CONTROL_STATE_RUNNING:
			break;
		case DISK_CONTROL_STATE_SLEEPING:
			if(SendContext.play_opt->disk_ctrl_callback && SendContext.play_opt->disk_ctrl_callback(DISK_CONTROL_ACTION_RUN) == RM_OK)
				SendContext.play_opt->disk_ctrl_state = DISK_CONTROL_STATE_RUNNING;
			break;
		}

		//SendContext.mkvtA = NULL;
		//SendContext.mkvtV = NULL;

		while (1) { // additional 'while' used to take care of commands issued during EOSWait
			RMbool sendAudio;
			RMbool sendVideo;

			sendAudio = (SendContext.mkvtA)   ? TRUE : FALSE;
			sendVideo = (SendContext.mkvtV)   ? TRUE : FALSE;

			while (1) {
				RMuint32 sizeBuffer = (1 << SendContext.dmaBufferSizeLog2);
				RMuint32 trackToSend;
				
				PROCESS_KEY(FALSE, TRUE);

				update_hdmi(&dcc_info, SendContext.disp_opt, &(SendContext.audio_opt[0]));

				// resume discs
				switch (SendContext.play_opt->disk_ctrl_state) {
				case DISK_CONTROL_STATE_DISABLE:
				case DISK_CONTROL_STATE_RUNNING:
					break;
				case DISK_CONTROL_STATE_SLEEPING: 
					{
						enum RM_PSM_State FSMstate = RM_PSM_GetState(SendContext.PSMcontext, &(SendContext.dcc_info));
						if ((FSMstate != RM_PSM_Stopped) || (FSMstate != RM_PSM_Paused)) {
							RMDBGLOG((ENABLE, "warning!!, disk is suspended but playback is not (status %lu)\n", (RMuint32)FSMstate));
						}

						if(SendContext.play_opt->disk_ctrl_callback && SendContext.play_opt->disk_ctrl_callback(DISK_CONTROL_ACTION_RUN) == RM_OK)
							SendContext.play_opt->disk_ctrl_state = DISK_CONTROL_STATE_RUNNING;
					}
				}


				trackToSend = check_interleave(&SendContext);

				RMDBGLOG((DBG, "videoBuffer %p used %lu/%lu : audioBuffer %p used %lu : SCbuffer %p used %lu\n",
					  SendContext.videoDMABuffer,
					  SendContext.videoDMABufferOffset,
					  sizeBuffer,
					  SendContext.audioDMABuffer,
					  SendContext.audioDMABufferOffset,
					  SendContext.startCodeDMABuffer,
					  SendContext.startCodeDMABufferOffset));

				if (trackToSend == SEND_VIDEO_TRACK) {
					RMDBGLOG((INTERLEAVE_DBG, "sendVideo\n"));

					if (!SendContext.videoDMABuffer) {
						get_buffer(&SendContext, &SendContext.videoDMABuffer);
						if (SendContext.processKeyResultValid) {
							SendContext.processKeyResultValid = FALSE;
							switch(SendContext.processKeyResult) {
							case LABEL_ERROR:
							case LABEL_CLEANUP:
								goto cleanup;
							case LABEL_MAINLOOP:
								goto mainloop;
							case LABEL_MAINLOOP_NOSEEK:
								goto mainloop_noseek;
							case LABEL_START_SENDING_DATA:
								goto start_sending_data;
							case LABEL_SIGNAL_EOS_AND_EXIT:
								goto signal_EOS_and_exit_loop;
							case LABEL_NONE:
								RMDBGLOG((ENABLE, "warning unrecognised key pressed\n"));
								break;
							}
						}
						SendContext.videoDMABufferOffset = 0;
						RMDBGLOG((DBG, "setting new buffer for video 0x%lx\n", (RMuint32)SendContext.videoDMABuffer));
					}

					while (send_MKV_video(&SendContext) == RM_PENDING) {
						// because a key might have been pressed while waiting for a H264 buffer
						if (SendContext.processKeyResultValid) {
							SendContext.processKeyResultValid = FALSE;
							switch(SendContext.processKeyResult) {
							case LABEL_ERROR:
							case LABEL_CLEANUP:
								goto cleanup;
							case LABEL_MAINLOOP:
								goto mainloop;
							case LABEL_MAINLOOP_NOSEEK:
								goto mainloop_noseek;
							case LABEL_START_SENDING_DATA:
								goto start_sending_data;
							case LABEL_SIGNAL_EOS_AND_EXIT:
								goto signal_EOS_and_exit_loop;
							case LABEL_NONE:
								RMDBGLOG((ENABLE, "warning unrecognised key pressed\n"));
								break;
							}
						}

						check_prebuf_state(&SendContext, 0);
						
						switch (SendContext.play_opt->disk_ctrl_state) {
						case DISK_CONTROL_STATE_DISABLE:
						case DISK_CONTROL_STATE_SLEEPING:
							break;
						case DISK_CONTROL_STATE_RUNNING:
							if(SendContext.play_opt->disk_ctrl_callback && SendContext.play_opt->disk_ctrl_callback(DISK_CONTROL_ACTION_SLEEP) == RM_OK)
								SendContext.play_opt->disk_ctrl_state = DISK_CONTROL_STATE_SLEEPING;
							break;
						}

						PROCESS_KEY(TRUE, TRUE);
					}

					if (SendContext.videoEOS)
						sendVideo = FALSE;
					else {
						
						if (SendContext.videoDMABufferOffset + DMABUFFER_UNUSED_BLOCK_SIZE >= sizeBuffer) {
							RMDBGLOG((DBG, "release buffer 0x%lx video %lu\n", (RMuint32)SendContext.videoDMABuffer, SendContext.videoDMABufferOffset));
							RUAReleaseBuffer(SendContext.pDMA, SendContext.videoDMABuffer);
							SendContext.videoDMABuffer = NULL;
							SendContext.videoDMABufferOffset = 0;
						}
						else
							continue;
					}
				}
				else if (trackToSend == SEND_AUDIO_TRACK) {
					RMuint32 nextSampleSize;

					RMDBGLOG((INTERLEAVE_DBG, "\tsendAudio\n"));

					if (!SendContext.audioDMABuffer) {
						get_buffer(&SendContext, &SendContext.audioDMABuffer);
						if (SendContext.processKeyResultValid) {
							SendContext.processKeyResultValid = FALSE;
							switch(SendContext.processKeyResult) {
							case LABEL_ERROR:
							case LABEL_CLEANUP:
								goto cleanup;
							case LABEL_MAINLOOP:
								goto mainloop;
							case LABEL_MAINLOOP_NOSEEK:
								goto mainloop_noseek;
							case LABEL_START_SENDING_DATA:
								goto start_sending_data;
							case LABEL_SIGNAL_EOS_AND_EXIT:
								goto signal_EOS_and_exit_loop;
							case LABEL_NONE:
								RMDBGLOG((ENABLE, "warning unrecognised key pressed\n"));
								break;
							}
						}
						SendContext.audioDMABufferOffset = 0;
						RMDBGLOG((DBG, "setting new buffer for audio 0x%lx\n", (RMuint32)SendContext.audioDMABuffer));
					}

					while (send_MKV_audio(&SendContext) == RM_PENDING) {
						check_prebuf_state(&SendContext, 0);
						
						switch (SendContext.play_opt->disk_ctrl_state) {
						case DISK_CONTROL_STATE_DISABLE:
						case DISK_CONTROL_STATE_SLEEPING:
							break;
						case DISK_CONTROL_STATE_RUNNING:
							if(SendContext.play_opt->disk_ctrl_callback && SendContext.play_opt->disk_ctrl_callback(DISK_CONTROL_ACTION_SLEEP) == RM_OK)
								SendContext.play_opt->disk_ctrl_state = DISK_CONTROL_STATE_SLEEPING;
							break;
						}

						PROCESS_KEY(TRUE, TRUE);
					}

					if (SendContext.audioEOS) 
						sendAudio = FALSE;
					else {

						RMGetMKVTrackNextSampleSize(SendContext.mkvtA, &nextSampleSize);
						
						if (SendContext.audioDMABufferOffset + nextSampleSize + ADTS_HEADER_SIZE > sizeBuffer) {
							RMDBGLOG((DBG, "release buffer 0x%lx audio %lu\n", (RMuint32)SendContext.audioDMABuffer, SendContext.audioDMABufferOffset));
							RUAReleaseBuffer(SendContext.pDMA, SendContext.audioDMABuffer);
							SendContext.audioDMABuffer = NULL;
							SendContext.audioDMABufferOffset = 0;
						}
						else
							continue;
					}

				}

				if (SendContext.videoEOS)
					sendVideo = FALSE;
				if (SendContext.audioEOS)
					sendAudio = FALSE;

#ifdef _DEBUG				
				PlaybackStatus = RM_PSM_GetState(SendContext.PSMcontext, &(SendContext.dcc_info));
				RMDBGLOG((DBG, "available buffer count: %lu\n", RUAGetAvailableBufferCount(SendContext.pDMA)));
				RMDBGLOG((DBG, "playBackState %lu\n", ((RMuint32)PlaybackStatus + 1) * 10));
#endif

				
				PROCESS_KEY(TRUE, TRUE);

				RMDBGLOG((DBG, "eos detect: video %lu, audio %lu\n",
					  sendVideo,
					  sendAudio));
		
				// EOS detection
				if (!SendContext.isAudioOnly) {
					if (((!sendVideo) && (!sendAudio)) 
					    ||
					    ((!sendVideo) &&
					     ((RM_PSM_GetState(SendContext.PSMcontext, &(SendContext.dcc_info)) != RM_PSM_Playing) &&
					      (RM_PSM_GetState(SendContext.PSMcontext, &(SendContext.dcc_info)) != RM_PSM_Prebuffering)))) {
						RMDBGLOG((ENABLE, "EOS!\n"));
						break;
					}
				}
				else {
					RMuint64 currentTime;

					DCCSTCGetTime(dcc_info.pStcSource, &currentTime, SendContext.audio_vop_tir);
				
					if (currentTime > SendContext.lastSTC + SendContext.audio_vop_tir) {
						SendContext.lastSTC = currentTime;
						currentTime /= SendContext.audio_vop_tir;
						if (SendContext.Duration)
							RMDBGLOG((ENABLE, "time = %llu/%llu secs (%lld/100)\n", currentTime, SendContext.Duration, (currentTime * 100)/SendContext.Duration));
						else
							RMDBGLOG((ENABLE, "time = %llu s\n", currentTime));
						if (currentTime > SendContext.Duration) {
							RMDBGLOG((ENABLE, "reached end, EOS!\n"));
							break;
						}
					}
					if (SendContext.audioEOS) {
						RMDBGLOG((ENABLE, "audio EOS (2)\n"));
						break;
					}

					PlaybackStatus = RM_PSM_GetState(SendContext.PSMcontext, &(SendContext.dcc_info));
					if ((PlaybackStatus == RM_PSM_IRewind) || (PlaybackStatus == RM_PSM_Fast) || (PlaybackStatus == RM_PSM_Slow))
						;//usleep(100000); // to prevent high CPU usage
                }
				
#if 1
                gettimeofday (&current, NULL);
                if ((current.tv_sec - begin.tv_sec) > 1200) {
                    printf ("Evaluation time expired, we are about to exit!");
                    break;
                }
#endif
			}

			check_prebuf_state(&SendContext, 0);

			switch (SendContext.play_opt->disk_ctrl_state) {
			case DISK_CONTROL_STATE_DISABLE:
			case DISK_CONTROL_STATE_SLEEPING:
				break;
			case DISK_CONTROL_STATE_RUNNING:
				if(SendContext.play_opt->disk_ctrl_callback && SendContext.play_opt->disk_ctrl_callback(DISK_CONTROL_ACTION_SLEEP) == RM_OK)
					SendContext.play_opt->disk_ctrl_state = DISK_CONTROL_STATE_SLEEPING;
				break;
			}

			PROCESS_KEY(FALSE, TRUE);

		wait_eos:
			fprintf(stderr, "File ready %ld times, waiting for EOS\n", SendContext.Ntimes+1);
			err = WaitForEOS(&SendContext, &SendContext.actions);
			{
				RMuint64 stc;
				DCCSTCGetTime(dcc_info.pStcSource, &stc, 90000);
				
				RMDBGLOG((ENABLE, "Timer duration %llu ms\n", stc/90));
			}
			if (err == RM_KEY_WHILE_WAITING_EOS) {
				RMDBGLOG((ENABLE, "command %lu while waiting for EOS\n", SendContext.actions.cmd));
				err = RM_OK;
				PROCESS_KEY(FALSE, FALSE);
				goto wait_eos;
			}
			else {
				if ((RM_PSM_GetState(&PSMContext, &(SendContext.dcc_info)) == RM_PSM_IRewind) && 0) {
					//restart playback
					goto restart_loop;
				}

			signal_EOS_and_exit_loop:
#ifdef WITH_MONO
				/* callback to signal EOS to curacao/mono */
				RMEOSCallback(); 
				break;
#else
				break;	// EOS
#endif
			}

		}
		
		if (SendContext.play_opt->loop_count > 0)
			SendContext.play_opt->loop_count --;
	restart_loop:

		/* if there is another loop, stop devices */
		if ((SendContext.play_opt->loop_count > 0) || (SendContext.play_opt->waitexit != TRUE) || (SendContext.play_opt->infinite_loop))
			Stop(&SendContext, RM_DEVICES_STC | RM_DEVICES_AUDIO | RM_DEVICES_VIDEO);

		SendContext.Ntimes++;
	} while ((SendContext.play_opt->loop_count > 0) || (SendContext.play_opt->infinite_loop));

 cleanup:
	
	error = 0; //normal exit

	if( SendContext.play_opt->waitexit ) {
		RMascii key;
		Stop(&SendContext, RM_DEVICES_STC);
		fprintf(stderr, "press q key again if you really want to stop & quit\n");
		while ( !(RMGetKeyNoWait(&key) && ((key == 'q') || (key =='Q'))) );
		Stop(&SendContext, RM_DEVICES_STC | RM_DEVICES_AUDIO | RM_DEVICES_VIDEO);
	}

 exit_with_error:

#ifndef WITH_MONO
    RMTermExit();
#endif

	if (err != RM_OK) {
		fprintf(stderr, "quitting due to error %s (%d)...\n", RMstatusToString(err), err);
		error = err; //exit with error
	}
			 
	RMDBGLOG((ENABLE, "closing...\n"));

#ifdef WITH_MONO
	RMDCCInfo(NULL); // invalidate DCC context
#endif


	RMDBGLOG((ENABLE, "sent %lu bytes of video, %lu of audio\n", 
		  SendContext.videoDataSent,
		  SendContext.audioDataSent));

/*
	if (SendContext.streamInfo.data.mkvInfo.chapters) {
		RMuint32 i;

		for (i = 0; i < SendContext.streamInfo.data.mkvInfo.chapters; i++) {
			
			RMDBGLOG((ENABLE, "free entry %lu, '%s'\n", i, SendContext.streamInfo.data.mkvInfo.chapterList[i].name));
			RMFree(SendContext.streamInfo.data.mkvInfo.chapterList[i].name);
		}
	}
*/
	err = close_save_files(SendContext.play_opt);
	if (RMFAILED(err)) {
		RMDBGLOG((ENABLE, "Cannot close files used to save data %d\n", err));
		error = -1;
	}

	if (SendContext.mkvc) {
		if (SendContext.mkvtA) {
			RMDBGLOG((ENABLE, "closing audio track\n"));
			RMCloseMKVTrack(SendContext.mkvc, SendContext.mkvtA);
		}
		if (SendContext.mkvtV) {
			RMDBGLOG((ENABLE, "closing video track\n"));
			RMCloseMKVTrack(SendContext.mkvc, SendContext.mkvtV);
		}
		RMDBGLOG((ENABLE, "closing mkv client\n"));
		RMCloseMKV(SendContext.mkvc);
	}

	if (SendContext.mkv_file) {
		RMDBGLOG((ENABLE, "closing file\n"));
		RMCloseFile(SendContext.mkv_file);
	}
	
	if (SendContext.dcc_info) {
		RMDBGLOG((ENABLE, "stopping devices\n"));
		if (SendContext.dcc_info->pStcSource)
			Stop(&SendContext, RM_DEVICES_STC);
		if (SendContext.dcc_info->pVideoSource)
			Stop(&SendContext, RM_DEVICES_VIDEO);
		if (SendContext.dcc_info->pMultipleAudioSource)
			Stop(&SendContext, RM_DEVICES_AUDIO);
	}
		
	if (SendContext.dmabuffer_array) {
		RMuint32 i;
		for (i = 0; i < SendContext.dmabuffer_index; i++) {
			RUAReleaseBuffer(SendContext.pDMA, (RMuint8 *)SendContext.dmabuffer_array[i]);
			RMDBGLOG((ENABLE, "released buffer[%lu] @ 0x%08lx\n", i, SendContext.dmabuffer_array[i]));
		}
		RMFree(SendContext.dmabuffer_array);
		SendContext.dmabuffer_index = 0;
		SendContext.dmabuffer_array = NULL;
	}


	if ((SendContext.SendVideoData) && (dcc_info.pVideoSource)) {
		RMDBGLOG((ENABLE, "closing video source\n"));
		err = DCCCloseVideoSource(dcc_info.pVideoSource);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Error cannot close video decoder %d\n", err));
			error = -1;
		}
	}

	
	if ((SendContext.SendAudioData) && (dcc_info.pMultipleAudioSource)) {
		RMDBGLOG((ENABLE, "closing audio source\n"));
		err = DCCCloseMultipleAudioSource(dcc_info.pMultipleAudioSource);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Error cannot close audio decoder %d\n", err));
			error = -1;
		}
	}

	if (SendContext.pDMA) {
		RMDBGLOG((ENABLE, "closing DMApool\n"));
		err = RUAClosePool(SendContext.pDMA);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Error cannot close dmapool %d\n", err));
			error = -1;
		}
	}

	
	if (dcc_info.pStcSource) {
		RMDBGLOG((ENABLE, "closing STC\n"));
		err = DCCSTCClose(dcc_info.pStcSource);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Error cannot close STC %d\n", err));
			error = -1;
		}
	}
#ifndef WITH_MONO		
	RMDBGLOG((ENABLE, "clear display options\n"));
	clear_display_options(&dcc_info, SendContext.disp_opt);

	if (dcc_info.pDCC) {
		RMDBGLOG((ENABLE, "closing DCC\n"));
		err = DCCClose(dcc_info.pDCC);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Cannot close DCC %d\n", err));
			error = -1;
		}
	}

	if (dcc_info.pRUA) {
		RMDBGLOG((ENABLE, "closing RUA\n"));
		err = RUADestroyInstance(dcc_info.pRUA);
		if (RMFAILED(err)) {
			RMDBGLOG((ENABLE, "Cannot destroy RUA instance %d\n", err));
			error = -1;
		}
	}

    RMDBGLOG((ENABLE, "Realy exit!\n"));

#endif /*WITH_MONO*/
	if (error)
		RMDBGLOG((ENABLE, "exiting with error (%d)\n", error));

//    exit (EXIT_SUCCESS);
	return 0;
}
