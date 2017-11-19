
#ifndef __IPCN_H
#define __IPCN_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

typedef struct ShmStru {
	key_t   key;
	int     shmid;
} ShmStru;

/* semaphore corresponding table */

typedef struct SemStru {
	key_t   key;
	int     semid;  /* semaphore id for queue */
} SemStru;

typedef struct MsgStru {
	key_t	key;
	int     msgid;
} MsgStru;

#endif
