
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/param.h>


#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "ftr.h"
#include "ftrmsgq.h"

/*
**   This  Module  is  Main  Function of Server Daemon.  In Fact,
**   The Server is the Function trserv(). 
*/

main(int argc,char *argv[])
{
	int fret ;
	char *pname, cfgname[200];
 
	daemon_init();

	sprintf(cfgname,"./cfg/%s.cfg",argv[0]);
	fret=ReadConfig(cfgname);
	if(fret<0 ) {
		fprintf(stderr,"¶ÁÅäÖÃÎÄ¼þ%s³ö´í,fret=%d\n",cfgname,fret); 
		return ;
	}
	else
		TypeConfigVal() ;

	trserv("000","LOGICAL");
	exit(0);
}
