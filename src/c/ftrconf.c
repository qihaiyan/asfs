/*
**
**     SOURCE FILE: getconf.c
**     PROGRAM NAME: GetConf()
**     AUTHOR: Qiyue
**     CREATE DATE: 11/30/96
**     VERSION: 1.0.0a
**
**     FUNCTION:
**        1. Read a record from HOST CONFIGURATION FILE,
**        2. Fill the record to structure host_cfg
**        3. Return the pointer to strucure host_cfg to caller
**
**     INTERFACE DESCRIPTION:
**        1. interface:  struct host_cfg *GetConf(char *, int)
**        2. parameter:  
**                the first is a character string of address, it can be
**                a LOGICAL ADDRESS or a NETWORK ADDRESS.
**                the second is a integer to indicate type of the first
**                parameter address, it can be defined LOGICAL (0) or 
**                NETWORK (1).
**                (See File "ftr.h")
**        3. return value:
**                - a valid pointer to structure host_cfg;
**                - NULLP pointer, fail to get structure host_cfg  
**
*/

#include <fcntl.h>
#include "ftr.h"
#include "ftrerr.h"

#define LSIZE 256

struct host_cfg hostid;

struct host_cfg *GetConf(char *ap, int atype)
{
 extern char *getenv();

 char cfg_file[FNAMELEN];
 char line_buf[LSIZE];
 char *home_dirp;
 int  cfgfd,retcode,match;
 
/*
**    Reset Structure Variable hostid
*/

 memset((char *)&hostid,0,sizeof(hostid));

/*
**    Get Configuration File Name
*/
#if 0
 /** 2001.04.10  bjli  **/
 if(access(CFG_FILE,0)==0)
 	strcpy(cfg_file,CFG_FILE);
 else if(access(CFG_FILE2,0)==0)
 	strcpy(cfg_file,CFG_FILE2);
 else if(access(CFG_FILE3,0)==0)
 	strcpy(cfg_file,CFG_FILE3);
 else if(access(CFG_FILE0,0)==0)
 	strcpy(cfg_file,CFG_FILE0);
 else
 {
	char cmd[200];
	sprintf(cmd, "echo \"000 \tspot \tTCP \t100 \t3000\" >%s", CFG_FILE);
	system(cmd);
 	strcpy(cfg_file,CFG_FILE);
 }
#else	/* added by taoh 2002.06.28  */ 
 	strcpy(cfg_file,CFG_FILE);
#endif
 
/*
**    Open Configuration File 
*/

 if ((cfgfd=open(cfg_file,O_RDONLY)) < 0) return(NULLP);

/*
**    Get a Record from Configuration File or File Buffer
*/
 while ((retcode=GetLine(cfgfd,line_buf)) > 0) {

/*
**    Check the Record Match to Specified Address
*/
    if ((match=ChkMatch(ap,atype,line_buf)) == MATCH) {

/*
**    The Record is Correct, Convert the Record to HostID Structure
*/

       retcode=FillHostID(&hostid,line_buf);
       break;
    }

	/**  bjli  2001.11.28  **/
    if (strcmp(ap,"000")==0  
		&& (match=ChkMatch("310000000",atype,line_buf)) == MATCH) {

/*
**    The Record is Correct, Convert the Record to HostID Structure
*/

       retcode=FillHostID(&hostid,line_buf);
       break;
    }
 }
 close(cfgfd);

/*
**    If not Found Specified Record, Return NULL Pointer to Calller
*/

 if (retcode < 0) return(NULLP);
 return(&hostid);
}


/*
**    Get a Line Record from File Buffer or File 
*/

#define FSIZE  200000

char fbuff[FSIZE];
int fcount=0;
char *fbp;

int GetLine(int fd, char *buffer)
{
 int rl;
 int lline;
 char *cp;

 cp=buffer;
 lline=0;

/*
**    Check File Buffer if not Empty, If Buffer is Empty, Read File to Buffer
*/
 
 if (fcount <= 0) {
    fbp=fbuff;
    if ((rl=read(fd,fbuff,FSIZE)) <= 0) return(CFG_READ);   
    fcount=rl;
 }
 
/*
**    Copy One Line from File Buffer to Line Buffer
*/

 while ((*fbp != '\n') && (lline < LSIZE)) {
    *cp++=*fbp++;
    lline++;
    fcount--;
    if (fcount == 0) {
       fbp=fbuff;
       if ((rl=read(fd,fbuff,FSIZE)) <= 0) return(CFG_READ);   
       fcount=rl;
    }
 }

/*
**    Check if not over line buffer
*/

 if ((lline == LSIZE) && (*fbp != '\n')) return(-1);

 fcount--;
 fbp++;
 *cp=0;
 return(lline);

}


/*
**    Check Address String Match
*/


int ChkMatch(char *ap, int atype, char *lbp)
{
 char *cp1,*cp2;
 int l1,l2,retcode;
 char alstr[LOG_ADDR];
 char anstr[NET_ADDR];

/*
**    Check Address String to be LOGICAL ADDRESS or NETWORK ADDRESS
*/
 if (atype == LOGICAL) {

/*
**    If Address string is LOGICAL ADDRESS
*/

    cp1=lbp;

/*
**    Slip Leading Space or Tab Character
*/

    while ((*cp1 == ' ') || (*cp1 == '\t')) cp1++;
    
/*
**    Get LOGICAL ADDRESS String
*/

    cp2=alstr;
    l1=0;
    while ((*cp1 != ' ') && (*cp1 != '\t') && (l1 < LOG_ADDR-1)) {
       *cp2++=*cp1++;
       l1++;
    }
    *cp2=0;

/*
**    Compare specified address string with one record in configuration file
*/
    if ((l2=strcmp(ap,alstr)) == 0) retcode=MATCH;
    else retcode=NOMATCH;
 } else {

    cp1=lbp;

/*
**    Slip Leading Space or Tab Character
*/

    while ((*cp1 == ' ') || (*cp1 == '\t')) cp1++;

/*
**    Slip LOGICAL ADDRESS string
*/
    
    while ((*cp1 != ' ') && (*cp1 != '\t')) cp1++;

/*
**   Slip separater between LOGICAL ADDRESS and NETWORK ADDRESS
*/

    while ((*cp1 == ' ') || (*cp1 == '\t')) cp1++;

/*
**    Get NETWORK ADDRESS String
*/

    cp2=anstr;
    l1=0;
    while ((*cp1 != ' ') && (*cp1 != '\t') && (l1 < NET_ADDR-1)) {
       *cp2++=*cp1++;
       l1++;
    }
    *cp2=0;

/*
**    Compare specified address string with one record in configuration file
*/

    if ((l2=strcmp(ap,anstr)) == 0) retcode=MATCH;
    else retcode=NOMATCH;
 }

 return(retcode);

}


/*
**    Fill Host ID Structure from Configuration Record
*/

int FillHostID(struct host_cfg *hp, char *record)
{
 char *cp1,*cp2;
 char chbuff[80];
 int l,retcode;

/*
**    Get LOGICAL Address String from Record
*/
 cp1=record;
 while ((*cp1 == ' ') || (*cp1 == '\t')) cp1++;
 cp2=hp->Laddress;
 l=0;
 while ((*cp1 != ' ') && (*cp1 != '\t') && (l < LOG_ADDR-1)) {
    *cp2++=*cp1++;
    l++;
 }
 
 if ((l == LOG_ADDR-1) && (*cp1 != ' ') && (*cp1 != '\t')) return(CFG_ERR);
 *cp2=0;

/*
**    Get NETWORK Address String from Record
*/
      
 while ((*cp1 == ' ') || (*cp1 == '\t')) cp1++;
 cp2=hp->Naddress;
 l=0;
 while ((*cp1 != ' ') && (*cp1 != '\t') && (l < NET_ADDR-1)) {
    *cp2++=*cp1++;
    l++;
 }
 
 if ((l == NET_ADDR-1) && (*cp1 != ' ') && (*cp1 != '\t')) return(CFG_ERR);
 *cp2=0;

/*
**    Get Protocol Name String from Record
*/
 
 while ((*cp1 == ' ') || (*cp1 == '\t')) cp1++;
 switch (*cp1) {
    case 'S':
    case 's':

/*    For SNA Protocol     */

       hp->protoname=SNA;
       while ((*cp1 != ' ') && (*cp1 != '\t')) cp1++;
       break;
    case 'T':
    case 't':

/*    For SNA Protocol     */

       hp->protoname=TCP;
       while ((*cp1 != ' ') && (*cp1 != '\t')) cp1++;
       break;
    default:
       return(CFG_ERR);
 }

/*
**    Get Max Waiting Timer
*/

 while ((*cp1 == ' ') || (*cp1 == '\t')) cp1++;
 cp2=chbuff;
 l=0;
 while ((*cp1 != ' ') && (*cp1 != '\t') && (l < 7)) {
    if ((*cp1 < '0') || (*cp1 > '9')) return(CFG_ERR);
    *cp2++=*cp1++;
    l++;
 }
 *cp2=0;
 if ((l == 7) && (*cp1 != ' ') && (*cp1 != '\t')) return(CFG_ERR);
 hp->wtime=atoi(chbuff);

/*
**    Get Port No. String from Record
*/

 while ((*cp1 == ' ') || (*cp1 == '\t')) cp1++;
 cp2=hp->portno;
 l=0;
 while ((*cp1 != 0) && (*cp1 != ' ') && (*cp1 != '\t') && (l < PORT_NO-1)) {
    *cp2++=*cp1++;
    l++;
 }
 
 if ((l == PORT_NO-1) && (*cp1 != ' ') && (*cp1 != '\t') && (*cp1 != '\n'))
    return(CFG_ERR);
 *cp2=0;

 return(OK);

}
