#include "all_head.h"
#include "mychat_model.h"
#include "mychat_ctrl.h"

void enter_wait(void)
{
	sleep(1);
	printf("\033[0;35m按回车以继续...\033[0;m\n");
	while(getchar() != '\n');
}
void client_user_exit(int clientfd, char* name)//客户端退出
{
	Cmsg exit_message = {};
	exit_message.type = EXIT;
	strcpy(exit_message.name,name);
	if(write(clientfd,&exit_message,sizeof(Cmsg)) <= 0)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return;
	}
	if(msgctl(msgqid,IPC_RMID,0) == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;
	}
	close(clientfd);
}
void client_user_regist(int clientfd)//客户端用户名注册函数
{
	Cmsg msg1 = {};
	msg1.type = REG_J;
	printf("\033[0;34m请输入您要注册的用户名：\033[0;m");
	scanf("%s",msg1.name);
	if(write(clientfd,&msg1,sizeof(msg1)) <= 0)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return;
	}
	Qmsg qmsg1 = {};
	printf("等待读取线程同步消息\n");
	sleep(1);
	if(msgrcv(msgqid,&qmsg1,sizeof(Qmsg),1, 0) == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;
	}
	if(strcmp(qmsg1.text,"此用户已存在") == 0)
	{
		printf("\033[0;31m%s!\033[0;m\n",qmsg1.text);
		enter_wait();
		return;
	}
	else if(strcmp(qmsg1.text,"此用户名可用") == 0)
	{
		Cmsg msg4 = {};
		msg4.type = REG_P;
		strcpy(msg4.name,msg1.name);
		printf("请输入密码：");
		scanf("%s",msg4.password);
		if(write(clientfd,&msg4,sizeof(msg4)) <= 0)
		{
			fprintf(stderr,"%s\n",strerror(errno));
			return;
		}
	}
	else
	{
		printf("error\n");
	}

	if(msgrcv(msgqid,&qmsg1,sizeof(Qmsg), 10, 0) == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;
	}
	printf("%s\n",qmsg1.text);
	enter_wait();
}

void client_user_login(int clientfd,char* name)//客户端用户登录函数
{
	int count = 0;
	while(count < 3)
	{
		Cmsg login_message = {};
		login_message.type = LOG;
		printf("\033[0;34m请输入用户名：\033[0;m");
		scanf("%s",login_message.name);
		printf("\033[0;34m请输入密码\033[0;m：");
		scanf("%s",login_message.password);
		if(write(clientfd,&login_message,sizeof(Cmsg)) <= 0)
		{
			fprintf(stderr,"%s\n",strerror(errno));
			return;
		}
		Qmsg qmsg = {};
		if(msgrcv(msgqid,&qmsg,sizeof(Qmsg), 2 ,0) == -1)
		{
			fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
			return ;
		}

		if(strcmp(qmsg.text,"此用户不存在") == 0)
		{
			printf("\033[0;31m%s!\033[0;m\n",qmsg.text);
			enter_wait();
			return ;
		}
		else if(strcmp(qmsg.text,"密码错误") == 0)
		{
			printf("\033[0;31m%s!\033[0;m\n",qmsg.text);
			enter_wait();
			count++;
		}
		else if(strcmp(qmsg.text,"登录成功") == 0 || strcmp(qmsg.text,"此账户已登录，将此在线账户成功代替") == 0)
		{
			printf("\033[0;34m%s!\033[0;m\n",qmsg.text);
		//进入二级界面
	//		printf("进入二级界面！\n");
			getchar();
			enter_wait();
			strcpy(name,login_message.name);
			client_ctrl_2(clientfd,login_message.name);
			return;
		}
	}
	printf("\033[0;31m三次密码错误，登录失败！\033[0;m\n");
	enter_wait();
	return ;
}
void client_user_quit(int clientfd, char* name)
{
	Cmsg quit_message = {};
	quit_message.type = QUIT;
	strcpy(quit_message.name,name);
	if(write(clientfd,&quit_message,sizeof(Cmsg)) <= 0)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return;
	}
	if(pthread_cancel(pthread_heart) != 0)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return;
	}
	printf("\033[0;31m心跳线程已终止\033[0;m\n");
}

void client_user_shell(int clientfd, char* name)//客户端shell命令
{
//	printf("进入shell命令函数\n");
	Cmsg shell_message = {};

	shell_message.type = SHELL;
	strcpy(shell_message.name,name);
	printf("请输入shell命令:");
	gets(shell_message.shell);
	if(write(clientfd,&shell_message,sizeof(Cmsg)) <= 0)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return;
	}

	enter_wait();
}

void client_user_check_online(int clientfd, char* name)//客户端查看在线用户
{
	Cmsg check_message = {};
	check_message.type = CHECK;
	strcpy(check_message.name,name);
	if(write(clientfd,&check_message,sizeof(Cmsg)) <= 0)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return;
	}

	Qmsg qmsg = {};
	if(msgrcv(msgqid,&qmsg,sizeof(Qmsg),4, 0) == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;	
	}
	printf("\033[0;34m%s\033[0;m\n",qmsg.text);
	enter_wait();
}

void client_user_chat(int clientfd, char* name)
{
	Cmsg chat_message = {};
	chat_message.type = CHAT;
	strcpy(chat_message.name,name);
	printf("\033[0;36m请输入聊天对象姓名：\033[0;m");
	scanf("%s",chat_message.chat_name);
	getchar();
	printf("\033[0;36mmychat:\ninput 'end' to exit\033[0;m\n");
	while(1)
	{
		gets(chat_message.text);
		if(strcmp(chat_message.text,"end") == 0)
		{
			return;
		}
		if(write(clientfd,&chat_message,sizeof(Cmsg)) <= 0)
		{
			fprintf(stderr,"%s\n",strerror(errno));
			return;
		}
	}
}

void client_user_multichat(int clientfd, char* name)
{
	Cmsg mulchat_message = {};
	mulchat_message.type = GROUP;
	strcpy(mulchat_message.name,name);
	printf("\033[0;36mgroup chat:\ninput 'end' to exit\033[0;m\n");
	while(1)
	{
		printf("\033[0;31m");
		gets(mulchat_message.text);
		printf("\033[0;m");
		if(strcmp(mulchat_message.text,"end") == 0)
		{
			return;
		}
		if(write(clientfd,&mulchat_message,sizeof(Cmsg)) <= 0)
		{
			fprintf(stderr,"%s\n",strerror(errno));
			return;	
		}
	}
}
