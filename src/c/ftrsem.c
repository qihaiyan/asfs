#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define SEM_PERMS     0664

static struct sembuf p_buf = { 0, -1, SEM_UNDO };
static struct sembuf v_buf = { 0, 1, SEM_UNDO };

int SemOpen(int id)
  {
    int semid, st = 0;

    semid = semget((key_t)id, 1, SEM_PERMS | IPC_EXCL | IPC_CREAT);
    if ( semid < 0 )
      {
        if ( errno == EEXIST )
        semid = semget((key_t)id, 1, 0);
      }
    else st = semctl(semid, 0, SETVAL, 1);
    if ( semid < 0 || st < 0 )
      return( -1 );
    return( semid );
  }

int Ipc_P_Operation(int semid)
  {
    semop(semid, &p_buf, 1);
  }

int Ipc_V_Operation(int semid)
  {
    semop(semid, &v_buf, 1);
  }


int SemClose(int semid)
  {
    return( semctl(semid, IPC_RMID, 0) ) ;
  }

