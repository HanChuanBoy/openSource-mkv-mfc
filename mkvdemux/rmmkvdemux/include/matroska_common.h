#ifndef __MATROSKA_COMMON_H
#define __MATROSKA_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>

#define MATROSKA_MAX_STREAMS 16
#define MATROSKA_MAX_INDEXES 1024

struct track_spec_t {
    int64_t tid, tuid;
    char *out_name;

    char *sub_charset;
    bool embed_in_ogg;
    bool extract_cuesheet;

    int extract_raw;
    int extract_blockadd_level;

    bool done;
};

#define in_parent(p) \
  (!p->IsFiniteSize() || \
   (in->getFilePointer() < \
    (p->GetElementPosition() + p->HeadSize() + p->GetSize())))

typedef enum {
    MKV_RT_FIRST = 0,
    MKV_RT_SUCCESS,
    MKV_RT_EGENERIC,
    MKV_RT_EEXIT,
    MKV_RT_ENOMEM,
    MKV_RT_EOF,
    MKV_RT_LAST
} MKV_RT;

#endif
