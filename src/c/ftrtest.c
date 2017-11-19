
/*
**
**   Head File Definition
**
*/

#include <stdio.h>
#include "ftr.h"
#include "ftrerr.h"

main()
{
 int   selsw;

 selsw=0;
 while (selsw == 0) {
    system("clear");
    printf("\n\n\t\tSWT Client & Server Test Program\n");
    printf(" ______________________________________________________________________________\n\n\n\n");
    printf("\t\t  1. File Transmission Function Test\n\n");
    printf("\t\t  2. Quit\n\n\n\n");
    printf("\n\t  Please Input Your Choice, Then Press RETURN: ");
    scanf("%d",&selsw);
    switch (selsw) {
    case 1:
       ftrtest();
       break;
    case 2:
       system("clear");
       exit(0);
    default:
       printf("\t  ERROR: You Only Select 1, or 2\n\n");
       printf("\t  Please Press RETURN Continue ");
       getchar();
       getchar();
    }
    selsw=0;
 }
}

#define TR_TIME 20

ftrtest()
{
 char  sfn[80];
 char  cfn[80];
 char  saddr[NET_ADDR];
 char  trtime[TR_TIME];
 char  trdir,addrtype;
 int   loopflg,i,j,k;
 int   fret;
 int   ftrstime,ftretime;


 loopflg=0;
 while (loopflg == 0) {
    system("clear");
    printf("\n\n\t\tSWT CNTL Layer Client & Server Test Program\n");
    printf(" ______________________________________________________________________________\n");
    printf(" FTR TEST                   File  Transmission  Test                   FTR TEST\n");
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    printf("   Please Input Following Parameters: \n\n");
    printf("   Client File Name                 (0 < Length < 80) : ");
    scanf("%s",cfn);
    i=strlen(cfn);
    if ((i >= 80) || (i < 1)) {
       printf("\n\n\n\n\n\n\n\n\n\n"); 
       printf("");
       printf(" ______________________________________________________________________________\n");
       printf(" ERROR: File Name Length is 1 to 80, Please Press RETURN, Input Again.         \n");
       printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
       getchar();
       getchar();
       continue;
    }
    printf("   Server Host Address String       (0 < Length < 12) : ");
    scanf("%s",saddr);
    i=strlen(saddr);
    if ((i >= 12) || (i < 1)) {
       printf("\n\n\n\n\n\n\n\n\n"); 
       printf("");
       printf(" ______________________________________________________________________________\n");
       printf(" ERROR: Address String Length is 1 to 12, Please Press RETURN, Input Again.    \n");
       printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
       getchar();
       getchar();
       continue;
    }
    printf("   Server File Name                 (0 < Length < 80) : ");
    scanf("%s",sfn);
    i=strlen(sfn);
    if ((i >= 80) || (i < 1)) {
       printf("\n\n\n\n\n\n\n\n"); 
       printf("");
       printf(" ______________________________________________________________________________\n");
       printf(" ERROR: File Name Length is 1 to 80, Please Press RETURN, Input Again.         \n");
       printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
       getchar();
       getchar();
       continue;
    }
    printf("   Type of Host Address String (0-LOGICAL, 1-NETWORK) : ");
    scanf("%c",&addrtype);
    if (addrtype == 10) scanf("%c",&addrtype);
    if ((addrtype != LOGICAL) && (addrtype != NETWORK)) {
       printf("\n\n\n\n\n\n\n"); 
       printf("");
       printf(" ______________________________________________________________________________\n");
       printf(" ERROR: Address String Typy is Only 0 or 1, Please Press RETURN, Input Again.  \n");
       printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
       getchar();
       getchar();
       continue;
    }
    loopflg=1;
 }
 printf("\n");
 printf(" ______________________________________________________________________________\n");
 printf(" FTR TEST              Transmission Begin, Please Wait ...             FTR TEST\n");
 printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
 printf("   Transmission Result    : ");
 ftrstime=FtpGetTime(trtime);
fflush(stdout);
 fret=FSnd(cfn,sfn,saddr,addrtype);
 ftretime=FtpGetTime(trtime);
 if (fret == 0) printf("Success");
 else printf("Return Status = %d",fret);
 printf("\n");
 printf("   FTR Transmission Time : %d Second(s)\n",ftretime-ftrstime);
 printf("\n");
 printf("\tPlease Press RETURN, Return Main Menu.");
 getchar();
 getchar();
 return;
}

