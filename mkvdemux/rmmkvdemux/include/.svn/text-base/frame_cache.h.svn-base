#ifndef __FRAME_CACHE_H
#define __FRAME_CACHE_H

#include "os.h"

#include "common.h"
#include "matroska_common.h"
#include <pthread.h>

#define MAX_FRAME_CACHE_SIZE 4096

#define FRAME_FLAG_FRAME_START 1
#define FRAME_FLAG_PTS_VALID   2
#define FRAME_FLAG_FRAME_END   4

typedef struct frame_t {
    int64_t m_pts;
    int64_t m_duration;
    uint8_t* m_pdata;
    uint32_t m_size;
    uint32_t m_flag;
} frame_c;

class frame_cache_c {
 public:
    frame_cache_c () {
        pthread_mutex_init (&m_lock, NULL);

        m_frame_needed = 0;
        pthread_cond_init (&m_frame_cond, NULL);

        m_space_needed = 0;
        pthread_cond_init (&m_space_cond, NULL);

        m_size = MAX_FRAME_CACHE_SIZE;

        m_head = m_tail = 0;
        memset (m_cache, 0, sizeof (frame_c) * MAX_FRAME_CACHE_SIZE);
    }

    ~frame_cache_c () {
        pthread_mutex_destroy (&m_lock);
        pthread_cond_destroy (&m_frame_cond);
        pthread_cond_destroy (&m_space_cond);
    }

    frame_c *begin ();
    frame_c *end ();
    void pop_front ();
    void pop_back ();

    void push_front (frame_c *pframe);
    void push_back (frame_c *pframe);

    void clear ();
    
    uint32_t size ();

    // This cache will cost 24KB memory,but it's much more costless than deque
    frame_c m_cache[MAX_FRAME_CACHE_SIZE];

 private:
 
    int32_t m_head;
    int32_t m_tail;
    int32_t m_size;

    pthread_mutex_t  m_lock;
    pthread_cond_t   m_frame_cond;
    pthread_cond_t   m_space_cond;
    
    int32_t          m_frame_needed;
    int32_t          m_space_needed;
};

#endif
