
#ifndef __CODECS_H__
#define __CODECS_H__

struct ACM_CODEC {
	char			*name;
	unsigned short	id;			// if "zero" the GUID is used
	GUID			guid;
};

struct VFW_CODEC {
	char			*name;
	union {
		unsigned long	dwfcc;
		char			fcc[4];
	};
};

#define ACM_CODEC_LIST_LEN		5
#define VFW_CODEC_LIST_LEN		1
static const ACM_CODEC ACM_CODEC_LIST[ACM_CODEC_LIST_LEN] = {
	{"A_NULL",				0x0000,						{0, 0, 0, 0}},
	{"A_MPEG/L3",			WAVE_FORMAT_MPEGLAYER3,		{0, 0, 0, 0}},
	{"A_AC3",				0x2000,						{0, 0, 0, 0}},
	{"A_PCM/INT/LIT",		WAVE_FORMAT_PCM,			{0, 0, 0, 0}},
	{"A_PCM/FLOAT/IEEE",	WAVE_FORMAT_IEEE_FLOAT,		{0, 0, 0, 0}}
};

/*
static const VFW_CODEC VFW_CODEC_LIST[] = {
	{"MPEG4", {'X','V','I','D'}}
};
*/

#endif __CODECS_H__
