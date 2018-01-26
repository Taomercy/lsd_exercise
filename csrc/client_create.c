#include "all_head.h"
#include "mychat_model.h"
#include "mychat_ctrl.h"
char* get_array(char* str, char* filename, char* value)
{
	FILE* fp = fopen(filename,"r");
	if(fp == NULL)
	{
		return NULL;
	}
	while(1)
	{
		char buff_line[1024] = "";
		char buff1[1024] = "";
		char buff2[1024] = "";
		char buff3[1024] = "";
		if(fgets(buff_line,1024,fp) == NULL)
		{
			break;
		}
		sscanf(buff_line,"%s" "%s" "%s",buff1,buff2,buff3);
		if(strcmp(buff1,value) == 0)
		{
			fclose(fp);
			strcpy(str,buff3);
			return str;
		}
	}
	fclose(fp);
	return NULL;
}
void msgq_create(void)//创建消息队列
{
//	printf("正在创建消息队列\n");
	srand(time(0));
	int array[3] = {0};
	int num = 0,i = 0, t = 0;
	for(i = 0; i < 3;++i)
	{
		t = 0;
		while(t < 100000)
		{
			num = rand()%100;
			t++;
		}
		array[i] = num;
	}
	key_t key = ftok("./hosts.txt",array[0] * array[1] * array[2]);
	if(key == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);
	}
	msgqid = msgget(key,IPC_CREAT | IPC_EXCL | 0664);
	if(msgqid == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);	
	}
//	printf("创建消息队列完成，msgqid:%d\n",msgqid);
}


void client_create(char* ip, char* port)//客户端创建
{
//创建消息队列
	msgq_create();
//创建客户端
	int clientfd = socket(AF_INET,SOCK_STREAM,0);
	if(clientfd == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in s_addr;
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(atoi(port));
	if(inet_pton(AF_INET,ip,&s_addr.sin_addr.s_addr) == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(connect(clientfd,(struct sockaddr*)&s_addr,sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);
	}

	//创建线程

	pthread_t pthread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	int err = 0;
	if((err = pthread_create(&pthread,&attr,client_receive,(void*)clientfd))!= 0)
	{
		fprintf(stderr,"文件名：%s 函数名：%s 行号：%d 错误原因：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);
	}

	//调用客户端主线程控制层
	client_ctrl_1(clientfd);
	
	close(clientfd);
	return;
}

void* client_receive(void* arg)//客户端数据接收线程，并同步到主线程
{
//	printf("到达客户端数据接收线程！\n");
	int clientfd = (int)arg;
	while(1)
	{
		Cmsg msg3 = {};
		if(read(clientfd,&msg3,sizeof(Cmsg)) <= 0)//接收客户端数据包
		{
			printf("\n");
			fprintf(stderr,"服务器关闭！\n");
			if(msgctl(msgqid,IPC_RMID,0) == -1)
			{
				fprintf(stderr,"%s\n",strerror(errno));
			}
			exit(1);
		
		}
		if(msg3.type == SHELL)
		{
			printf("\033[0;34m%s\033[0;m\n",msg3.text);
			continue;
		}
		if(msg3.type == CHAT)
		{
			printf("\033[0;34m\t\t%s\t:%s\033[0;m\n",msg3.text,msg3.name);
			continue;
		}
		if(msg3.type == GROUP)
		{
			printf("\033[0;31m\t\t%s\t:%s(group)\033[0;m\n",msg3.text,msg3.name);
			continue;
		}

		if(msg3.type == QUIT)
		{
			printf("\n");
			printf("\033[0;31m异地登录，被迫下线！\033[0;m\n");
			if(msgctl(msgqid,IPC_RMID,0) == -1)
			{
				fprintf(stderr,"%s\n",strerror(errno));
			}
			exit(1);
		}
		msg_send(msg3);
	}
	return NULL;
}

void msg_send(Cmsg msg3)//消息队列发送消息
{
//	printf("正在发送消息队列给主线程\n");
	Qmsg qmsg2 = {};
	qmsg2.type = msg3.type;
	strcpy(qmsg2.text,msg3.text);
	if(msgsnd(msgqid,&qmsg2,sizeof(Qmsg),0) == -1)
	{
		fprintf(stderr,"文件名：%s 函数名：%s 行号：%d 错误原因：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;
	}
}

