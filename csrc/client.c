#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "mychat_model.h"

int main(int argc,char **argv)
{
	char ip[20] = "";
	char port[20] = "";

	if(get_array(ip,argv[1],"ip") == NULL)//IP获取函数：成功返回str，失败返回NULL
	{
		printf("IP获取失败！\n");
		exit(EXIT_FAILURE);
	}
	if(get_array(port,argv[1],"port") == NULL)//port获取函数：成功返回str，失败返回NULL
	{
		printf("port获取失败！\n");
		exit(EXIT_FAILURE);
	}

	client_create(ip,port);//客户端创建
	return 0;
}


