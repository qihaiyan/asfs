#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/param.h>

#include <sys/ioctl.h>
#include <sys/wait.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "ftr.h"
#include "ftrerr.h"

/*
**
**   Function Name:
**        trserv()
**
**   Function:
**        This is A Main Server of OFN System. It is Called by
**        Server Daemon Header Program. It Completes All Serving
**        Functions.
**
**   Function Interface Definition:
**        int trserv(local_address,address-type)
**
**   Remark:
**        trserv         function name, integer.
**
**   External Variable:
**       void  (*rsvsig[RSVSIG])(int)    routine entry to reserve signal process.
**
**   Return:
**       None.
**        
*/

/*
**   Globle and/or External Variable Definition
*/

#define    RSVSIG     10
#define    RSVTERM    0
#define    RSVCLD     1
#define    RSVPIPE    2
#define    MAXWAIT    10

void  (*rsvsig[RSVSIG])(int);          /* Reserve Old Signal Process Entry       */
pid_t  g_pid;

/*
**   Function daemon_init()
*/

int daemon_init(void)
{
    pid_t pid;
    int i;
    if ( (pid=fork()) < 0 ){
        return -1;
    }
    else if ( pid != 0 )
        exit(0);

    setsid();
    umask(0);
    for (i=1;i<=64;i++){
        close(i);
    }
    open("/dev/null",O_RDONLY);
    open("/dev/null",O_WRONLY);
    open("/dev/null",O_WRONLY);

    return 0;
}

int serv_proc(int servsock, struct host_cfg *lhp, char *msgword)
{
    int fret=0;
    struct host_cfg local;
    struct ftr_req freq;
    struct ftr_resp fresp;
    int ftrfd;
    char sfile[FNAMELEN];
    char *p, sfile2[FNAMELEN];

    local = *lhp;

    /*   Clear All Datagram and Other Buffers, to Be Ready for Receiveing   */

    memset(&freq,0,sizeof(freq));
    memset(&fresp,0,sizeof(fresp));

    for (;;) {

        /*   Receive A Transmission Request Datagram from Client   */
        fret=DGRcv(servsock,FPUTREQ,(char *)&freq,MAXWAIT);
        if (fret < 0) {
            /* Receive Error, Inform Client This Error; Then I Exit */
            sprintf(msgword,"<SERVER PID=%d> Can't receive datagram .\n",getpid());
            ProcessSys(msgword);
            close(servsock);
            exit(SDGRCV_ERR);
        }
        else if (fret == DGRCV_END) { /* Client CLosed the Connection */
            sprintf(msgword,"<SERVER PID=%d> Connection has been closed .\n",getpid());
            ProcessSys(msgword);
            close(servsock);
            exit(OK);
        }

        /*
        **   Perform A Service to Request from Client in Datagram Type (Service Type)
        */

        fresp.dgtype=FRESP;
        sprintf(fresp.dglength,"%d",sizeof(fresp));

        if (freq.dgtype == FPUTREQ )  /* 客户端向服务器发送文件  */
        {
            sprintf(msgword,"<SERVER PID=%d> Client Send Put File Name: %s.\n",getpid(),freq.sfile);
            ProcessSys(msgword);

            dos2unix_path(freq.sfile);

            if(abs_path(freq.sfile))
            {
                strcpy(sfile,freq.sfile);
            }
            else
            {
                strcpy(sfile,FILEIN_PATH);
                strcat(sfile,"/");
                strcat(sfile,freq.sfile);
            }

            /* 传送时用临时文件名 */

            sprintf(sfile2,"%s%s",sfile,FILE_SUFFIX ) ;

            unlink(sfile);
            ftrfd=open(sfile2,O_CREAT|O_WRONLY|O_TRUNC) ;
            if (ftrfd < 0) {
                fresp.respstat=FTR_SFOPERR;
                fret=DGSnd(servsock,(char *)&fresp,sizeof(fresp));
                sprintf(msgword,"<SERVER PID=%d> Open/Create File %s Error.\n",getpid(),sfile);
                ProcessSys(msgword);
                close(servsock);
                exit(SFOPEN_ERR);
            }
            chmod(sfile2,0644);
        }
        else if (freq.dgtype == FGETREQ)  /* 客户端从服务器接收文件 */
        {
            sprintf(msgword,"<SERVER PID=%d> Client Send Get File Name: %s.\n",getpid(),freq.sfile);
            ProcessSys(msgword);

            dos2unix_path(freq.sfile);

            if(abs_path(freq.sfile))
            {
                strcpy(sfile,freq.sfile);
            }
            else
            {
                strcpy(sfile,FILEOUT_PATH);
                strcat(sfile,"/");
                strcat(sfile,freq.sfile);
            }
            ftrfd=0;
        }
        else if (freq.dgtype == FLISTREQ) /* LIST DIR REQ */
        {
            sprintf(msgword,"<SERVER PID=%d> Client Send List Dir Path : [%s].\n",getpid(),freq.sfile);
            ProcessSys(msgword);

            dos2unix_path(freq.sfile);

            if(abs_path(freq.sfile))
            {
                strcpy(sfile,freq.sfile);
            }
            else
            {
                strcpy(sfile,FILEOUT_PATH);
                strcat(sfile,"/");
                strcat(sfile,freq.sfile);
            }
        }
        else {
            sprintf(msgword,"<SERVER PID=%d> Datagram Type Error, DGTYPE=%d.\n",getpid(),freq.dgtype);
            ProcessSys(msgword);
            close(servsock);
            exit(SDGRCV_ERR);
        }

        fresp.respstat=FTR_SERVEROK;
        if ((fret=DGSnd(servsock,(char *)&fresp,sizeof(fresp))) < 0) {
            sprintf(msgword,"<SERVER PID=%d> Send Response DG Error.\n",getpid());
            ProcessSys(msgword);
            close(servsock);
            close(ftrfd) ;
            exit(SFRCV_ERR);
        }

        /* Recieved a PUT FILE Request */
        if (freq.dgtype == FPUTREQ ) {
            fret=RcvFile(servsock,ftrfd,freq.filekey,FKEYSIZE,local.wtime);
            if (fret<0) {
                sprintf(msgword,"<SERVER PID=%d> Receive File Error.\n",getpid());
                ProcessSys(msgword);
            }
            else {
                /* 将临时文件名改成正式文件名 */
                if(rename(sfile2, sfile) != 0) {
                    sprintf(msgword,"<SERVER PID=%d> Rename File From %s to %s Error.\n",getpid(),sfile2,sfile);
                    ProcessSys(msgword);
                    close(servsock);
                    close(ftrfd);
                    exit(SFRCV_ERR);
                }

                sprintf(msgword,"<SERVER PID=%d> Receive File Successfully.\n",getpid());
                ProcessSys(msgword);

                if(FLAG[0]=='1' || FLAG[0]=='2')     /* 收到文件处理标志 */
                    ReceivedFileProc(sfile);
            }
        }
        /* Recieved a GET FILE Request */
        else if (freq.dgtype == FGETREQ ) {
            fret=SndFile(servsock,sfile,freq.filekey,FKEYSIZE,local.wtime);
            if (fret<0) {
                sprintf(msgword,"<SERVER PID=%d> Send File Error fret=%d.\n",getpid(), fret);
                ProcessSys(msgword);
            }
            else {
                sprintf(msgword,"<SERVER PID=%d> Send File Successfully.\n",getpid());
                ProcessSys(msgword);
            }
        }
        else if (freq.dgtype == FLISTREQ) { /* LIST DIR REQ */
            fret=DirList(servsock,sfile,local.wtime);
            if (fret<0) {
                sprintf(msgword,"<SERVER PID=%d> Send Dir List Error fret=%d.\n",getpid(), fret);
                ProcessSys(msgword);
            }
            else {
                sprintf(msgword,"<SERVER PID=%d> Send Dir List Successfully.\n",getpid());
                ProcessSys(msgword);
            }
        }
        else {
            fret=-1;
        }
        close(ftrfd);
    }

    return fret;
}

/*
**   Function trserv()
*/

trserv(char *local_address, char *local_adtype)
{
    /*
    **   External & Internal Function Definition
    */

    extern struct hostent *gethostaddr();
    extern void serverr();
    extern struct host_cfg *GetConf();
    int    optvar,optlen;
    void termserv();
    char msgword[256];

    /*
    **   External & Internal Variable Definition
    */

    struct hostent *hp;
    struct sockaddr_in server,client;
    struct host_cfg local,*lhp;
    int mainsock,servsock;
    int lengthc,lengths;
    int fret;
    char latype;
    
    /* Trap Process Terminating Signal SIGTERM */

    rsvsig[RSVTERM]=signal(SIGTERM,termserv);

    /*   Get Current Child Process Exiting Signal SIGCLD and Ignore It   */

    rsvsig[RSVCLD]=signal(SIGCLD,SIG_IGN);

    /*   Get Current Signal SIGPIPE and Ignore It   */

    rsvsig[RSVPIPE]=signal(SIGPIPE,SIG_IGN);
 
    /*   Get Local Host ID   */

    g_pid=getpid();

    if (strcmp(local_adtype,"LOGICAL") == 0) latype=LOGICAL;
    else latype=NETWORK;
    if ((lhp=GetConf(local_address,latype)) == NULLP) { 
        sprintf(msgword,"<SERVER PID=%d> Can't get Server Configuration.\n",g_pid);
        ProcessSys(msgword);
        apisrvext();
        exit(CONFIG_ERR);
    }
    local=*lhp;

    /*   Create a TCP Socket Descriptor (in Internet & Stream)   */

    if ((mainsock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0) {
        sprintf(msgword,"<SERVER PID=%d> Can't create A Socket.\n",g_pid);
        ProcessSys(msgword);
        apisrvext();
        exit(SOCKOPN_ERR);
    }

    /*
    **  Allow socket option SO-REUSEADDR.
    */

    optlen=sizeof(optvar);
    optvar=1;
    fret=setsockopt(mainsock,SOL_SOCKET,SO_REUSEADDR,(char *)&optvar,optlen);
    if (fret < 0) {
        sprintf(msgword,"<SERVER PID=%d> Can't set Socket Option.\n",g_pid);
        ProcessSys(msgword);
        printf("\b\b\b\b\b\b\b");
        printf("FTR Server Start Up Fault: Can't set Socket Option.\n");
        apisrvext();
        exit(SOCKSET_ERR);
    }

    /*   Set Socket Address Structure Variable of Server   */

    memset(&server,0,sizeof(server));
    server.sin_port=htons(atoi(local.portno)); 
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=htonl(INADDR_ANY);

    /*   Bind My Address, to Client Can Connect with Me   */

    if ((bind(mainsock,(struct sockaddr *)&server,sizeof(server))) < 0) {
        sprintf(msgword,"<SERVER PID=%d> Can't bind Socket's Address.\n",g_pid);
        ProcessSys(msgword);
        printf("\b\b\b\b\b\b\b");
        printf("FTR Server Start Up Fault: Can't bind Socket's Address.\n");
        close(mainsock);
        apisrvext();
        exit(BINDSOCK_ERR);
    }

    lengths=sizeof(server);
    lengthc=sizeof(client);
    getsockname(mainsock,(struct sockaddr *)&server,&lengths);

    /*   Bind Server Port   */

    if ((ntohs(server.sin_port)) == 0) {
        sprintf(msgword,"<SERVER PID=%d> Can't convert Network Address.\n",g_pid);
        ProcessSys(msgword);
        printf("\b\b\b\b\b\b\b");
        printf("FTR Server Start Up Fault: Can't convert Network Address.\n");
        close(mainsock);
        apisrvext();
        exit(ADCONV_ERR);
    }

    /* Listen Client Request(s) (Max 5) to Me in Uncondition Loop */

    sprintf(msgword,"<SERVER PID=%d> FTR Server Start Up Successfully.\n",getpid());
    ProcessSys(msgword);
    g_pid=getpid();
    listen(mainsock,5);

    /*  Wait Accepting for A Client Process Request Connection */
    for ( ; ; ) {
        if ((servsock=accept(mainsock,(struct sockaddr *)&client,&lengthc)) < 0) {
            sprintf(msgword,"<SERVER PID=%d> accept err.\n",getpid());
            ProcessSys(msgword);
               continue;
        }

        /*
        **   Fork A Serving Child Process to Perform Specific Service
        **   in Concurrent Mode
        */
     
        if (fork() == 0)
        {
            /*  Child Process Close Old Socket Descriptor of Parent Process  */
            close(mainsock);

            signal(SIGCLD,rsvsig[RSVCLD]);

            /*   Get Client Host Address to Connect Local Host   */

            hp=gethostbyaddr(&client.sin_addr,sizeof(struct in_addr),client.sin_family);
            if (hp == 0) {
                close(servsock);
                sprintf(msgword,"<SERVER PID=%d> Can't get Client Address.\n",getpid());
                ProcessSys(msgword);
                perror("gethostbyaddr") ;
                exit(GETCAD_ERR);
            }

            /* The Server Processing Routine */
            serv_proc(servsock,lhp,msgword);

            if (fret < 0) {
                fret=SFRCV_ERR;
            }

            close(servsock);

            exit(fret);
        }
    
        /*
        **   Parent Process Close Accepting Socket to Connect 
        **   Client Host Request, Entry The Next Loop.
        */

        close(servsock);
     }

     signal(SIGCLD,rsvsig[RSVCLD]);

     exit(0);
}

/*
**   Signal SIGTERM Trap Processing Routine termserv()
*/

void termserv()
{
     extern int g_pid;
     register int i;
     char msgword[256];

     if (getpid() != g_pid) exit(0);

     sprintf(msgword,"<SERVER PID=%d> FTR Server Daemon Exit Normally.\n",getpid());
     ProcessSys(msgword);
     for (i=3; i<20; i++) close(i);
     exit(0);
}

apisrvext()
{
     register int i;
     char msgword[256];

     if (getpid() != g_pid) return;

     sprintf(msgword,"<SERVER PID=%d> FTR Server Daemon Exit Abnormally.\n",getpid());
     ProcessSys(msgword);
     for (i=3; i<20; i++) close(i);
     return;
}
