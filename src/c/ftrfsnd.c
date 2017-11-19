
/*
**     SOURCE FILE: ftrfsnd.c
**     PROGRAM NAME: Fftp()
**
**     FUNCTION:
**        Send or Receive a specified file to or from Server.
*/

#include <stdio.h>
#include <stdlib.h>
#include "ftr.h"
#include "ftrerr.h"

int Fftp(char *cfp, char *sfp, char *saddr, char atype, char *mode_new)
{
 struct host_cfg *GetConf();

 struct host_cfg *shp,shost;
 int retcode;
 char cfnbuf[FNAMELEN];
 char cfname[FNAMELEN];
 char sfname[FNAMELEN];
 char msgword[1000];

/*
**    Interface Parameter(s) Check
*/

 if ((atype != LOGICAL) && (atype != NETWORK)) {
    sprintf(msgword,"<CLIENT PID=%d> FSnd() === Pos. 1 : Calling Parameter(s) Error.\n",getpid());
   	printf("<CLIENT PID=%d> FSnd() === Pos. 1 : Calling Parameter(s) Error.\n",getpid());
    ProcessSys(msgword);
    return(PARM_ERR);
 }
 if ((saddr == (char *)0) || (saddr == (char *)-1)) {
    sprintf(msgword,"<CLIENT PID=%d> FSnd() === Pos. 2 : Calling Parameter(s) Error.\n",getpid());
    ProcessSys(msgword);
    return(PARM_ERR);
 }
 if ((cfp == (char *)0) || (sfp == (char *)-1)) {
    sprintf(msgword,"<CLIENT PID=%d> FSnd() === Pos. 3 : Calling Parameter(s) Error.\n",getpid());
    ProcessSys(msgword);
    return(PARM_ERR);
 }
 if(getenv("FILEOUT_PATH")==NULL)
 	strcpy(cfname,FILEOUT_PATH);
 else
 	strcpy(cfname, getenv("FILEOUT_PATH"));
 strcat(cfname,"/");
 strcat(cfname,cfp);
 sprintf(msgword,"<CLIENT PID=%d> FSnd() === Pos. 3.5 : ftpcli fname=%s \n",
			getpid(), cfname);
 ProcessSys(msgword);
 
 if (sfp == (char *)0) strcpy(sfname,cfp);
 else strcpy(sfname,sfp);
    
/*
**    Get Server Host ID Information
*/

 shp=GetConf(saddr,atype);
 if (shp == NULLP) {
    sprintf(msgword,"<CLIENT PID=%d> FSnd() === Pos. 4 : Host Configuration Error.\n",getpid());
    ProcessSys(msgword);
    return(CFG_ERR);
 }
 shost=*shp;

    if(mode_new[0]!='R'){
        retcode=TCPFSnd(&shost,cfname,sfname);
    }
    else {
        retcode=TCPFRcv(&shost,cfname,sfname);
        if( retcode>=0 ) retcode=SUCCESS ; 	
    }
    if (retcode < 0) {
       sprintf(msgword,"<CLIENT PID=%d> FSnd() === Pos. 5 : TCPFSnd() Failure.\n",getpid());
       ProcessSys(msgword);
    } 
    else {
       sprintf(msgword,"<CLIENT PID=%d> FSnd() : TCPFSnd() OK.\n",getpid());
       ProcessSys(msgword);
    }
 
/*
**   If File Send OK, Move File to Save Directory and Remove The File from
**   Directory FILEOUT
*/

 return(retcode);

 if (retcode == SUCCESS) {
    strcpy(cfnbuf,FILEMOV_PATH);
    strcat(cfnbuf,"/");
    strcat(cfnbuf,cfp);
    link(cfname,cfnbuf);
    unlink(cfname);
 }

 return(retcode);

}
