#include "frame_cache.h"

frame_c* frame_cache_c::begin () {
    int num_frame;
    frame_c *pframe = NULL;

    pthread_mutex_lock (&m_lock);

    // Wait until frames available
    do {
        if (m_head >= m_tail)
            num_frame = m_head - m_tail;
        else 
            num_frame = m_head + m_size - m_tail;

        if (num_frame <= 0) {
            m_frame_needed++;
            fprintf (stderr, "More frames are needed, waiting...\n");
            pthread_cond_wait (&m_frame_cond, &m_lock);
            fprintf (stderr, "Frames available, recheck...!\n");
            m_frame_needed--;
            continue;
        } else 
            break;
    } while (1);

    pframe = &m_cache[m_tail];

    pthread_mutex_unlock (&m_lock);
    
    return pframe;
}

frame_c* frame_cache_c::end () {
    int num_frame;
    frame_c *pframe = NULL;

    pthread_mutex_lock (&m_lock);

    // Wait until frames available
    do {
        if (m_head >= m_tail)
            num_frame = m_head - m_tail;
        else 
            num_frame = m_head + m_size - m_tail;

        if (num_frame <= 0) {
            m_frame_needed++;
            pthread_cond_wait (&m_frame_cond, &m_lock);
            m_frame_needed--;
            continue;
        } else 
            break;
    } while (1);

    pframe = &m_cache[m_head - 1];

    pthread_mutex_unlock (&m_lock);
    
    return pframe;
}

void frame_cache_c::pop_front () {
    int num_frame;

    pthread_mutex_lock (&m_lock);

    // Wait until frames available
    do {
        if (m_head >= m_tail)
            num_frame = m_head - m_tail;
        else 
            num_frame = m_head + m_size - m_tail;

        if (num_frame <= 0) {
            m_frame_needed++;
            pthread_cond_wait (&m_frame_cond, &m_lock);
            m_frame_needed--;
            continue;
        } else 
            break;
    } while (1);

    memset (&m_cache[m_tail], 0, sizeof (frame_c));
    m_tail++;
    if (m_tail >= m_size)
        m_tail = 0;

    pthread_mutex_unlock (&m_lock);
    if (m_space_needed)
        pthread_cond_broadcast (&m_space_cond);
}

void frame_cache_c::pop_back () {
    int num_frame;

    pthread_mutex_lock (&m_lock);

    // Wait until frames available
    do {
        if (m_head >= m_tail)
            num_frame = m_head - m_tail;
        else 
            num_frame = m_head + m_size - m_tail;

        if (num_frame <= 0) {
            m_frame_needed++;
            pthread_cond_wait (&m_frame_cond, &m_lock);
            m_frame_needed--;
            continue;
        } else 
            break;
    } while (1);

    m_head--;
    if (m_head < 0)
        m_head = m_size - 1;
    memset (&m_cache[m_head], 0, sizeof (frame_c));

    pthread_mutex_unlock (&m_lock);
    if (m_space_needed)
        pthread_cond_broadcast (&m_space_cond);
}

void frame_cache_c::push_front (frame_c *pframe) {

}

void frame_cache_c::push_back (frame_c *pframe) {
    int num_space;
    
    assert (pframe != NULL);

    pthread_mutex_lock (&m_lock);
    do {
        if (m_head >= m_tail)
            num_space = (m_size - 1) - (m_head - m_tail);
        else
            num_space = m_tail - m_head -1;
        if (num_space <= 0) {
            m_space_needed++;
            fprintf (stderr, "Frame space needed, waiting...\n");
            pthread_cond_wait (&m_space_cond, &m_lock);
            fprintf (stderr, "Frame space now available, recheck\n");
            m_space_needed--;
            continue;
        } else 
            break;
    } while (1);

    memcpy (&m_cache[m_head], pframe, sizeof (frame_c));
    m_head++;
    if (m_head >= m_size)
        m_head = 0;

    if (m_frame_needed)
        pthread_cond_broadcast (&m_frame_cond);

    pthread_mutex_unlock (&m_lock);
}

void frame_cache_c::clear () {
    pthread_mutex_lock (&m_lock);

    m_head = m_tail = 0;
    m_frame_needed = m_space_needed = 0;
    memset (m_cache, 0, sizeof (frame_c) * MAX_FRAME_CACHE_SIZE);
    pthread_mutex_unlock (&m_lock);
}

uint32_t frame_cache_c::size () {
    uint32_t num_frame = 0;

    if (m_head >= m_tail)
        num_frame = m_head - m_tail;
    else 
        num_frame = m_head + m_size - m_tail;

    return num_frame;
}


