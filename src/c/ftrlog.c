
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>

#include <sys/errno.h>
#include <time.h>
#include "ftripc.h"
#include "ftr.h"  

#define OK 0
#define   SYSLOGKEY    7 

int initlogsem();
int GetLogSemKey();


SemStru Sem_errlog;  /* semphore struct */
int init_ok=0;

/*
**      display the proper start and stop time in sys.log
*/

int ProcessSys(char *errmsg)
{
    char datetimestr[128];
    FILE *tf;

    if((tf=fopen(SYSLOG,"a"))==NULL)
    {
         fprintf(stderr,"\n syslog.c:Fail to open SYSLOG fname=%s\n",SYSLOG);   
         return -1;
    }

    FtpGetTime(datetimestr);

    fprintf(tf,"%s%s",datetimestr,errmsg);
    fclose(tf);
    return 0;
}

/*
**     get the sem id
*/

int initlogsem()
{
	return OK;

        GetLogSemKey();
        Sem_errlog.semid = SemOpen(Sem_errlog.key);

	return OK;

}

/*
**      get the sem key
*/

int GetLogSemKey()
{
 char fname[200];

/*
        strcpy(fname, (char *)getenv("HOME"));
        if ( strlen(fname) > 1 )
         strcat(fname, IPC_MSGKEY);
        else
         strcpy(fname, IPC_MSGKEY);
*/
        strcpy(fname, IPC_MSGKEY);

        Sem_errlog.key = ftok(fname, (char)SYSLOGKEY);

	return OK;

}

