
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE	2048			/* message size	*/
#define MSG_MODE	(0666 | IPC_CREAT)	/* message mode	*/

typedef struct	{
	long	mtype;			/* ��Ϣ����		*/
	unsigned char	mtext[MSG_SIZE];	/* ��Ϣ�ı�		*/
} MSGSTRUCT;

