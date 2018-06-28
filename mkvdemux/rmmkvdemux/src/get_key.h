/*
 *
 * Copyright (c) Sigma Designs, Inc. 2003. All rights reserved.
 *
 */

/**
	@file get_key.c
	@brief non-blocking character read from terminal
	
	@author Christian Wolff
*/

#ifndef __GET_KEY_H__
#define __GET_KEY_H__

RM_EXTERN_C_BLOCKSTART

/**
   initialize the terminal to pass keypresses directly, without echo
   and to ignore ctrl-C.
   
   @param block_int  TRUE: block ctrl-C, FALSE: issue SIGINT on ctrl-C
*/
void RMTermInit(RMbool block_int);

/**
   undo the terminal initialization
*/
void RMTermExit(void);

/**
   catch all terminating signals and call a termination function instead
   RMTermExit() is always called, cleanup() only if pointer is non-zero.
   
   @param cleanup  function to handle signal occurence
   @param param
*/
void RMSignalInit(void (*cleanup)(void *param), void *param);

/**
   keypress availability
   
   @return TRUE if at least one character is available
*/
RMbool RMKeyAvailable(void);

/**
   keypress retreival
   
   @return value of the next keypress character, or '\\0' on error
*/
RMascii RMGetKey(void);

/**
   retreive keypress character, if available
   
   @param pKey
   @return TRUE if *pKey contains next keypress character, FALSE on unavailable or error
*/
RMbool RMGetKeyNoWait(RMascii *pKey);

/**
   enable echo for a terminal
*/
void RMTermEnableEcho(void);

/**
   disable echo for a terminal
*/
void RMTermDisableEcho(void);

/**
   get a uint32 from stdin

   @param data 
*/
void RMTermGetUint32(RMuint32 *data);


void RMremoteResetState(void);

	
RM_EXTERN_C_BLOCKEND

#endif  // __GET_KEY_H__
