#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "ftr.h"
#include "ftrerr.h"

int DirList(int sockd,char *pdirname,int wtime)
{
    DIR *dirp;
    struct dirent *dp;
    struct stat stbuf;
    struct ftr_fsndreq fsd;
    struct ftr_frcvresp frd;
    int bfsize,dl;
    int retcode;
    char msgword[256];

    memset((char *)&fsd,0,sizeof(fsd));
    memset((char *)&frd,0,sizeof(frd));
    fsd.dgtype=FSREQ;
    fsd.sndstat=FTR_SNDOK;

    if (stat(pdirname,&stbuf)<0) {
        sprintf(msgword,"<SERVER PID=%d> DirList() === Stat Error(%s),Path=[%s].\n",getpid(),strerror(errno),pdirname);
        ProcessSys(msgword);
        dl=sizeof(fsd)-FTR_FSNDBUF;
        memset(fsd.dglength,0,sizeof(fsd.dglength));
        sprintf(fsd.dglength,"%d",dl);
        fsd.sndstat=FTR_SNDERR;
        retcode=DGSnd(sockd,(char *)&fsd,dl);
        if (retcode < 0) {
            closedir(dirp);
            sprintf(msgword,"<SERVER PID=%d> DirList() === Pos. 1 : Send FSREQ DG Error.\n",getpid());
            ProcessSys(msgword);
            return FDGSND_ERR;
        }
        return -1;
    }

    if (S_ISREG(stbuf.st_mode)) {
        memset(fsd.databuf,0,sizeof(fsd.databuf));
        snprintf(fsd.databuf,FTR_FSNDBUF-1,"%s|f",basename(pdirname));
        bfsize=strlen(fsd.databuf);
        dl=(sizeof(fsd)-FTR_FSNDBUF) + bfsize;
        memset(fsd.dglength,0,sizeof(fsd.dglength));
        sprintf(fsd.dglength,"%d",dl);
        retcode=DGSnd(sockd,(char *)&fsd,dl);
        if (retcode < 0) {
            closedir(dirp);
            sprintf(msgword,"<SERVER PID=%d> DirList() === Pos. 4 : Send FSREQ DG Error.\n",getpid());
            ProcessSys(msgword);
            return FDGSND_ERR;
        }
    }
    else if (S_ISDIR(stbuf.st_mode)) {
        if ((dirp = opendir(pdirname)) == NULL) {
            sprintf(msgword,"<SERVER PID=%d> DirList() === Pos. 1 : Open Dir Error,Path=[%s].\n",getpid(),pdirname);
            ProcessSys(msgword);
    
            dl=sizeof(fsd)-FTR_FSNDBUF;
            memset(fsd.dglength,0,sizeof(fsd.dglength));
            sprintf(fsd.dglength,"%d",dl);
            fsd.sndstat=FTR_SNDERR;
            retcode=DGSnd(sockd,(char *)&fsd,dl);
            if (retcode < 0) {
                closedir(dirp);
                sprintf(msgword,"<SERVER PID=%d> DirList() === Pos. 3 : Send FSREQ DG Error.\n",getpid());
                ProcessSys(msgword);
                return FDGSND_ERR;
            }
            return -1;
        }
    
        while ((dp = readdir(dirp)) != NULL) {
            if (dp->d_name[0]=='.' || dp->d_name[0]=='\0') {
                continue;
            }
            snprintf(fsd.databuf,FTR_FSNDBUF-1,"%s|%c",dp->d_name,dp->d_type==DT_DIR?'d':'f');
            bfsize=strlen(fsd.databuf);
            dl=(sizeof(fsd)-FTR_FSNDBUF) + bfsize;
            memset(fsd.dglength,0,sizeof(fsd.dglength));
            sprintf(fsd.dglength,"%d",dl);
            retcode=DGSnd(sockd,(char *)&fsd,dl);
            if (retcode < 0) {
                closedir(dirp);
                sprintf(msgword,"<SERVER PID=%d> DirList() === Pos. 4 : Send FSREQ DG Error.\n",getpid());
                ProcessSys(msgword);
                return FDGSND_ERR;
            }
        }
        closedir(dirp);
    }
    else {
        sprintf(msgword,"<SERVER PID=%d> DirList() === Incorrect File Type,Path=[%s].\n",getpid(),pdirname);
        ProcessSys(msgword);
        dl=sizeof(fsd)-FTR_FSNDBUF;
        memset(fsd.dglength,0,sizeof(fsd.dglength));
        sprintf(fsd.dglength,"%d",dl);
        fsd.sndstat=FTR_SNDERR;
        retcode=DGSnd(sockd,(char *)&fsd,dl);
        if (retcode < 0) {
            closedir(dirp);
            sprintf(msgword,"<SERVER PID=%d> DirList() === Pos. 2 : Send FSREQ DG Error.\n",getpid());
            ProcessSys(msgword);
            return FDGSND_ERR;
        }
        return -1;
    }


    dl=sizeof(fsd)-FTR_FSNDBUF;
    memset(fsd.dglength,0,sizeof(fsd.dglength));
    sprintf(fsd.dglength,"%d",dl);
    fsd.sndstat=FTR_SNDEND;
    retcode = DGSnd(sockd,(char *)&fsd,dl);
    if (retcode < 0) {
        sprintf(msgword,"<       PID=%d> DirList() === Pos. 5 : Send FSREQ DG Error.\n",getpid());
        ProcessSys(msgword);
        return FDGSND_ERR;
    }

    retcode=DGRcv(sockd,FRRESP,(char *)&frd,wtime);
    if (retcode < 0) {
        sprintf(msgword,"<       PID=%d> DirList() === Pos. 6 : Receive FRRESP DG Error.\n",getpid());
        ProcessSys(msgword);
        return FDGRCV_ERR;
    }
    return 0;
}
