
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

/*
**
**    Function GettyName()
**
*/

int GettyName(ttyp)
char *ttyp;
{
 char dd[20];
 char *zd;

 zd=&dd[0];
 zd=(char *)ttyname(0);
 strcpy(ttyp,zd);
 return (strlen(ttyp));
}

/*
**
**   Fucntion GetUsrName()
**
*/

int GetUsrName(up)
char *up;
{
 cuserid(up);
 return(strlen(up));
}

/*
**
**   Fucntion FtpGetTime()
**
*/

time_t FtpGetTime(tp)
char *tp;
{
 extern struct tm *localtime();
 struct tm *timp;
 time_t clk;
 char   *cp;
 int    tl;

 clk=time((time_t *)0);
 timp=localtime(&clk);
 sprintf(tp,"%4d/%2d/%2d-%2d:%2d:%2d",timp->tm_year+1900,timp->tm_mon+1,
         timp->tm_mday,timp->tm_hour,timp->tm_min,timp->tm_sec);
 tl=strlen(tp);
 cp=tp;
 while (tl > 0) {
    if (*cp == ' ') *cp='0';
    cp++;
    tl--;
 }
 return(clk);
}
