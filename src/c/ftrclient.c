
/*
**   This  Module  is  Main  Function of FTR Client.
**   Send or Receive file from FTR Server.
*/

#include <stdio.h>
#include <time.h>
#include "ftr.h"
#include "ftrerr.h"

#define TR_TIME 200

main(argc, argv)
int	argc;
char 	** argv ;
{
 char  sfn[80];
 char  cfn[80];
 char  saddr[NET_ADDR];
 char  trdir,addrtype;
 int   loopflg,i,j,k;
 int   fret;
 char	mode_new[20];	/**  "R" 接收，其他发送  **/
 char *pname, cfgname[200];

 if ( argc < 3 ) {
	printf ( "Usage: %s addrcode file_name \n", argv[0] ) ;
	exit ( 1 ) ;
 }

 if( (pname=strrchr(argv[0],'/')) ==NULL ) pname=argv[0];
 else pname++ ;
 sprintf(cfgname,"./cfg/%s.cfg",pname);

 fret=ReadConfig(cfgname);
 if(fret<0 ) {
	fprintf(stderr,"读配置文件%s出错,fret=%d\n",cfgname,fret); 
	return -1;
 }

 strcpy ( cfn, argv[2] ) ;
 strcpy ( sfn, argv[2] ) ;
 strcpy ( saddr, argv[1] ) ;
 if(argc>=4) 
	strcpy ( mode_new, argv[3] ) ;
 else
	strcpy ( mode_new, "");

 addrtype = '0' ;

 fret=Fftp(cfn,sfn,saddr,addrtype, mode_new);

 printf("File Transfer End, Status = %d.",fret);

 return fret;
}
