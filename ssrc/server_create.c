#include "mychat_model.h"
#include "mychat_ctrl.h"
#include "all_head.h"
char* get_array(char* str ,char* filename,char* value)//IP,port获取函数：成功返回str，失败返回NULL
{
	FILE* fp = fopen(filename,"r");
	if(fp == NULL)
	{
		return NULL;
	}

	while(1)
	{
		char buff_line[1024] = " ";
		char buff_1[1024] = " ";
		char buff_2[1024] = " ";
		char buff_3[1024] = " ";
		if(fgets(buff_line,1024,fp) == NULL)
		{
			break;
		} 	
		sscanf(buff_line,"%s %s %s",buff_1,buff_2,buff_3);
		if(strcmp(buff_1,value) == 0)
		{
			fclose(fp);
			strcpy(str,buff_3);
			return str;
		}
	}	
	fclose(fp);
	return NULL;
}

void server_create(char* ip, char* port)//服务器创建函数
{
	int serverfd = socket(AF_INET,SOCK_STREAM,0);//创建套接字
	if(serverfd == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in s_addr = {};//定义服务器因特网地址结构体
	s_addr.sin_family = AF_INET;//设置网域
	s_addr.sin_port = htons(atoi(port));//设置端口
	s_addr.sin_addr.s_addr = INADDR_ANY;

/*	if(inet_pton(AF_INET,ip,&s_addr.sin_addr.s_addr) == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);	
	}
*/
	if(bind(serverfd,(struct sockaddr*)&s_addr,sizeof(s_addr)) == -1)//服务器绑定IP,port
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if(listen(serverfd,10) == -1)//设置监听队列
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);
	}

	//创建心跳包处理线程
	pthread_t pthread_deal_heart;
	pthread_attr_t attr2;
	pthread_attr_init(&attr2);
	pthread_attr_setdetachstate(&attr2,PTHREAD_CREATE_DETACHED);
	if(pthread_create(&pthread_deal_heart,&attr2,deal_heart,(void*)NULL) != 0)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		struct sockaddr_in client_addr;//用来接收客户端因特网地址
		socklen_t client_len = sizeof(client_addr);
		int clientfd = accept(serverfd,(struct sockaddr*)&client_addr,&client_len);
		if(clientfd == -1)
		{
			fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
			exit(EXIT_FAILURE);
		}

		char ip_buffer[1024] = "";
		printf("IP:%s\tID:%d\t链接服务器成功！\n",inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ip_buffer,1024),clientfd);


		//开辟线程对客服端做点对点服务
		pthread_t pthread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

		int err = 0;
		if((err = pthread_create(&pthread,&attr,server_receive,(void*)clientfd)) != 0)
		{
			fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
			exit(EXIT_FAILURE);
		}

		//  保存客户端信息
		strcpy(g_data.ip,inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ip_buffer,1024));
		g_data.clientfd = clientfd;
		g_data.pth_id = pthread;
		g_data.second = (int)time(NULL);

	}
}

void* deal_heart(void* arg)
{
	int i = 0;
	int t = 0;
	Onlink* save = onlink;
	while(1)
	{
		sleep(3);
		t = (int)time(NULL);
		save = onlink;
		//遍历在线用户链表
		if(save->next == NULL)
		{
			printf("无在线用户\n");
			++i;
			if(i == 10)
			{
				//处理内存释放
				free(onlink);
				userlink_free(userlink);
				printf("服务器关闭\n");
				exit(0);
			}
			continue;
		}
		i = 0;
		Onlink* pre = save;
		save = save->next;
		while(save != NULL)
		{
			if((t-save->data.second) > 15)
			{
				Onlink* del = pre->next;
				pre->next = del->next;
				free(del);
				printf("删除此在线用户节点\n");
				close(save->data.clientfd);
				if(pthread_cancel(save->data.pth_id) != 0)
				{
					fprintf(stderr,"%s\n",strerror(errno));
					break;
				}
				printf("结束点对点服务线程\n");
				break;
			}
			pre = save;
			save = save->next;
		}
	}
	return NULL;
}

void* server_receive(void* arg)//服务器数据接收线程
{
	Online net_message = g_data;
	int clientfd = (int)arg;

	//调用服务器接收线程控制层
	server_ctrl_1(clientfd,net_message);

	close(clientfd);
	return NULL;
}
