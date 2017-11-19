#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "ftr.h"
#include "ftrerr.h"

/* 
 * 对接收的文件进行分析处理,调用后台处理程序 
 */
int ReceivedFileProc(char *ifile_namep)
{
  char pname[256], msgword[256];
  char procname[256] ;
  char cmd[200];
  char	ReqFile[256]; 
  char	RspFile[256]; 
  int i;
  char *ppp;
  int	rc ;

  sprintf(msgword,"<SERVER PID=%d> Execute Function will be Called to File : %s.\n",getpid(),ifile_namep);
  ProcessSys(msgword);

  memset(cmd, 0x00, 100);

  ppp=strrchr(ifile_namep,'/');
  if(ppp==NULL)
  	sprintf( ReqFile, "%s", ifile_namep ) ;
  else
  	sprintf( ReqFile, "%s", ppp+1 ) ;
  
  strcpy( procname, ReqFile ) ;
  ppp=strchr(procname,'.' ) ;
  if( ppp==NULL ) strcpy(procname,"fileproc" ) ;
  else ppp[0]='\0' ;
  
  if(FLAG[0]=='2') 
      sprintf(procname,"%s",CALL_NAME);
  
  sprintf( pname,"%s/%s", FPEXE_PATH, procname);
  
  if(access(pname,X_OK)!=0) {
      sprintf(msgword,"<SERVER PID=%d> Process File Name=%s is not Accessable.\n",getpid(), pname );
      ProcessSys(msgword);
      return -1;
  }
  
  sprintf( cmd, "%s %s ", pname, ifile_namep ) ;
  rc=system( cmd ) / 256;
  
  sprintf(msgword,"<SERVER PID = %d> Execute Command[%s], Return Code=%d.\n",getpid(), cmd, rc );
  ProcessSys(msgword);

  return(0);
}
