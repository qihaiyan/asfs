
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "ftr.h"
#include "ftrerr.h"

int TCPFSnd(struct host_cfg *shp, char *cfp, char *sfp)
{
 int retcode,sockd,Pid;
 struct ftr_req freq;
 struct ftr_resp fresp;
 char msgword[256];


 memset((char *)&freq,0,sizeof(freq));
 memset((char *)&fresp,0,sizeof(fresp));
 Pid=getpid();

/*
**    Make FTR Request Datagram
*/

 freq.dgtype=FPUTREQ;
 sprintf(freq.dglength,"%d",sizeof(freq));
 strcpy(freq.sfile,sfp);
 MakeKey(freq.filekey,FKEYSIZE);
 
/*
**    Make Connection to Server
*/

 if ((sockd=MkConnect(shp->Naddress,atoi(shp->portno))) < 0) {
    sprintf(msgword,"<       PID=%d> TCPFSnd() === Pos. 1 : MkConnect() Connection Error.\n",Pid);
    ProcessSys(msgword);
    return(LINK_ERR);
 }

/*
**    Send FTR Request Datagram to Server
*/

 if ((retcode=DGSnd(sockd,(char *)&freq,sizeof(freq))) < 0) {
    Disconnect(sockd);
    sprintf(msgword,"<       PID=%d> TCPFSnd() === Pos. 2 : DGSnd() Send Request DG Error.\n",Pid);
    ProcessSys(msgword);
    return(retcode);
 }

/*
**    Receive FTR Response Datagram from Server
*/

 if ((retcode=DGRcv(sockd,FRESP,(char *)&fresp,shp->wtime)) < 0) {
    Disconnect(sockd);
    sprintf(msgword,"<       PID=%d> TCPFSnd() === Pos. 3 : DGRcv() Receive Response DG Error.\n",Pid);
    ProcessSys(msgword);
    return(retcode);
 }

/*
**    Check Response Status
*/

 if (fresp.respstat != FTR_SERVEROK) {
    sprintf(msgword,"<       PID=%d> TCPFSnd() === Pos. 4 : Response DG Status Error.\n",Pid);
    ProcessSys(msgword);
    Disconnect(sockd);
    return(SDINY_ERR);
 }

/*
**    Send A Local File to Server
*/

 retcode=SndFile(sockd,cfp,freq.filekey,FKEYSIZE,shp->wtime);
 if (retcode < 0) {
    sprintf(msgword,"<       PID=%d> TCPFSnd() === Pos. 5 : SndFileServer() Error.\n",Pid);
    ProcessSys(msgword);
 } else {
    sprintf(msgword,"<       PID=%d> TCPFSnd() === Pos. 5 : OK!\n",Pid);
    ProcessSys(msgword);
 }

 Disconnect(sockd);
 return(retcode);

}

int TCPFRcv(struct host_cfg *shp, char *cfp, char *sfp)
{
 int retcode,sockd,Pid;
 struct ftr_req freq;
 struct ftr_resp fresp;
 char msgword[256];
 char sfile[256];
 char sfile2[256];
 int ftrfd;

 memset((char *)&freq,0,sizeof(freq));
 memset((char *)&fresp,0,sizeof(fresp));
 Pid=getpid();


/*
**    Make FTR Request Datagram
*/
       /** added by bjli 2001.11.28  **/
 if(getenv("FILEIN_PATH")==NULL)
 	strcpy(sfile,FILEIN_PATH);
 else
 	strcpy(sfile,getenv("FILEIN_PATH"));
 strcat(sfile,"/");
 strcat(sfile,basename(sfp));
/*******
 ftrfd=open(sfile,O_CREAT|O_WRONLY|O_TRUNC,0664) ;
 if (ftrfd < 0) return(SFOPEN_ERR);
*******/
	/* added by taoh 2002.09.14 传送时用临时文件名 */
 sprintf(sfile2,"%s%s",sfile,FILE_SUFFIX ) ;
 unlink(sfile);
    sprintf(msgword,"<       PID=%d> TCPFRcv() === local file=%s.\n",Pid,sfile2);
    ProcessSys(msgword);
 ftrfd=open(sfile2,O_CREAT|O_WRONLY|O_TRUNC,0664) ;
 if (ftrfd < 0) return(SFOPEN_ERR);

 freq.dgtype=FGETREQ;	/** bjli 2001.11.28  **/
 sprintf(freq.dglength,"%d",sizeof(freq));
 strcpy(freq.sfile,sfp);
 MakeKey(freq.filekey,FKEYSIZE);
 
/*
**    Make Connection to Server
*/

 if ((sockd=MkConnect(shp->Naddress,atoi(shp->portno))) < 0) {
    sprintf(msgword,"<       PID=%d> TCPFRcv() === Pos. 1 : MkConnect() Connection Error.\n",Pid);
    ProcessSys(msgword);
    return(LINK_ERR);
 }

/*
**    Send FTR Request Datagram to Server
*/

 if ((retcode=DGSnd(sockd,(char *)&freq,sizeof(freq))) < 0) {
    Disconnect(sockd);
    sprintf(msgword,"<       PID=%d> TCPFRcv() === Pos. 2 : DGSnd() Send Request DG Error.\n",Pid);
    ProcessSys(msgword);
    return(retcode);
 }

/*
**    Receive FTR Response Datagram from Server
*/

 if ((retcode=DGRcv(sockd,FRESP,(char *)&fresp,shp->wtime)) < 0) {
    Disconnect(sockd);
    sprintf(msgword,"<       PID=%d> TCPFRcv() === Pos. 3 : DGRcv() Receive Response DG Error.\n",Pid);
    ProcessSys(msgword);
    return(retcode);
 }

/*
**    Check Response Status
*/

 if (fresp.respstat != FTR_SERVEROK) {
    sprintf(msgword,"<       PID=%d> TCPFRcv() === Pos. 4 : Response DG Status Error.\n",Pid);
    ProcessSys(msgword);
    Disconnect(sockd);
    return(SDINY_ERR);
 }


  retcode=RcvFile(sockd,ftrfd,freq.filekey,FKEYSIZE,shp->wtime);
	/**   end bjli 2001.11.28  **/
  close(ftrfd);

 if (retcode < 0) {
    sprintf(msgword,"<       PID=%d> TCPFRcv() === Pos. 5 : SndFileServer() Error:%d.\n",Pid, retcode);
    ProcessSys(msgword);
 } else {
    sprintf(msgword,"<       PID=%d> TCPFRcv() === Pos. 5 : OK!\n",Pid);
    ProcessSys(msgword);
	 if(rename( sfile2,sfile )!=0) retcode=-999 ;	 
	/* added by taoh 2002.09.14 将临时文件名改成正式文件名 */
 }
 Disconnect(sockd);
 return(retcode);

}


int MakeKey(char *kp, int ksize)
{
 time_t initv;
 int i,j;
 char *cp1,initch;

 initv=time((time_t *)0);
 j=(int)initv;
 cp1=kp;
 for (i=0; i<ksize; i++,cp1++) {
    j >>= (i%5);
    srand(j);
    j=rand();
    initch=(char)(0xff&j);
    *cp1=initch;
 }
 return;
}

/*
**   Function DGSnd()
*/

int DGSnd(int sockd, char *dgp, int dl)
{
 int i,j,k;
 register char *dp;

/*   Check Socket Descriptor is Valid, or Not.   */

 signal(SIGPIPE,SIG_IGN);
 dp=dgp;
 if (sockd < 0) return(PARM_ERR);
 
/*   Make Sure to Set Datagram Length Field to Parameter dl   */

 k=0;

/*   Send Datagram to Destination Host   */

 i=dl;
 j=0;
 while (i > 0) {
    if (i > DGMTU) {
       if ((j=write(sockd,dp,DGMTU)) <= 0) return(DGSND_ERR);
    } else {
       if ((j=write(sockd,dp,i)) <= 0) return(DGSND_ERR);
    }
    k += j;
    i-=j;
    dp+=j;
 }
 return(k);
}

/*
**   Function DGRcv()
*/

#define NOTRCVTOUT    0
#define RCVTOUT      -1

int toutflg=NOTRCVTOUT;

int DGRcv(int sockd, int dgtype, char *dgp, int maxtm)
{
 void Tout();
 int fret,i,j,k,n;
 register char *dp;
 char bjlen[10];
 char *dgp1;
 extern int errno;

/*   Check Parameters Passed by Caller is Valid, or Not   */

 signal(SIGPIPE,SIG_IGN);
 signal(SIGALRM,Tout);
 alarm(maxtm);
 if (sockd < 0) {
    signal(SIGALRM,SIG_IGN);
    return(PARM_ERR);
 }

 dp=dgp;
 j=0;
 i=0;
 k=1+STD_DGSIZE;
 while (k > 0) {
    if ((j=read(sockd,dp,k)) < 0) {
       if (errno == EINTR) {
          if (toutflg == RCVTOUT) {
             signal(SIGALRM,SIG_IGN);
             return(TOUT_ERR);
          }
       } else {
          signal(SIGALRM,SIG_IGN);
          return(DGRCV_ERR);
       }
    }
    if (j == 0) {
       signal(SIGALRM,SIG_IGN);
       return(DGRCV_END);
    }
    /*** READ OK  */

    i += j;
    dp += j;
    k -= j;
 }

/*   Get Length of Datagram to Receive   */

 dgp1=dgp;
 dgp1++;
 sprintf ( bjlen, "%5.5s", dgp1 ) ;
 n=atoi(bjlen);
 if (n <= 0) {
    signal(SIGALRM,SIG_IGN);
    return(DGSIZE_ERR);
 }

/*   Secondary, Read Rest Datagram as Datagram Length to Receive   */

 n -= i;
 while (n > 0) {
    if (n > DGMTU) k=DGMTU;
    else k=n;

/*   Datagram to Receive is Not Arrival in Specified Timer (TIMEOUT)   */

    if ((j=read(sockd,dp,k)) < 0) {
       if (errno == EINTR) {
          if (toutflg == RCVTOUT) {
             signal(SIGALRM,SIG_IGN);
             return(TOUT_ERR);
          }
       } else {
          signal(SIGALRM,SIG_IGN);
          return(FDGRCV_ERR);
       }
    }
    if (j == 0) {
       signal(SIGALRM,SIG_IGN);
       return(FDGRCV_ERR);
    }
    /*** READ OK  */

    i += j;
    dp += j;
    n -= j;
 }
 signal(SIGALRM,SIG_IGN);

/*   Receipt Operation is Successful   */

 return(i);

}

void Tout()
{
 extern int toutflg;

 toutflg=RCVTOUT;
 signal(SIGALRM,SIG_IGN);
 return;
}

#define LOOP         0
#define STOP         1

int RcvFile(int sockd, int fd, char *fkp, int fksize, int wtime)
{
    int retcode,bfsize,dl,rl,endflag,cpsize;
    char filebuff[FBUFSIZE];
    char *fbp,*fkeyp,*dp;
    char msgword[256];
    char bjlen[10] ;
    struct ftr_fsndreq fsd;
    struct ftr_frcvresp frd;

    int    sendflag=0 ;
    /*
    sendflag=0:不需要给客户端发送信息 
    sendflag=1:需要给客户端发送信息 
    */

    /*
    **     Reset Datagram Buffer
    */

    memset((char *)&fsd,0,sizeof(fsd));
    memset((char *)&frd,0,sizeof(frd));
    fkeyp=fkp;

    frd.dgtype=FRRESP;
    sprintf(frd.dglength,"%d",sizeof(frd));
    endflag=LOOP;
    fbp=filebuff;
    bfsize=0;

    while (endflag == LOOP) {
        if ((retcode=DGRcv(sockd,FSREQ,(char *)&fsd,wtime)) < 0) {
            frd.rcvstat=FTR_RCVERR;
            DGSnd(sockd,(char *)&frd,sizeof(frd));
            sprintf(msgword,"<       PID=%d> RcvFile() === Pos.1:DG Receive Error.\n",getpid());
            ProcessSys(msgword);
            retcode=SDGRCV_ERR;
            endflag=STOP;
        }
        switch (fsd.sndstat) {
           case FTR_SNDEND:
             if (bfsize > 0) {
               fkeyp=Encrypt(filebuff,bfsize,fkp,fksize);
               write(fd,filebuff,bfsize);
             }
             endflag=STOP;
             retcode=SUCCESS;
             sendflag=1 ;
             break;

           case FTR_SNDOK:
             dp=fsd.databuf;
             sprintf ( bjlen, "%5.5s", fsd.dglength ) ;
             cpsize=atoi(bjlen)-(sizeof(fsd)-FTR_FSNDBUF);
             if ((bfsize+cpsize) >= FBUFSIZE) {
               while (bfsize < FBUFSIZE) {
                 *fbp++=*dp++;
                 bfsize++;
                 cpsize--;
               }
               fkeyp=Encrypt(filebuff,bfsize,fkp,fksize);
               retcode=write(fd,filebuff,bfsize);
               if (retcode != bfsize) {
                 frd.rcvstat=FTR_RCVERR;
                 DGSnd(sockd,(char *)&frd,sizeof(frd));
                 sprintf(msgword,"<       PID=%d> RcvFile() === Pos.2:WR File Error.\n",getpid());
                 ProcessSys(msgword);
                 retcode=SDGRCV_ERR;
                 break;
               }
               bfsize=0;
               fbp=filebuff;
               while (cpsize > 0) {
                 *fbp++=*dp++;
                 bfsize++;
                 cpsize--;
               }
             }
             else {
               while (cpsize > 0) {
                 *fbp++=*dp++;
                 bfsize++;
                 cpsize--;
               }
             }

             if( 0 )     /* del by taoh 2002.05.15 */
             {
               frd.rcvstat=FTR_RCVOK;
               retcode=DGSnd(sockd,(char *)&frd,sizeof(frd));
               if (retcode < 0) {
                 sprintf(msgword,"<       PID=%d> RcvFile() === Pos.3:DG Send Error.\n",getpid());
                 ProcessSys(msgword);
                 retcode=SDGSND_ERR;
               }
             }
             break;
           case FTR_SNDERR:
             sprintf(msgword,"<       PID=%d> RcvFile() === Pos. 4: DG Status Error, Status=FTR_SNDERR.\n",getpid());
             ProcessSys(msgword);
             retcode=SSNDSTAT_ERR;
             break;
           default:
             sprintf(msgword,"<       PID=%d> RcvFile() === Pos. 5: DG Status Error, Status=%d.\n",getpid(),fsd.sndstat);
             ProcessSys(msgword);
             retcode=SSNDSTAT_ERR;
             break;
        }
        if (retcode < 0) endflag=STOP;
    }
  
    if( sendflag==1 )     /* Send to client added by taoh 2002.05.15 */
    {
        frd.rcvstat=FTR_RCVOK ;
        retcode=DGSnd(sockd,(char *)&frd,sizeof(frd)) ;
        if (retcode < 0) {
            sprintf(msgword,"<       PID=%d> RcvFile() === Pos. 6: DG Send Error.\n",getpid()) ;
            ProcessSys(msgword) ;
            retcode=SDGSND_ERR ;
        }
    }
    return(retcode);
}

int SndFile(int sockd, char *cfp, char *fkp, int fksize, int wtime)
{
 int fd,retcode,bfsize,dl,rl,Pid;
 char filebuff[FBUFSIZE];
 char *fbp,*fkeyp;
 char msgword[256];
 struct ftr_fsndreq fsd;
 struct ftr_frcvresp frd;

#if 1  /** added by bjli    **/
 int count=0;	/** added by bjli  **/
 int  skip_count=10;

#endif

/*
**     Reset Datagram Buffer
*/

 memset((char *)&fsd,0,sizeof(fsd));
 memset((char *)&frd,0,sizeof(frd));
 fkeyp=fkp;
 fsd.dgtype=FSREQ;
 Pid=getpid();

/*
**     Open File to be sent
*/

 if ((fd=open(cfp,O_RDONLY)) < 0) {
    dl=sizeof(fsd)-FTR_FSNDBUF;
    sprintf(fsd.dglength,"%d",dl);
    fsd.sndstat=FTR_SNDERR;
    DGSnd(sockd,(char *)&fsd,sizeof(fsd)-FTR_FSNDBUF);
    sprintf(msgword,"<       PID=%d> SndFile() === Pos. 1 : Open Local File Error.\n",Pid);
    ProcessSys(msgword);
    return(FOPEN_ERR);
 }

/*
**     PUT a File to Server
*/

 while ((rl=read(fd,filebuff,FBUFSIZE)) > 0) {

    fkeyp=Encrypt(filebuff,rl,fkp,fksize);
    fbp=filebuff;
    while (rl > 0) {
       if (rl >= FTR_FSNDBUF) {
          memcpy(fsd.databuf,fbp,FTR_FSNDBUF);
          bfsize=FTR_FSNDBUF;
          rl -= FTR_FSNDBUF;
          fbp=(char *)(fbp+FTR_FSNDBUF);
       } else {
          memcpy(fsd.databuf,fbp,rl);
          bfsize=rl;
          rl=0;
       }
       fsd.dgtype=FSREQ;
       dl=(sizeof(fsd)-FTR_FSNDBUF) + bfsize;
       memset(fsd.dglength,0,sizeof(fsd.dglength));
       sprintf(fsd.dglength,"%d",dl);

       fsd.sndstat=FTR_SNDOK;

       retcode=DGSnd(sockd,(char *)&fsd,dl);
       if (retcode < 0) {
          sprintf(msgword,"<       PID=%d> SndFile() === Pos. 2 : Send FSREQ DG Error.%d\n",Pid,errno);
          ProcessSys(msgword);
          close(fd);
          return(FDGSND_ERR);
       }

       /** bjli 2001.12.10  先不等待应答报文  begin  **/
       count++;
       if(count<=skip_count) continue; 
       /** bjli 2001.12.10  先不等待应答报文  end    **/
	continue ; 
	/* added by taoh 2002.05.14  不间断连续接收  */

       if ((retcode=DGRcv(sockd,FRRESP,(char *)&frd,wtime)) < 0) {
          dl=sizeof(fsd)-FTR_FSNDBUF;
          memset(fsd.dglength,0,sizeof(fsd.dglength));
          sprintf(fsd.dglength,"%d",dl);
          fsd.sndstat=FTR_SNDERR;
          DGSnd(sockd,(char *)&fsd,sizeof(fsd)-FTR_FSNDBUF);
          close(fd);
          sprintf(msgword,"<       PID=%d> SndFile() === Pos. 3 : Receive FRRESP DG Error.\n",Pid);
          ProcessSys(msgword);
          return(FDGRCV_ERR);
       }

       if (frd.rcvstat != FTR_RCVOK) {
          close(fd);
          sprintf(msgword,"<       PID=%d> SndFile() === Pos. 4 : FRRESP Status Error.\n",Pid);
          ProcessSys(msgword);
          return(FFRCV_ERR);
       }
    }
 }
 close(fd);
 if (rl < 0) {
    dl=sizeof(fsd)-FTR_FSNDBUF;
    memset(fsd.dglength,0,sizeof(fsd.dglength));
    sprintf(fsd.dglength,"%d",dl);
    fsd.sndstat=FTR_SNDERR;
    DGSnd(sockd,(char *)&fsd,sizeof(fsd)-FTR_FSNDBUF);
    sprintf(msgword,"<       PID=%d> SndFile() === Pos. 5 : Read File Error.\n",Pid);
    ProcessSys(msgword);
    return(FREAD_ERR);
 } 

 dl=sizeof(fsd)-FTR_FSNDBUF;
 memset(fsd.dglength,0,sizeof(fsd.dglength));
 sprintf(fsd.dglength,"%d",dl);
 fsd.sndstat=FTR_SNDEND;
 retcode = DGSnd(sockd,(char *)&fsd,sizeof(fsd)-FTR_FSNDBUF);

 /**  added bjli  2001.12.10 begin  **/
 if (retcode < 0) {	/** added bjli  2001.12.10  **/
       close(fd);
       sprintf(msgword,"<       PID=%d> SndFile() === Pos. 12 : Send FSREQ DG Error.\n",Pid);
       ProcessSys(msgword);
       return(FDGSND_ERR);
 }
 if(count>skip_count) count=skip_count;
 /** 补接收跳过的返回报文  **/
 /** for(; count>0; count--) **/
/* modify by taoh 2002.05.14 */
 if(1 ) 
 {
     if ((retcode=DGRcv(sockd,FRRESP,(char *)&frd,wtime)) < 0) {
         close(fd);
         sprintf(msgword,"<       PID=%d> SndFile() === Pos. 13 : Receive FRRESP DG Error.\n",Pid);
         ProcessSys(msgword);
         return(FDGRCV_ERR);
     }
 }
 /**  added bjli  2001.12.10 end  **/

 return(SUCCESS);
}

char *Encrypt(char *bp, int bsize, char *kp, int ksize)
{
 char *cp1,*cp2;
 int i;
 cp1=bp;

return 0;

 for (i=0; i<bsize; i++) {
    if ((i % ksize) == 0) cp2=kp;
    /****************
    *cp1++ ^= *cp2++;
    ******************/
    *cp1++ ; *cp2++;
 }
 return(cp2);
}
