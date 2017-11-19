
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE	2048			/* message size	*/
#define MSG_MODE	(0666 | IPC_CREAT)	/* message mode	*/

typedef struct	{
	long	mtype;			/* 消息类型		*/
	unsigned char	mtext[MSG_SIZE];	/* 消息文本		*/
} MSGSTRUCT;

