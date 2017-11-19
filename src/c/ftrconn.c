
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "ftr.h"
#include "ftrerr.h"

int MkConnect(char *naddress,int tcpport)
{
 int k,sockd,Pid;
 struct sockaddr_in client;
 struct hostent *hp, *gethostbyname();
 char msgword[256];

/*   Open A TCP STREAM Socket for Connection to Destination Host   */

 Pid=getpid();
 if ((sockd=socket(AF_INET,SOCK_STREAM,0)) < 0) {
    sprintf(msgword,"<CLIENT PID=%d> MkConnect() === Pos. 1 : socket() Create Socket Error.\n",Pid);
    ProcessSys(msgword);
    return(SOCKOPN_ERR);
 }
 memset(&client,0,sizeof(client));
 client.sin_family=AF_INET;
 client.sin_port=htons(tcpport);
 hp=gethostbyname(naddress);
 if (hp == 0) {
    sprintf(msgword,"<CLIENT PID=%d> MkConnect() === Pos. 2 : gethostbyname() Host Name Error.\n",Pid);
    ProcessSys(msgword);
    return(BADNAME_ERR);
 }

 memcpy(&client.sin_addr,hp->h_addr,hp->h_length); 
 if ((k=connect(sockd,(struct sockaddr *)&client,sizeof(client))) < 0) {
    sprintf(msgword,"<CLIENT PID=%d> MkConnect() === Pos. 3 : connect() Connection Error.\n",Pid);
    ProcessSys(msgword);
    return(UNCON_ERR);
 }
 sprintf(msgword,"<CLIENT PID=%d> MkConnect() : OK!\n",Pid);
 ProcessSys(msgword);
 return(sockd);
}

int Disconnect(sockd)
int  sockd;
{
 struct linger tt;

/*
**
**   Set Delay Timer to Send Rest Data in Sending Buffer, After
**   Specified Socket to Be Closed.
**
*/

/* zero = OFF nozero = ON */

 tt.l_onoff=1;

/* delay time in seconds */

 tt.l_linger=DTIME;
 
/*   Close Specified Socket Descriptor   */

 close(sockd); 
 return;

}
