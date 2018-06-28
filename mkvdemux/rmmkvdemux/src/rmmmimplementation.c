/*****************************************
 Copyright © 2001-2003  
 Sigma Designs, Inc. All Rights Reserved
 Proprietary and Confidential
 *****************************************/
/**
  @file   rmmm.c
  @brief  

  @author Christian Wolff
  @date   2003-04-03
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ALLOW_OS_CODE 1
#include "../../rmdef/rmdef.h"

inline void *RMMalloc(RMuint32 size)
{
	return malloc(size);
}

inline void RMFree(void *ptr)
{
	free(ptr);
}

inline void *RMMemset(void *s, RMuint8 c, RMuint32 n)
{
	return memset(s, c, n);
}

inline void *RMMemcpy(void *dest, const void *src, RMuint32 n)
{
	return memcpy(dest, src, n);
}

inline RMint32 RMMemcmp(const void *s1, const void *s2, RMuint32 n)
{
	return memcmp(s1, s2, n);
}

inline void *RMCalloc(RMuint32 nmemb, RMuint32 size)
{
	void *ptr = RMMalloc(nmemb * size);
	if (ptr != NULL) RMMemset(ptr, 0, nmemb * size);
	return ptr;
}

