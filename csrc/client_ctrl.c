#include "all_head.h"
#include "mychat_model.h"
#include "mychat_view.h"

void client_ctrl_1(int clientfd)//客户端 主线程1级控制层
{
	char name[32] = "";
	int choose = 0;
	while(1)
	{
		sleep(1);
		system("clear");
		client_view_1();
		printf("\033[0;34m请输入您的选择：\033[0;m\n");
		scanf("%d",&choose);
		if(getchar()!= '\n')
		{
			printf("无效选择！\n");
			continue;
		}
		switch(choose)
		{
			case REG_J:
				client_user_regist(clientfd);
				break;
			case LOG:
				client_user_login(clientfd,name);
				break;
			case EXIT:
				client_user_exit(clientfd,name);
				return;
			default:
				printf("\033[0;31m输入错误！\033[0;m\n");
				break;
		}
	}
}
void client_ctrl_2(int clientfd, char* name)//客户端 主线程2级控制层
{	
	//创建心跳线程
	pthread_t pthread_heart1;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	Nmsg nmsg = {};
	nmsg.clientfd = clientfd;
	strcpy(nmsg.name,name);
	if(pthread_create(&pthread_heart1,&attr,client_heart,(void*)&nmsg) != 0)
	{
		fprintf(stderr,"文件名：%s 函数名：%s 行号：%d 错误原因：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;	
	}
	pthread_heart = pthread_heart1;


	int choose = 0;
	while(1)
	{
		sleep(1);
		system("clear");
		client_view_2(name);
		printf("\033[0;34m请输入您的选择：\033[0;m\n");
		scanf("%d",&choose);
		if(getchar()!= '\n')
		{
			printf("选择无效！\n");
			continue;
		}
		switch(choose)
		{
			case 1:
				client_user_shell(clientfd,name);
				break;
			case 2:
				client_user_check_online(clientfd,name);
				break;
			case 3:
				client_user_chat(clientfd,name);
				break;
			case 4:
				client_user_multichat(clientfd,name);
				break;
			case 0:
				client_user_quit(clientfd,name);
				return;
		}
	}
}

void* client_heart(void* arg)
{
	Nmsg* p = (Nmsg*)arg;
	Cmsg heart_message = {};
	heart_message.type = HEART;
	strcpy(heart_message.name,p->name);
	time_t t;
	while(1)
	{
		t = time(NULL);
		heart_message.second = (int)t;
		strftime(heart_message.time,128,"%H%M%S",localtime(&t));
		if(write(p->clientfd,&heart_message,sizeof(Cmsg)) <= 0)
		{
			printf("心跳包发送失败\n");
			return NULL;
		}
//		printf("heart packet success\n");
		sleep(3);
	}
	return NULL;
}
