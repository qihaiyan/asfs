
/*
**     FILE NAME: ftr.h
**
**     FUNCTION:
**           Header file defined for file transmission function in
**           BJGC Project.
**
**     CONTENTS:
**           1. datagram structure definition;
**           2. description of record format in HOST CONFIGURATION file;
**           3. description of HOST CONFIGURATION structure;
**           4. all symble definition to be used.
*/

/*
**
**     FTR Datagram Format
**
*/

#define NOTCHK_DGTYPE   '\0'    /* No care datagram type use only DGRCV */
#define FPUTREQ         'p'     /* FTR put file request datagram        */
#define FGETREQ         'g'     /* FTR get file request datagram        */
#define FLISTREQ        'l'     /* FTR list dir request datagram        */
#define FRESP           'a'     /* FTR response datagram                */
#define FSREQ           '1'     /* FTR file sending request datagram    */
#define FRRESP          '2'     /* FTR file receiving response datagram */

#define STD_DGSIZE       5
#define FNAMELEN            256
#define FKEYSIZE         6

/*
**    FTR Request Datagram Format
*/

struct ftr_req {
   char dgtype;                 /* Datagram type                        */
   char dglength[STD_DGSIZE];   /* Datagram length                      */
   char sfile[FNAMELEN];           /* Server file name                     */
   char filekey[FKEYSIZE];      /* Encrypt file key                     */
};

/*
**    FTR Response Datagram Format
*/

struct ftr_resp {
   char dgtype;                 /* Datagram type                        */
   char dglength[STD_DGSIZE];   /* Datagram length                      */
   char respstat;               /* Response status                      */
};

/*
**   FTR Response Status Definition
*/

#define FTR_SERVEROK    '0'     /* Server checking OK, ready for serving */
#define FTR_SDISABLE    '1'     /* Server disable to serve              */
#define FTR_SFOPERR     '2'     /* Server file open or creation error   */

/*
**   FTR File Sending Request Datagram
*/

#define FTR_FSNDBUF     1024    /* File sending request datagram data   */
                                /* area length                          */

struct ftr_fsndreq {
   char dgtype;                 /* Datagram type                        */
   char dglength[STD_DGSIZE];   /* Datagram length                      */
   char sndstat;                /* Sending status                       */
   char databuf[FTR_FSNDBUF];   /* Sending data area                    */
};

/*
**   FTR File Sending Status Definition
*/

#define FTR_SNDEND      '0'     /* Finish to send a file normally       */
#define FTR_SNDOK       '1'     /* Send proceeding normally             */
#define FTR_SNDERR      '2'     /* Sending terminate abnormally         */

/*
**   FTR File Receiving Response Datagram
*/

struct ftr_frcvresp {
   char dgtype;                 /* Datagram type                        */
   char dglength[STD_DGSIZE];   /* Datagram length                      */
   char rcvstat;                /* Receiving status                     */
};

/*
**   FTR File Receiving Status Definition
*/

#define FTR_RCVEND      '0'     /* Finish to receive a file normally    */
#define FTR_RCVOK       '1'     /* Receiving proceeding normally        */
#define FTR_RCVERR      '2'     /* Receiving abnormally, terminate to   */
                                /* receive a file                       */

#define FBUFSIZE        FTR_FSNDBUF*100   /* Size of File Buffer        */

/*
**
**     Host Configuration File Format
**
**  Logical-address	Host-address	Protocol-name	Wtime	Port-No
**
**  Example:
**
**    1. For TCP/IP
**
**  	0000621000	ICBCHOST		TCP	100	6588
**      (Primer-BIN)	(Host-Name)	    (Protocol)  (Sec.)  (Server-Port)
**
**
**    2. For SNA
**
**  	0000621000	Side_Info profile	SNA	100	FTPSVR	
**	(Primer-BIN)    (NetName.LU-Name)   (Protocol)  (Sec.)  (TP-Name)
**
*/

#define HOMEFTP      "."
#define OSITEFTP     "."

#define	FILE_SUFFIX	".filetmp"

/* 
 * 以下变量采用配置文件
 * added by taoh 2002.09.14 
 */
#if 0

#define CFG_FILE     (OSITEFTP "/db/HOST.CFG")
#define FILEIN_PATH  (OSITEFTP "/db/ftprcv")
#define FILEOUT_PATH (OSITEFTP "/db/ftpsend")
#define FILEMOV_PATH (HOMEFTP "/db/ftpout")
#define FILERCV_PATH (HOMEFTP "/db/ftpin")
#define FPEXE_PATH (HOMEFTP "/bin/ftpproc")

#define ERRORLOG (HOMEFTP "/log/netftp.err")
#define SYSLOG   (HOMEFTP "/log/netftp.log")

#define IPC_MSGKEY   (HOMEFTP "/db")
#else 
extern	char	CFG_FILE[FNAMELEN], FILEIN_PATH[FNAMELEN], FILEOUT_PATH[FNAMELEN], FILEMOV_PATH[FNAMELEN], FILERCV_PATH[FNAMELEN], FPEXE_PATH[FNAMELEN], ERRORLOG[FNAMELEN], SYSLOG[FNAMELEN], IPC_MSGKEY[FNAMELEN] , FLAG[FNAMELEN] , CALL_NAME[FNAMELEN];

#endif

/*
**      Host Configuration Structure 
*/

#define LOG_ADDR        12     /* Logical address length               */
#define NET_ADDR        44     /* Host address length                  */
#define PORT_NO         44     /* Port number length                   */

struct host_cfg {
   char Laddress[LOG_ADDR];              /* Logical address              */
   char Naddress[NET_ADDR];              /* Network address              */
   char portno[PORT_NO];                 /* Port No.                     */ 
   int  protoname;                       /* Protocol name                */
   int  wtime ;                          /* Timeout Timer                */
};

#define NULLP (struct host_cfg *)0

/*
**     Protocol Name Field Definition
*/

#define    UNDEF_PROTO       0           /* Undefine protocol name      */
#define    TCP               1           /* TCP/IP protocol             */
#define    SNA               2           /* SNA protocol                */

/*
**     Address Type Flag Definition 
*/

#define LOGICAL         '0'    /* Logical address flag                 */
#define NETWORK         '1'    /* Network address flag                 */

#define MATCH           0      /* Match Flag                           */
#define NOMATCH         1      /* Not match Flag                       */

#define DGMTU           1024   /* Datagram Max Transmission Unit       */
#define DTIME           3      /* Delay time tick (in sec.) for close  */
                               /* a socket                             */

char * Encrypt();
extern char *basename(char *);

#  define abs_path(x)    ((x)[0] == '/')
#  define dos2unix_path(x) do {char *p=x;while(*p){if(*p=='\\') *p='/'; *p++;}} while(0)

