/*
 *
 *
 *  
 */

#define RM_LIBRARY_SELF_COMPILING 1

#include <string>
#include <unistd.h>
#include "rmmkvapi.h"

void* MKVDemuxThreadFunc (void *pargs) {
    RMmkvClient *mkvc = (RMmkvClient *)pargs;
    matroska_demux_c *pdemux = mkvc->pdemux;
    
    RMASSERT((mkvc != NULL) && (pdemux != NULL));

    fprintf (stderr, "%s ready to run..., pid = %u, ppid = %u\n", __FUNCTION__, (uint32_t)getpid(), (uint32_t)pthread_self());

    while (TRUE == mkvc->running) {
        switch (mkvc->state) {
        case DEMUX_STATE_PLAY:
            pdemux->Demux ();
            break;
        case DEMUX_STATE_READY:
            break;
        case DEMUX_STATE_FINISH:
            break;
        case DEMUX_STATE_STOP:
            break;
        default:
            break;
        }
    }

    pthread_exit (NULL);
    fprintf (stderr, "%s ended!\n", __FUNCTION__);
    return NULL;
}

RMstatus RMOpenFileMKV(const char *filename, ExternalRMmkvClient *mkvc)
{
	RMmkvClient *p_ret = NULL;
	matroska_demux_c *pdemux = NULL;
    
    string file_name(filename);

	if (NULL == (p_ret=(RMmkvClient *)malloc (sizeof (RMmkvClient)))) {
		fprintf (stderr, "Error allocate memory for client!\n");
		return RM_ERROR;
	}

    p_ret->video_cache.buffer = NULL;
    p_ret->video_cache.buffer_size = 4*1024*1024 - 1024;

    p_ret->audio_cache.buffer = NULL;
    p_ret->audio_cache.buffer_size = 256*1024 - 1024;
    
    // Open the demux module
	pdemux = new matroska_demux_c ();
	if (pdemux) {
        pdemux->Open (file_name);
        pdemux->PreparePlayback ();

        p_ret->pdemux = pdemux;
	}

    // Startup the demux thread
//    RMASSERT(p_ret->demux_thread == NULL);
    p_ret->running = TRUE;
    p_ret->state = DEMUX_STATE_PLAY;

    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_create (&p_ret->demux_thread, &attr, MKVDemuxThreadFunc, p_ret);
    //p_ret->demux_thread = RMCreateThread ("MKVDemuxThread", MKVDemuxThreadFunc, p_ret);
    //if (p_ret->demux_thread == NULL) {
    //    pdemux->Close ();
    //delete pdemux;

//        free (p_ret);
    //      *mkvc = NULL;
    //  return RM_ERROR;
    //}

    *mkvc = p_ret;

	return RM_OK;
}

RMstatus RMOpenExternalFileMKV(RMfile file, ExternalRMmkvClient *mkvc)
{
	RMstatus status = RM_OK;

	return status;
}

RMstatus RMSetAVTrackCacheMKV(ExternalRMmkvClient mkvc, MKVTrackCacheBuffer *vcache, MKVTrackCacheBuffer *acache) {
    RMstatus status = RM_OK;

    if (NULL != vcache) {
        mkvc->video_cache.buffer = vcache->buffer;
        mkvc->video_cache.buffer_size = vcache->buffer_size;
    }
    if (NULL != acache) {
        mkvc->audio_cache.buffer = acache->buffer;
        mkvc->audio_cache.buffer_size = acache->buffer_size;
    }

    return status;
}

/*
 * Seek to time mesaured in seconds
 */

RMreal RMSeekMKV(ExternalRMmkvClient mkvc, RMreal time)
{
	uint64_t timecode, timescale;
    uint64_t ret = 0;

    timescale = mkvc->pdemux->MovieTimecodeScale ();
	timecode = (uint64_t)(time)*timescale;

    mkvc->state = DEMUX_STATE_PAUSE;

    ret = mkvc->pdemux->Seek (timecode);

    mkvc->state = DEMUX_STATE_PLAY;

    mkvc->pdemux->pre_buffer (100.0/100.0);

	return (RMreal)(ret/timescale);
}

/*
 * Seek to time measured in mini seconds
 */
RMuint32 RMSeekMKVmillisec(ExternalRMmkvClient mkvc, RMuint32 time_ms)
{
	uint64_t timecode, timescale;
    uint64_t ret = 0;
    
    timescale = mkvc->pdemux->MovieTimecodeScale ();
	timecode = time_ms*timescale/1000;

    mkvc->state = DEMUX_STATE_PAUSE;

    fprintf (stderr, "%s %d: seek to %d, timescale = %lld, timecode = %lld!\n", __FUNCTION__, __LINE__, time_ms, timescale, timecode);
    ret = mkvc->pdemux->Seek (timecode);

    mkvc->state = DEMUX_STATE_PLAY;

    mkvc->pdemux->pre_buffer (100.0/100.0);
    
	return (ret*1000/timescale);
}

void RMPrintMKV(ExternalRMmkvClient mkvc)
{
	mkvc->pdemux->DumpTracks ();
}

RMuint32 RMGetMKVVideoTrackID(ExternalRMmkvClient mkvc)
{
	return mkvc->pdemux->GetVideoTrackID (); 
}

RMuint32 RMGetMKVAudioTrackID(ExternalRMmkvClient mkvc)
{
	return mkvc->pdemux->GetAudioTrackID ();
}

ExternalRMmkvTrack RMOpenMKVTrack(ExternalRMmkvClient mkvc, RMuint32 trackID)
{
	ExternalRMmkvTrack ret_track = NULL;
    matroska_track_c *ptrack = mkvc->pdemux->GetTrackByID (trackID);

    if (ptrack != NULL) {
        switch (ptrack->m_track_type) {
        case MATROSKA_TRACK_TYPE_VIDEO:
            ptrack->SetCacheBuffer (mkvc->video_cache.buffer, mkvc->video_cache.buffer_size);
            break;
        case MATROSKA_TRACK_TYPE_AUDIO:
            ptrack->SetCacheBuffer (mkvc->audio_cache.buffer, mkvc->audio_cache.buffer_size);
            break;
        default:
            break;
        }
        
        mkvc->pdemux->OpenTrack (ptrack);
        ret_track = (ExternalRMmkvTrack)malloc (sizeof (RMmkvTrack));
        if (!ret_track)
            return NULL;

        ret_track->pdemux = mkvc->pdemux;
        ret_track->ptrack = ptrack;
    }

    return ret_track;
}

void RMCloseMKVTrack(ExternalRMmkvClient mkvc, ExternalRMmkvTrack mkvt)
{
    assert ((mkvc != NULL) && (mkvt != NULL));
    mkvc->pdemux->CloseTrack (mkvt->ptrack);
    free (mkvt);
    mkvt = NULL;
}

void RMCloseMKV(ExternalRMmkvClient mkvc)
{
    assert (mkvc != NULL);
    mkvc->running = FALSE;
    mkvc->state = DEMUX_STATE_FINISH;

    mkvc->pdemux->Close ();

    pthread_join (mkvc->demux_thread, NULL);

    delete mkvc->pdemux;

    free (mkvc);

    mkvc = NULL;
}

void RMPrintMKVTrack(ExternalRMmkvTrack mkvt)
{

}


void RMDumpMKVTrack(ExternalRMmkvTrack mkvt, const RMuint8 *dumpname)
{

}


RMuint64 RMGetMKVTrackTimeScale(ExternalRMmkvTrack mkvt)
{
    RMuint64 timescale;
    
    timescale = (RMuint64)(mkvt->pdemux->MovieTimecodeScale () * mkvt->ptrack->TrackTimecodeScale ());
	return (RMuint64)timescale;
}


RMuint32 RMGetNextMKVSampleSize(ExternalRMmkvTrack mkvt)
{
    size_t size;
    mkvt->pdemux->track_get_next_framesize (mkvt->ptrack, size);

	return size;
}


RMbool RMGetNextMKVSample(ExternalRMmkvTrack mkvt, RMmkvSample *mkvs, RMuint32 maxSize)
{
    frame_c frame;
    frame.m_pdata = mkvs->buf;
    frame.m_size = maxSize;

    mkvt->pdemux->track_get_next_frame (mkvt->ptrack, frame);

    mkvs->flags = 0;
    mkvs->buf = frame.m_pdata;
    mkvs->size = frame.m_size;

    if (FRAME_FLAG_FRAME_START == (frame.m_flag & FRAME_FLAG_FRAME_START))
        mkvs->flags |= MKV_AU_START;
    
    if (FRAME_FLAG_FRAME_END == (frame.m_flag & FRAME_FLAG_FRAME_END))
        mkvs->flags |= MKV_AU_END;
    
    if (FRAME_FLAG_PTS_VALID == (frame.m_flag & FRAME_FLAG_PTS_VALID)) {
        //fprintf (stderr, "%s: pts = %lld!\n", __FUNCTION__, frame.m_pts);
        frame.m_pts /= 1000000;
        mkvs->flags |= MKV_CTS_VALID;
        mkvs->CTS_LSB = frame.m_pts & 0xFFFFFFFF;
        mkvs->CTS_MSB = (frame.m_pts >> 32) & 0xFFFFFFFF;
        mkvs->DTS_LSB = frame.m_pts & 0xFFFFFFFF;
        mkvs->DTS_MSB = (frame.m_pts >> 32) & 0xFFFFFFFF;
    }

    // Check eof
    if (MKV_RT_EOF == mkvt->pdemux->Eof ())
        return FALSE;

	return TRUE;
}



RMbool RMGetNextMKVRandomAccessSample(ExternalRMmkvTrack mkvt, RMmkvSample *mkvs, RMuint32 maxSize)
{ 
	return FALSE;
}

     
RMbool RMGetPrevMKVRandomAccessSample(ExternalRMmkvTrack mkvt, RMmkvSample *mkvs, RMuint32 maxSize)
{
	return FALSE;
}

RMuint8 *RMGetMKVTrackDSI(ExternalRMmkvTrack mkvt, RMuint32 *size)
{
    size_t dsiSize = 0;
    RMuint8* dsiBuffer = NULL;

    dsiBuffer = mkvt->ptrack->TrackDSI (dsiSize);
    
    if (dsiSize > 0) {
        *size = dsiSize;
        return dsiBuffer;
    }

	return NULL;
}

RMuint32 RMGetMKVTrackDuration(ExternalRMmkvTrack mkvt)
{
	return mkvt->pdemux->GetMovieDuration ();
}

RMuint32 RMGetMKVTrackSampleCount(ExternalRMmkvTrack mkvt)
{

	return 0;
}

RMuint32 RMGetMKVTrackRandomAccessSampleCount(ExternalRMmkvTrack mkvt)
{

	return 0;
}

RMuint32 RMGetMKVTrackVopTimeIncrement(ExternalRMmkvTrack mkvt)
{
	return 0;
}

RMuint32 RMGetMKVNumberOfVideoTracks(ExternalRMmkvClient mkvc) {
	return mkvc->pdemux->NumberOfVideoTracks ();
}

RMuint32 RMGetMKVNumberOfAudioTracks(ExternalRMmkvClient mkvc) {
	return mkvc->pdemux->NumberOfAudioTracks ();;
}

RMuint32 RMGetMKVNumberOfSPUTracks(ExternalRMmkvClient mkvc) {
    return 0;
}

RMuint32 RMGetMKVNumberOfSubtitleTracks(ExternalRMmkvClient mkvc) {
	return mkvc->pdemux->NumberOfSubtitleTracks ();
}

RMstatus RMGetMKVVideoTrackIDByIndex(ExternalRMmkvClient mkvc, RMuint32 index, RMuint32 *trackID) {
    *trackID = mkvc->pdemux->GetAudioTrackID ();

    //*trackID = mkvc->pdemux->GetVideoTrackIDByIndex (index);
    if (*trackID > 0)
        return RM_OK;

	return RM_ERROR;
}

RMstatus RMGetMKVAudioTrackIDByIndex(ExternalRMmkvClient mkvc, RMuint32 index, RMuint32 *trackID) {
    *trackID = mkvc->pdemux->GetAudioTrackIDByIndex (index);
    if (*trackID > 0)
        return RM_OK;

	return RM_ERROR;
}

RMstatus RMGetMKVSPUTrackIDByIndex(ExternalRMmkvClient mkvc, RMuint32 index, RMuint32 *trackID) {
    *trackID = mkvc->pdemux->GetSPUTrackIDByIndex (index);
    if (*trackID > 0)
        return RM_OK;

	return RM_ERROR;
}

RMstatus RMGetMKVSubtitleTrackIDByIndex(ExternalRMmkvClient mkvc, RMuint32 index, RMuint32 *trackID) {
    *trackID = mkvc->pdemux->GetSubtitleTrackIDByIndex (index);
    if (*trackID > 0)
        return RM_OK;

	return RM_ERROR;
}

RMstatus RMGetMKVMovieTimescale(ExternalRMmkvClient mkvc, RMuint64 *timescale) {
	*timescale = mkvc->pdemux->MovieTimecodeScale ();

	return RM_OK;
}

RMuint32 RMSeekMKVTrack(ExternalRMmkvClient mkvc, RMuint32 time_ms, ExternalRMmkvTrack mkvt) {
	uint64_t timecode, timescale;
    uint64_t ret = 0;

    timescale = (uint64_t)(mkvc->pdemux->MovieTimecodeScale () * mkvt->ptrack->TrackTimecodeScale ());

    timecode = (uint64_t)(time_ms/1000)*timescale;

    mkvc->state = DEMUX_STATE_PAUSE;

    ret = mkvc->pdemux->Seek (timecode);

    mkvc->state = DEMUX_STATE_PLAY;

//    mkvt->pdemux->track_pre_buffer (mkvt->ptrack, 100.0/100.0);

	return (ret*1000/timescale);
}

RMstatus RMGetMKVTrackWidth(ExternalRMmkvTrack mkvt, RMuint32 *width) {
	*width = mkvt->ptrack->TrackWidth ();
	
	return RM_OK;
}

RMstatus RMGetMKVTrackHeight(ExternalRMmkvTrack mkvt, RMuint32 *height) {
	*height = mkvt->ptrack->TrackHeight ();
	
	return RM_OK;
}

RMstatus RMGetMKVTrackSize(ExternalRMmkvTrack mkvt, RMuint32 *sizeTrack, RMuint32 *sampleCount) {
	
	return RM_OK;
}

RMstatus RMGetMKVTrackNextSampleSize(ExternalRMmkvTrack mkvt, RMuint32 *size) {
	return RM_OK;
}

RMstatus RMGetMKVTrackNextReadPos(ExternalRMmkvTrack mkvt, RMuint64 *nextReadPos) {
	return RM_OK;
}

RMstatus RMGetMKVTrackStartupDelay(ExternalRMmkvTrack mkvt, RMuint32 *startup_delay) {
    *startup_delay = 0;
	return RM_OK;
}

RMstatus RMGetMKVTrackType(ExternalRMmkvTrack mkvt, RMmkvTrackType *type) {
	*type = RM_MKV_TYPE_TRACK_NOT_DEFINED;

    const char *codecid = mkvt->ptrack->m_codec_id.c_str ();
	fprintf (stderr, "%s %d: codecid = %s\n", __FUNCTION__, __LINE__, codecid);	
    if (starts_with_case (codecid, MATROSKA_CODEC_ID_V_MPEG4_SP)
        || starts_with_case (codecid, MATROSKA_CODEC_ID_V_MPEG4_ASP)
        || starts_with_case (codecid, MATROSKA_CODEC_ID_V_MPEG4_AP)
        )
        *type = RM_MKV_VIDEO_MPEG4_TRACK;
    else if (starts_with_case (codecid, MATROSKA_CODEC_ID_V_MPEG4_AVC))
        *type = RM_MKV_VIDEO_H264_TRACK;
    else if (starts_with_case (codecid, MATROSKA_CODEC_ID_V_VFW_FOURCC))
        *type = RM_MKV_VIDEO_AVI_TRACK;
    else if (starts_with_case (codecid, MATROSKA_CODEC_ID_V_MSMPEG4V3))
        *type = RM_MKV_VIDEO_DIVX3_TRACK;
    else if (starts_with_case (codecid, MATROSKA_CODEC_ID_V_MPEG1))
        *type = RM_MKV_VIDEO_MPEG1_TRACK;
    else if (starts_with_case (codecid, MATROSKA_CODEC_ID_V_MPEG2))
        *type = RM_MKV_VIDEO_MPEG2_TRACK;
    else if (starts_with_case (codecid, MATROSKA_CODEC_ID_A_AAC))
        *type = RM_MKV_AUDIO_AAC_TRACK;
    else if (starts_with_case (codecid, MATROSKA_CODEC_ID_A_MPEG1_L1)
             || starts_with_case (codecid, MATROSKA_CODEC_ID_A_MPEG1_L2)
             || starts_with_case (codecid, MATROSKA_CODEC_ID_A_MPEG1_L3)) 
        *type = RM_MKV_AUDIO_MP3_TRACK;
    else if (starts_with_case (codecid, MATROSKA_CODEC_ID_A_AC3))
        *type = RM_MKV_AUDIO_AC3_TRACK;
    else if (starts_with_case (codecid, MATROSKA_CODEC_ID_A_PCM_INT_BE)
             || starts_with_case (codecid, MATROSKA_CODEC_ID_A_PCM_INT_LE)
             || starts_with_case (codecid, MATROSKA_CODEC_ID_A_PCM_FLOAT)
        )
        *type = RM_MKV_AUDIO_PCM_TRACK;
    else if (starts_with_case (codecid, MATROSKA_CODEC_ID_A_DTS))
        *type = RM_MKV_AUDIO_DTS_TRACK;
			
    return RM_OK;

	return RM_ERROR;
}

RMstatus RMGetMKVTrackDeinterlaced(ExternalRMmkvTrack mkvt, RMbool *deinterlaced) {
    *deinterlaced = (RMuint32)mkvt->ptrack->TrackDeinterlaced () ? TRUE : FALSE;

	return RM_OK;
}

RMstatus RMGetMKVTrackSampleRate(ExternalRMmkvTrack mkvt, RMuint32 *sampleRate) {
    *sampleRate = (RMuint32)mkvt->ptrack->TrackSamplingFreq ();

	return RM_OK;
}

RMstatus RMGetMKVTrackChannelCount(ExternalRMmkvTrack mkvt, RMuint32 *channelCount) {
//	*channelCount = mkvt->ptrack->audio_track->channels;

	return RM_OK;
}

RMstatus RMGetMKVTrackBitPerSample(ExternalRMmkvTrack mkvt, RMuint32 *bitPerSample) {
//	*bitPerSample = mkvt->ptrack->audio_track->bits_per_sample;

	return RM_OK;
}

RMbool   RMisMKVH264Track(ExternalRMmkvTrack mkvt) {
	return (mkvt->ptrack->IsH264Track()?TRUE:FALSE);
}

RMstatus RMGetMKVH264Level(ExternalRMmkvTrack mkvt, RMuint32 *level) {
     *level = mkvt->ptrack->H264Level ();
	 return RM_OK;
}

RMstatus RMGetMKVH264Profile(ExternalRMmkvTrack mkvt, RMuint32 *profile) {
    *profile = mkvt->ptrack->H264Profile ();
	return RM_OK;
}

RMstatus RMGetMKVH264LengthSize(ExternalRMmkvTrack mkvt, RMuint32 *lengthSize) {
    *lengthSize = mkvt->ptrack->H264LengthSize ();
	return RM_OK;
}

