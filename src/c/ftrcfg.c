
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

char	CFG_FILE[200], FILEIN_PATH[200], FILEOUT_PATH[200], FILEMOV_PATH[200], FILERCV_PATH[200], FPEXE_PATH[200], ERRORLOG[200], SYSLOG[200], IPC_MSGKEY[200], FLAG[200] , CALL_NAME[200];

/***********************************************************************
** º¯Êı¹¦ÄÜ:  ¶ÁÎÄ¼ş fname  ²éÕÒ name ¶ÔÓ¦Öµ answer(×Ö·û´®ĞÍ).        **
** ²Î¾   Êı:  ÎÄ¼şÃû, ²éÕÒ±äÁ¿, ±äÁ¿¶ÔÓ¦Öµ, ±äÁ¿³¤¶È.                 **
** ·µ »Ø Öµ:  0 , ²éÕÒ³É¹¦;                                           **
**	      -1, ²ÎÊı´í(±äÁ¿³¤¶ÈÌ«¶Ì);                               **
**	      -2, ´ò¿ªÎÄ¼ş´íÎó;                     		      **
**	      -3, ²éÕÒÊ§°Ü                  			      **
***********************************************************************/
int ReadParaStr(fname,name,answer,len)
char  	*fname, *name, *answer;  
int 	len;
{
	FILE 	*fp;
	char 	buf[500], s1[200], s2[200], s3[100], *p;
	int 	rc, count, len1, len2, len3;

	if(len<=3) return -1;

	fp=fopen(fname,"r");
	if(fp==NULL) {
		return -2;
	}

	memset( buf, 0, sizeof(buf) );
	memset( answer, 0, sizeof(answer) );
	while( !feof(fp) ) {
		if( fgets(buf, sizeof(buf)-1, fp) == NULL)
			break;
		if( buf[0] == '#' ) continue;	

		memset( s1, 0, sizeof(s1) );
		memset( s2, 0, sizeof(s2) );
		memset( s3, 0, sizeof(s3) );
		rc = sscanf( buf, "%s%s%s", s1, s2, s3);

		len1 = strlen(s1);
		len2 = strlen(s2);
		len3 = strlen(s3);
		memset( buf, 0, sizeof(buf) );

		p = &s1[0];
		count = 0;
		while( (*p != '=') && (count < len1-2) ) {
			p ++;
			count ++;
		}

		if( rc ){
			if( (*p == '=') && (strncmp(s1, name, count) == 0) ) {
				if(len1-count-1 > len)
					strncpy(answer, s1+count+1, len);
				else
					strcpy(answer, s1+count+1);
					
				fclose(fp);
				return 0;
			}
			else if( (s1[len1-1] == '=') && 
					(strncmp(s1, name, len1-1 ) == 0) && 
					(len2 > 1) ) {
				if(len2 > len)
					strncpy(answer, s2, len);
				else
					strcpy(answer, s2);
				fclose(fp);
				return 0;
			}
			else if( (s2[0] == '=') && (strcmp(s1, name) == 0)
					&& (len2 > 1) ) {
				if (len2-1 > len)
					strncpy(answer, s2+1, len);
				else
					strcpy(answer, s2+1);
				fclose(fp);
				return 0;
			}
			else if( strcmp(s1,name) == 0 && strcmp(s2, "=") == 0 ){
				s3[len] = 0;
				if(len3 > len){
					strncpy(answer, s3, len);
				}
				else
					strcpy(answer, s3);
				fclose(fp);
				return 0;
			}
		}		
	}
	fclose(fp);
	return -3;
}

/***********************************************************************
** º¯Êı¹¦ÄÜ:  ¶ÁÎÄ¼ş fname  ²éÕÒ name ¶ÔÓ¦Öµ answer(ÕûĞÍ).            **
** ²Î¾   Êı:  ÎÄ¼şÃû, ²éÕÒ±äÁ¿, ±äÁ¿¶ÔÓ¦Öµ, ±äÁ¿³¤¶È.                 **
** ·µ »Ø Öµ:  0 , ²éÕÒ³É¹¦;                                           **
**	      -1, ²ÎÊı´í(±äÁ¿³¤¶ÈÌ«¶Ì);                               **
**	      -2, ´ò¿ªÎÄ¼ş´íÎó;                     		      **
**	      -3, ²éÕÒÊ§°Ü                  			      **
***********************************************************************/
int ReadParaInt(fname,name,value)
char *fname, *name;  
int *value;
{
	char	tmp[50];
	int	rc;

	rc = ReadParaStr(fname, name, tmp, sizeof(tmp)-1);
	if(rc<0) return rc;
	*value=atoi(tmp);
	return rc;
}

/***********************************************************************
** º¯Êı¹¦ÄÜ:  ¶ÁÎÄ¼ş fname  ²éÕÒ name ¶ÔÓ¦Öµ²¢¸ÄÎª cname,	      **
**	      ÈçÎ´ÕÒµ½ name, Ôò²åÈë.			              **
** ²Î¾   Êı:  ÎÄ¼şÃû, ²éÕÒ±äÁ¿, ±äÁ¿¶ÔÓ¦Öµ, ±äÁ¿³¤¶È.                 **
** ·µ »Ø Öµ:  0 , ²éÕÒ³É¹¦;                                           **
**	      -1, ²ÎÊı´í(±äÁ¿³¤¶ÈÌ«¶Ì);                               **
**	      -2, ´ò¿ªÎÄ¼ş´íÎó;                     		      **
***********************************************************************/
int WritePara(fname, name, cname, len)
char  	*fname, *name, *cname;  
int 	len;
{
	
	FILE 	*fp;
	char 	buf[500];
	int 	count = 0, len2 = 0, rc = 0, i = 0, id;
	char 	file[200][500];

	memset( file, 0, sizeof(file) );
	memset( buf,  0, sizeof(buf) );

	fp = fopen(fname,"r");		/*´ò¿ª²ÎÊıÎÄ¼ş*/
	if(fp == NULL) {
		goto write_f;
	}

	len2 = strlen( name );

	while( !feof(fp) ) {
		if( fgets(buf, sizeof(buf)-1, fp) == NULL)
			break; 
		if( count > 200 ){
			fclose(fp);
			return -2;
		}
	
		for(i = strlen(buf)-1; i >= 0; i--)
			if(buf[i] == '\n') buf[i] = 0;
			else break;
		memcpy( file[count], buf, sizeof(buf) );
		
		for(id = 0; id < strlen(buf); id++)
			if(buf[id] == ' '|| buf[id] == '\t') continue;	
			else break;

		if( memcmp(buf+id, name, len2) == 0 &&
				(buf[len2] == '=' || buf[len2] == ' ' ||  
				 buf[len2] == '\t' ) ){ 
			for(id += len2; id < strlen(buf); id++)
				if(buf[id] == '#')	break;
			sprintf(file[count], "%s = %s %s", 
				name, cname, buf+id);	
			rc = 1;
		}
		count ++;
	}
	fclose(fp);

write_f:
	fp = fopen(fname,"w");		/*´ò¿ª²ÎÊıÎÄ¼ş*/
	if(fp == NULL) {
		return -1;
	}

	if( rc == 0) {
		sprintf(file[count], "%s = %s", name, cname);	
		count ++;
	}

	for( i = 0; i < count; i ++)
		fprintf(fp, "%s\n", file[i]);

	fclose(fp);
	return 	0;
}

int	TypeConfigVal(void)
{
	printf("CFG_FILE=%s\n",CFG_FILE);
	printf("FILEIN_PATH=%s\n",FILEIN_PATH);
	printf("FILEOUT_PATH=%s\n",FILEOUT_PATH);
	printf("FILEMOV_PATH=%s\n",FILEMOV_PATH);
	printf("FILERCV_PATH=%s\n",FILERCV_PATH);
	printf("FPEXE_PATH=%s\n",FPEXE_PATH);
	printf("ERRORLOG=%s\n",ERRORLOG);
	printf("SYSLOG=%s\n",SYSLOG);
	printf("IPC_MSGKEY=%s\n",IPC_MSGKEY);
	printf("FLAG=%s\n",FLAG);
	printf("CALL_NAME=%s\n",CALL_NAME);
	return 0 ;
}

int	ReadConfig(char *cfgname)
{
	int	rc ;

	rc=ReadParaStr(cfgname,"CFG_FILE",CFG_FILE,sizeof(CFG_FILE)-1);
	if(rc < 0) return -1001;
	rc=ReadParaStr(cfgname,"FILEIN_PATH",FILEIN_PATH,sizeof(FILEIN_PATH)-1);
	if(rc < 0) return -1002;
	rc=ReadParaStr(cfgname,"FILEOUT_PATH",FILEOUT_PATH,sizeof(FILEOUT_PATH)-1);
	if(rc < 0) return -1003;
	rc=ReadParaStr(cfgname,"FILEMOV_PATH",FILEMOV_PATH,sizeof(FILEMOV_PATH)-1);
	if(rc < 0) return -1004;
	rc=ReadParaStr(cfgname,"FILERCV_PATH",FILERCV_PATH,sizeof(FILERCV_PATH)-1);
	if(rc < 0) return -1005;
	rc=ReadParaStr(cfgname,"FPEXE_PATH",FPEXE_PATH,sizeof(FPEXE_PATH)-1);
	if(rc < 0) return -1006;
	rc=ReadParaStr(cfgname,"ERRORLOG",ERRORLOG,sizeof(ERRORLOG)-1);
	if(rc < 0) return -1007;
	rc=ReadParaStr(cfgname,"SYSLOG",SYSLOG,sizeof(SYSLOG)-1);
	if(rc < 0) return -1008;
	rc=ReadParaStr(cfgname,"IPC_MSGKEY",IPC_MSGKEY,sizeof(IPC_MSGKEY)-1);
	if(rc < 0) return -1009;
	rc=ReadParaStr(cfgname,"FLAG",FLAG,sizeof(FLAG)-1);
	if(rc < 0) return -1010;
	rc=ReadParaStr(cfgname,"CALL_NAME",CALL_NAME,sizeof(CALL_NAME)-1);
	if(rc < 0) return -1011;
	return 0 ;
}
