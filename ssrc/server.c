#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "mychat_model.h"


int main(int argc, char **argv)
{
	userlink = (Userlink*)calloc(1,sizeof(Userlink));//为用户链表创建头节点空间
	onlink = (Onlink*)calloc(1,sizeof(Onlink));//为在线用户链表创建头节点空间
	userlink_load(userlink);
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

	if(getcwd(first_path,1024) == NULL)
	{
		printf("目录获取失败!\n");
		return 0;
	}
	server_create(ip,port);//服务器创建


	return 0;
}
