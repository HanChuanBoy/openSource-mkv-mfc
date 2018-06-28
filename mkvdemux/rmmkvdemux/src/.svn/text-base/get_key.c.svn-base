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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#define ALLOW_OS_CODE
#include "../../rmdef/rmdef.h"

#include "get_key.h"

#include "../../samples/command_ids.h"
#include "../../rmcore/include/rmcorequeue.h"

static struct termios tio_save;
static RMuint32 init_count = 0;
static RMuint32 exit_count = 0;
static void (*terminate)(void *param) = NULL;
void *g_param;

#include "../../rmremoteapi/include/rmremoteapi.h"
RMremoteHandle rh = (RMremoteHandle) NULL;

enum remote_state {
	REMOTE_STATE_PLAYING = 0,
	REMOTE_STATE_PAUSED,
	REMOTE_STATE_STOPPED,
	REMOTE_STATE_SLOW_FWD,
	REMOTE_STATE_FAST_FWD,
	REMOTE_STATE_FAST_RWD,
};


#define REMOTE_CMD_FIFO_SIZE 16
/* *32 */
#define MAX_SPEED 8192

/* /32 */
#define MIN_SPEED 8

/* *4 */
#define MAX_SPEED_ALL_FRAME 1024

static RMcoreQueue remote_cmd_queue;
static RMcoreQueue remote_param_queue;
static enum remote_state state;
static RMint32 speed_factor;

static RMascii interpret_symbol(RMremoteKey key)
{
	switch (key) {
	case RM_HW_ON_OFF:
		return KEY_CMD_QUIT;
	case RM_HW_PROGRAM:
	case RM_HW_UP:
	case RM_HW_DOWN:
	case RM_HW_LEFT:
	case RM_HW_RIGHT:
	case RM_HW_SELECT:
	case RM_HW_MENU:
	case RM_HW_TITLE:
	case RM_HW_OSD:
	case RM_HW_LANGUAGE:
	case RM_HW_ANGLE:
	case RM_HW_SUB_TITLE:
		return KEY_CMD_NONE1;
	case RM_HW_PLAY_PAUSE:
		if (state != REMOTE_STATE_PLAYING) {
			state = REMOTE_STATE_PLAYING;
			speed_factor = 256;
			return KEY_CMD_PLAY;
		}
		else {
			state = REMOTE_STATE_PAUSED;
			return KEY_CMD_PAUSE;
		}
	case RM_HW_STOP:
		state = REMOTE_STATE_STOPPED;
		return KEY_CMD_STOP_ZERO;
	case RM_HW_PAUSE_PLAY:
		if (state != REMOTE_STATE_PAUSED) {
			RMInsertFirstCoreQueue(remote_cmd_queue, (void *) KEY_CMD_NEXT_PICTURE);
			state = REMOTE_STATE_PAUSED;
			return KEY_CMD_PAUSE;
		}
		return KEY_CMD_NEXT_PICTURE;
	case RM_HW_EJECT:
		return KEY_CMD_NONE1;
	case RM_HW_FAST_REWIND: 
		if (speed_factor > 0) {
			speed_factor = -256;
		}
		
		if (speed_factor == -MAX_SPEED) {
			speed_factor = 256;
			state = REMOTE_STATE_PLAYING;
			return KEY_CMD_PLAY;
		}
		speed_factor *= 2;
		RMInsertFirstCoreQueue(remote_param_queue, (void *) speed_factor);
		RMInsertFirstCoreQueue(remote_cmd_queue, (void *) KEY_CMD_SPEED_FACTOR);
		state = REMOTE_STATE_FAST_RWD;
		return KEY_CMD_IFRAME_BWD;
	case RM_HW_FAST_FORWARD:
		if (speed_factor == MAX_SPEED) {
			speed_factor = 256;
			state = REMOTE_STATE_PLAYING;
			return KEY_CMD_PLAY;
		}
		speed_factor *= 2;
		RMInsertFirstCoreQueue(remote_param_queue, (void *) speed_factor);
		state = (speed_factor > 256) ? REMOTE_STATE_FAST_FWD : REMOTE_STATE_SLOW_FWD; 
		if (speed_factor > MAX_SPEED_ALL_FRAME) {
			RMInsertFirstCoreQueue(remote_cmd_queue, (void *) KEY_CMD_SPEED_FACTOR);
			return KEY_CMD_IFRAME_FWD;
		}
		return KEY_CMD_SPEED_FACTOR;
	case RM_HW_PREV_TRACK:
		return KEY_CMD_PREV_TRACK;
	case RM_HW_NEXT_TRACK:
		return KEY_CMD_NEXT_TRACK;
	case RM_HW_VOL_PLUS:
		return KEY_CMD_VOLUME_PLUS;
	case RM_HW_VOL_MINUS:
		return KEY_CMD_VOLUME_MINUS;
	case RM_HW_MUTE:
		return KEY_CMD_VOLUME_MUTE;
	case RM_HW_L_R:
	case RM_HW_KEY_1:
	case RM_HW_KEY_2:
	case RM_HW_KEY_3:
	case RM_HW_KEY_4:
	case RM_HW_KEY_5:
	case RM_HW_KEY_6:
	case RM_HW_KEY_7:
	case RM_HW_KEY_8:
	case RM_HW_KEY_9:
	case RM_HW_KEY_0:
	case RM_HW_VGA_TV:
	case RM_HW_ENT:
	case RM_HW_SLOW_REVERSE: 
		return KEY_CMD_NONE1;
	case RM_HW_SLOW_FORWARD:
		if (speed_factor == MIN_SPEED) {
			speed_factor = 256;
			state = REMOTE_STATE_PLAYING;
			return KEY_CMD_PLAY;
		}
		speed_factor /= 2;
		RMInsertFirstCoreQueue(remote_param_queue, (void *) speed_factor);
		state = (speed_factor > 256) ? REMOTE_STATE_FAST_FWD : REMOTE_STATE_SLOW_FWD; 
		if ((speed_factor <= MAX_SPEED_ALL_FRAME) && (speed_factor > MAX_SPEED_ALL_FRAME/2)) {
			RMInsertFirstCoreQueue(remote_cmd_queue, (void *) KEY_CMD_SPEED_FACTOR);
			return KEY_CMD_PLAY;
		}
		return KEY_CMD_SPEED_FACTOR;
	case RM_HW_REPEAT: 
	case RM_HW_AB_REPEAT: 
	case RM_HW_SETUP:
	case RM_HW_CLEAR:
	case RM_HW_TVMODE:
	case RM_HW_PBC:
	case RM_HW_RETURN:
	case RM_HW_SHUFFLE:
	case RM_HW_SEARCH:
	case RM_HW_ZOOM:
	case RM_HW_TIMEOUT:
	default:
		return KEY_CMD_NONE1;
	}

	return KEY_CMD_NONE1;
}

static void sig_handler(int signo)
{
	if (exit_count++) {
		RMDBGLOG((ENABLE, "Signal %d during cleanup, exiting unclean!\n", signo));
		exit(1);
	}
	RMDBGLOG((ENABLE, "Signal %d, exiting...\n", signo));
	RMTermExit();
	if (terminate) (*terminate)(g_param);
	exit(0);
}

void RMTermInit(RMbool block_int)
{
	struct termios tio;
	
	if (rh == NULL) {
		if (init_count++) return;
		if (tcgetattr(STDIN_FILENO, &tio_save) < 0) {
			init_count--;
			return;
		}
		tio = tio_save;
		if (block_int) {
			tio.c_lflag &= ~(ECHO | ICANON | ISIG);
			tio.c_iflag &= ~(BRKINT);
		} else {
			tio.c_lflag |= (ISIG);
			tio.c_lflag &= ~(ECHO | ICANON);
			tio.c_iflag |= (BRKINT);
			tio.c_iflag &= ~(IGNBRK);
		}
		
		/* 
		   arrows or F1 characters generate a sequence of 3 characters in a row:
		   like ESC [ A (up arrow).
		   
		   To avoid reading [ or A as an individual character, we set
		   an inter character interval of 1/10 second. If characters
		   are received in less of 1/10 second then it is treated as
		   an individual character and is discard.
		*/
		
		tio.c_cc[VMIN] = 3;   /* up to 3 chars at a time */
		tio.c_cc[VTIME] = 1;  /* after receiving 1 char and after a delay of 1/10 second, 'read' returns */
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &tio);
	}
	else {
		remote_cmd_queue = RMCreateCoreQueue(REMOTE_CMD_FIFO_SIZE);
		remote_param_queue = RMCreateCoreQueue(REMOTE_CMD_FIFO_SIZE);
		state = REMOTE_STATE_PLAYING;
		speed_factor = 256;
	}
}

void RMTermExit(void)
{
	if (rh == NULL) {
		if (--init_count) return;
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &tio_save);
	}
	else {
		RMDeleteCoreQueue(remote_cmd_queue);
		RMDeleteCoreQueue(remote_param_queue);
	}
}

void RMSignalInit(void (*cleanup)(void *param), void *param)
{
	if (rh == NULL) {
		terminate = cleanup;
		g_param = param;
		
		if (signal(SIGBUS, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGBUS!\n"));
#ifdef SIGEMT
		if (signal(SIGEMT, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGEMT!\n"));
#endif
		if (signal(SIGFPE, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGFPE!\n"));
		if (signal(SIGHUP, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGHUP!\n"));
		if (signal(SIGILL, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGILL!\n"));
		if (signal(SIGINT, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGINT!\n"));
		if (signal(SIGIOT, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGIOT!\n"));
		if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGPIPE!\n"));
		if (signal(SIGQUIT, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGQUIT!\n"));
		if (signal(SIGSEGV, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGSEGV!\n"));
		if (signal(SIGSYS, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGSYS!\n"));
		if (signal(SIGTERM, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGTERM!\n"));
		if (signal(SIGTRAP, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGTRAP!\n"));
		if (signal(SIGUSR1, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGUSR1!\n"));
		if (signal(SIGUSR2, sig_handler) == SIG_ERR) RMDBGLOG((ENABLE, "Can't catch SIGUSR2!\n"));
	}
}

RMbool RMKeyAvailable(void)
{
	struct timeval tv;
	fd_set readfds;
	
	if (rh == NULL) {
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		FD_ZERO(&readfds);
		FD_SET(STDIN_FILENO, &readfds);
		return (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0);
	}
	else {
		if (RMGetCountCoreQueue (remote_cmd_queue) > 0)
			return TRUE;
		else
			return RMFremoteSymbolAvailable(rh);
	}
}

RMascii RMGetKey(void)
{
	if (rh == NULL) {
		RMascii Key[3];
		if (read(STDIN_FILENO, Key, 3) == 1) return Key[0];
		return '\0';
	}
	else {
		RMremoteKey key;
		if (RMGetCountCoreQueue (remote_cmd_queue) > 0) {
			void *item;

			RMRemoveFirstCoreQueue(remote_cmd_queue, &item);
			return (RMascii) (RMuint32) item;
		}
		else {
			while ((key = RMFremoteWaitSymbol(rh, 100000000)) == RM_HW_TIMEOUT);
			return interpret_symbol(key);
		}
	}
}

RMbool RMGetKeyNoWait(RMascii *pKey)
{
	RMascii Key[3];

	if (rh == NULL) {
		if (! RMKeyAvailable()) return FALSE;
		if (pKey == NULL) return FALSE;
		if (read(STDIN_FILENO, Key, 3) == 1) {
			*pKey = Key[0];
			return TRUE;
		}
		return FALSE;
	}
	else {
		RMremoteKey key;

		if (RMGetCountCoreQueue (remote_cmd_queue) > 0) {
			void *item;

			RMRemoveFirstCoreQueue(remote_cmd_queue, &item);
			return (RMascii) (RMuint32) item;
		}
		else {
			key = RMFremoteWaitSymbol(rh, 0);
			if (key != RM_HW_TIMEOUT) {
				*pKey = interpret_symbol(key);
				return TRUE;
			}
			else
				return FALSE;
		}
	}
}

void RMTermEnableEcho()
{
	static struct termios tio;
	
	if (rh == NULL) {
		if (tcgetattr(STDIN_FILENO, &tio) < 0) {
			return;
		}
		tio.c_lflag |= ECHO | ICANON;
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &tio);
	}
}

void RMTermDisableEcho()
{
	static struct termios tio;

	if (rh == NULL) {
		if (tcgetattr(STDIN_FILENO, &tio) < 0) {
			return;
		}
		tio.c_lflag &=  ~( ECHO | ICANON );
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &tio);
	}
}

void RMTermGetUint32(RMuint32 *data)
{
	//static struct termios tio;
	RMbool handle_echo = FALSE;

	if (rh == NULL) {
		RMuint32 i = 0;
		RMuint8 buffer[31];
		RMascii key;

		if (data == NULL)
			return;
		/*		
		if ( tcgetattr(STDIN_FILENO, &tio) == 0 && (tio.c_lflag & ( ECHO | ICANON )) == 0 )
			handle_echo = TRUE;
		*/
		
		if (handle_echo)	
			RMTermEnableEcho();

try_again:
		buffer[0] = '\0';
		while (i < 31) {
			if (RMKeyAvailable()) {
				key = RMGetKey();
				printf("%c", key); fflush(stdout); //echo
				if ((key >= '0') && (key <= '9')) {
					buffer[i++] = key;
					buffer[i] = '\0';
				}
				if (key == '\n') {
					buffer[i] = '\0';
					break;
				}
			}
		}
		RMDBGLOG((DISABLE, "read '%s'\n", buffer));
		sscanf((char *)buffer, "%lu", data);
		RMDBGLOG((DISABLE, "scanned %lu\n", *data));
		
		if (i < 1) {
			fprintf(stderr, "you must enter a number\n");
			goto try_again;
		}

		if (handle_echo)
			RMTermDisableEcho();
	}
	else {
		if (RMGetCountCoreQueue (remote_param_queue) > 0) {
			void *item;
			RMRemoveFirstCoreQueue(remote_param_queue, &item);
			*data = (RMuint32) item;
		}
	}
}


void RMremoteResetState(void)
{
	RMDBGLOG((ENABLE, "RMremoteResetState()\n"));
	state = REMOTE_STATE_PLAYING;
	speed_factor = 256;
}
